/**
 * @brief Offboard controller
 * @file offboard_control_srv.cpp
 */

#include "offboard_control/Inner_loop_geometric_PID.h" // revised PID control
#include "offboard_control/QSF_w_offset_intctrl_U.h"
#include "offboard_control/QSF_w_offset_intctrl.h"
#include "offboard_control/diff_flatness_mission_QSF.h"
#include "offboard_control/Flatness_mission_spfig8.h"
#include "px4_ros_com/frame_transforms.h"

#include <nav_msgs/msg/odometry.hpp> 
#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/vehicle_attitude.hpp>
#include <px4_msgs/msg/vehicle_attitude_setpoint.hpp>
#include <px4_msgs/msg/vehicle_local_position.hpp>
#include <px4_msgs/msg/vehicle_odometry.hpp>
#include <px4_msgs/msg/vehicle_rates_setpoint.hpp>
#include <px4_msgs/msg/vehicle_status.hpp>
#include <px4_msgs/msg/vehicle_thrust_setpoint.hpp>
#include <px4_msgs/msg/vehicle_torque_setpoint.hpp>

#include <rclcpp/rclcpp.hpp>
#include <stdint.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>

// #include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose_array.hpp>
#include <geometry_msgs/msg/wrench_stamped.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace px4_msgs::msg;

#define FD_EPSILON DBL_MIN

class OffboardControl : public rclcpp::Node {
  public:
    OffboardControl(std::string px4_namespace)
        : Node("offboard_control_srv"), control_mode_("position"), 
        kp_(2.0), kv_(1.5), ki_(0.0), mass_(2.0232), // 2.093 for exp, 2.0232 for sim
        // make sure mass & hover thrust are defined                                                                 
        // before using the se3 controller
          hover_thrust_(0.5) {
        // Declare Parameters
        control_mode_ = this->declare_parameter<std::string>("control_mode", control_mode_);
        kp_ = this->declare_parameter<double>("Kp", kp_);
        kv_ = this->declare_parameter<double>("Kv", kv_);
        ki_ = this->declare_parameter<double>("Ki", ki_); // PID for Lee's control
        mass_ = this->declare_parameter<double>("mass", mass_); 
        hover_thrust_ = this->declare_parameter<double>("hover_thrust", hover_thrust_);
        yaw_ = this->declare_parameter<double>("yaw_", 0.0);
        attitude_tau_ = this->declare_parameter<double>("attitude_tau_", 0.1);                 
        norm_thrust_const_ = this->declare_parameter<double>("norm_thrust_const_", 0.03749);  // 0.034436
        norm_thrust_offset_ = this->declare_parameter<double>("norm_thrust_offset_", 0.22043); // 0.14344
        calib_thrust_ = this->declare_parameter<double>("calib_thrust_", 0.54);
        ref_rate_limit_ = this->declare_parameter<double>("ref_rate_limit_", 1);
        att_control_type_ = this->declare_parameter<std::string>("att_control_type_", "Lee"); // "Default", "QSF_offset", "Lee" (For later)

        // QSF gains
        sls_offset_params_.Kx_int = this->declare_parameter<double>("Kx_int", 10.0000);
        sls_offset_params_.Kx_pos = this->declare_parameter<double>("Kx_pos", 44.3906);
        sls_offset_params_.Kx_vel = this->declare_parameter<double>("Kx_vel", 48.5263);
        sls_offset_params_.Kx_acc = this->declare_parameter<double>("Kx_acc", 27.2053);
        sls_offset_params_.Kx_jerk = this->declare_parameter<double>("Kx_jerk", 8.0256);
        sls_offset_params_.Ky_int = this->declare_parameter<double>("Ky_int", 10.0);
        sls_offset_params_.Ky_pos = this->declare_parameter<double>("Ky_pos", 44.3906);
        sls_offset_params_.Ky_vel = this->declare_parameter<double>("Ky_vel", 48.5263);
        sls_offset_params_.Ky_acc = this->declare_parameter<double>("Ky_acc", 27.2053);
        sls_offset_params_.Ky_jerk = this->declare_parameter<double>("Ky_jerk", 8.0256);
        sls_offset_params_.Kz_int = this->declare_parameter<double>("Kz_int", 1.0000);
        sls_offset_params_.Kz_pos = this->declare_parameter<double>("Kz_pos", 4.0);
        sls_offset_params_.Kz_vel = this->declare_parameter<double>("Kz_vel", 3.0);

        // Determines vicon versus sim for data, and whether to use EKF for UAV state:
        use_sim_ = this->declare_parameter<bool>("use_sim", true);
        use_ekf_ = this->declare_parameter<bool>("use_ekf", false);
        
        std::string uav_topic = use_sim_ ? "/model/px4vision_sls_0/odometry_with_covariance" : "/vicon/F450_1/odom";
        std::string load_topic = use_sim_ ? "/model/px4vision_sls_0/load_odom" : "/vicon/load_1/odom";
        
        RCLCPP_INFO(this->get_logger(), "Running in %s mode.", use_sim_ ? "SIMULATION" : "EXPERIMENT");
        RCLCPP_INFO(this->get_logger(), "Using EKF: %s", use_ekf_ ? "TRUE" : "FALSE");

        // Inertia matrix parameters
        sls_offset_params_.Iqxx = this->declare_parameter<double>("Iqxx", 0.020653500000000005); // 0.020653500000000005
        sls_offset_params_.Iqyy = this->declare_parameter<double>("Iqyy", 0.020653500000000005); // 0.020653500000000005
        sls_offset_params_.Iqzz = this->declare_parameter<double>("Iqzz", 0.04046400000000001); // 0.04046400000000001
        // double Iqxx = 0.02091; // experiment values
        // double Iqyy = 0.02091;
        // double Iqzz = 0.02934;

        // Geometric controller gains
        // 0 < cI < min(sqrt(kR/Iqxx)/Iqzz, 4*kR*kOmega/(4*kR*Iqzz + kOmega^2))
        kR_ = this->declare_parameter<double>("kR_", 2.5); // 5.0(sim), 2.5(exp)
        kOmega_ = this->declare_parameter<double>("kOmega_", 0.35); //0.8(sim), 0.35(exp)
        kI_ = this->declare_parameter<double>("kI_", 0.0); // 0.1 
        cI_ = this->declare_parameter<double>("cI_", 0.0); // 0.5 

        // SLS offset Max torque
        // sls_offset_params_.tau_x_max_ = this->declare_parameter<double>("tau_x_max_", 4.15*2.21356); // 4.15*3.21356 for exp
        // sls_offset_params_.tau_y_max_ = this->declare_parameter<double>("tau_y_max_", 4.15*2.21356);
        // sls_offset_params_.tau_z_max_ = this->declare_parameter<double>("tau_z_max_", 0.35); // 2.5 for exp, 0.35 for sim

        // Initialize gain matrices
        K_p_ = kp_ * Eigen::Matrix3d::Identity();
        K_v_ = kv_ * Eigen::Matrix3d::Identity();
        K_i_ = ki_ * Eigen::Matrix3d::Identity();

        // Initialize the flight path parameter
        flight_path_ = this->declare_parameter<std::string>("flight_path", "hover");

        // Setup Parameter Callback
        param_callback_handle_ = this->add_on_set_parameters_callback(std::bind(&OffboardControl::parameters_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Starting Offboard Control Node in '%s' mode.", control_mode_.c_str());

        // Publishers
        offboard_control_mode_publisher_ = this->create_publisher<OffboardControlMode>(px4_namespace + "in/offboard_control_mode", rclcpp::SensorDataQoS());
        attitude_setpoint_publisher_ = this->create_publisher<VehicleAttitudeSetpoint>(px4_namespace + "in/vehicle_attitude_setpoint", rclcpp::SensorDataQoS());
        trajectory_setpoint_publisher_ = this->create_publisher<TrajectorySetpoint>(px4_namespace + "in/trajectory_setpoint", rclcpp::SensorDataQoS());
        debug_trajectory_setpoint_publisher_ = this->create_publisher<TrajectorySetpoint>("debug/trajectory_setpoint", rclcpp::SensorDataQoS());
        rate_setpoint_publisher_ = this->create_publisher<VehicleRatesSetpoint>(px4_namespace + "in/vehicle_rates_setpoint", rclcpp::SensorDataQoS());
        torque_setpoint_publisher_ = this->create_publisher<VehicleTorqueSetpoint>(px4_namespace + "in/vehicle_torque_setpoint", rclcpp::SensorDataQoS());
        thrust_setpoint_publisher_ = this->create_publisher<VehicleThrustSetpoint>(px4_namespace + "in/vehicle_thrust_setpoint", rclcpp::SensorDataQoS());
        qsf_attitude_debug_publisher_ = this->create_publisher<geometry_msgs::msg::PoseArray>("debug/qsf_attitude", 10);
        controller_output_debug_publisher_ = this->create_publisher<geometry_msgs::msg::WrenchStamped>("debug/qsf_controller_output", 10);
        inner_integral_debug_publisher_ =this->create_publisher<geometry_msgs::msg::Vector3Stamped>("/debug/qsf_inner_integral", 10);

        // Subscribers
        vehicle_local_position_subscriber_ = this->create_subscription<px4_msgs::msg::VehicleLocalPosition>(
            px4_namespace + "out/vehicle_local_position", rclcpp::SensorDataQoS(), [this](const px4_msgs::msg::VehicleLocalPosition::SharedPtr msg) {
                if (!msg->xy_valid || !msg->z_valid || !msg->v_xy_valid || !msg->v_z_valid) {
                    RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "EKF2 Local position invalid. Ignoring.");
                    return;
                }
                latest_local_pos_ = *msg;
                pos_received_ = true;

                publish_offboard_control_mode();

                if (is_offboard_) {
                    if (control_mode_ == "se3") {
                        // publish_se3_setpoint();
                    } else {
                        publish_trajectory_setpoint();
                    }
                }
            });

        trajectory_ref_subscriber_ = this->create_subscription<TrajectorySetpoint>("custom/trajectory_reference", 10, [this](const TrajectorySetpoint::SharedPtr msg) {
            latest_ref_ = *msg;
            ref_received_ = true;
        });

        // NOTE: using the _v1 topic to match PX4's internal uORB updates
        vehicle_status_subscriber_ =
            this->create_subscription<px4_msgs::msg::VehicleStatus>("/fmu/out/vehicle_status_v1", rclcpp::SensorDataQoS(), [this](const px4_msgs::msg::VehicleStatus::SharedPtr msg) {
                bool was_offboard = is_offboard_;
                is_offboard_ = (msg->nav_state == px4_msgs::msg::VehicleStatus::NAVIGATION_STATE_OFFBOARD);

                if (is_offboard_ && !was_offboard) {
                    RCLCPP_INFO(this->get_logger(), "Offboard mode engaged.");
                    start_time_ = this->now();
                    // Reset integral when entering offboard mode
                    reset_qsf_integral_ = true;
                    reset_position_integral_ = true;
                    reset_inner_integral_ = true; 
                    RCLCPP_INFO(this->get_logger(), "Resetting integral state for QSF offset controller.");
                } else if (!is_offboard_ && was_offboard) {
                    RCLCPP_INFO(this->get_logger(), "Offboard mode disengaged.");
                }
            });

        // use FMU odometry
        vehicle_odometry_subscriber_ =
            this->create_subscription<px4_msgs::msg::VehicleOdometry>("/fmu/out/vehicle_odometry", rclcpp::SensorDataQoS(), [this](const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
                if (!use_ekf_) return; // ignore if relying on external vision

                if (std::isnan(msg->q[0]) || std::isnan(msg->velocity[0])) {
                    RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "VehicleOdometry contains NaN. EKF not ready.");
                    return;
                }

                // When frame == 1, it is in NED
                if ((msg->pose_frame != 1) || (msg->velocity_frame != 1)) {
                    RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Bad frame for vehicle_odometry, expected 1 (NED) for velocity and position, got %d and %d. Ignoring.",
                                         msg->pose_frame, msg->velocity_frame);
                    return;
                }
                // Get UAV pose, converted to ENU
                sls_offset_params_.latest_pos_enu_ = Eigen::Vector3d(msg->position[1], msg->position[0], -msg->position[2]);

                // Get UAV attitude estimate
                Eigen::Quaterniond q_ned(msg->q[0], msg->q[1], msg->q[2], msg->q[3]);
                Eigen::Quaterniond q_enu = px4_ros_com::frame_transforms::px4_to_ros_orientation(q_ned);
                latest_attitude_(0) = q_enu.w();
                latest_attitude_(1) = q_enu.x();
                latest_attitude_(2) = q_enu.y();
                latest_attitude_(3) = q_enu.z();
                attitude_received_ = true;

                // msg->velocity is already in the world frame (NED). Just map to ENU.
                sls_offset_params_.latest_vel_enu_ = Eigen::Vector3d(msg->velocity[1], msg->velocity[0], -msg->velocity[2]);
                // FRD angular rates
                sls_offset_params_.latest_rate_frd_ = Eigen::Vector3d(msg->angular_velocity[0], msg->angular_velocity[1], msg->angular_velocity[2]);
            });

        // gz and vicon odometry subscribers
        uav_odom_sub_ =
            this->create_subscription<nav_msgs::msg::Odometry>(uav_topic, rclcpp::SensorDataQoS(), [this](const nav_msgs::msg::Odometry::SharedPtr msg) {
                // //current_sim_time_ = msg->header.stamp.sec + msg->header.stamp.nanosec * 1e-9;

                if (use_ekf_) return; // ignore if using EKF measurements
                
                // Drone Position & Attitude
                sls_offset_params_.latest_pos_enu_ << msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z;
                latest_attitude_(0) = msg->pose.pose.orientation.w;
                latest_attitude_(1) = msg->pose.pose.orientation.x;
                latest_attitude_(2) = msg->pose.pose.orientation.y;
                latest_attitude_(3) = msg->pose.pose.orientation.z;

                Eigen::Vector3d lin_vel(msg->twist.twist.linear.x, msg->twist.twist.linear.y, msg->twist.twist.linear.z);
                Eigen::Vector3d ang_vel(msg->twist.twist.angular.x, msg->twist.twist.angular.y, msg->twist.twist.angular.z);

                if (use_sim_) {
                    // Rotate Body Frame Twist to World Frame (ENU)
                    Eigen::Quaterniond q_world(msg->pose.pose.orientation.w, msg->pose.pose.orientation.x, 
                                            msg->pose.pose.orientation.y, msg->pose.pose.orientation.z);
                    Eigen::Matrix3d R_body_to_world = q_world.toRotationMatrix();
                    
                    sls_offset_params_.latest_vel_enu_ = R_body_to_world * lin_vel;
                    sls_offset_params_.latest_rate_frd_ = Eigen::Vector3d(ang_vel.x(), -ang_vel.y(), -ang_vel.z()); // FLU -> FRD
                } else {
                    // Vicon provides velocities in world frame (ENU)
                    sls_offset_params_.latest_vel_enu_ = lin_vel;
                    // sls_offset_params_.latest_rate_frd_ = Eigen::Vector3d(ang_vel.y(), ang_vel.x(), -ang_vel.z()); // original needs ENU -> NED
                    // ENU -> FRD angular rates (newly fixed)
                    Eigen::Quaterniond q_world(msg->pose.pose.orientation.w, msg->pose.pose.orientation.x,
                                               msg->pose.pose.orientation.y, msg->pose.pose.orientation.z);
                    Eigen::Matrix3d R_body_to_world = q_world.toRotationMatrix();
                    Eigen::Vector3d omega_body_flu = R_body_to_world.transpose() * ang_vel;
                    sls_offset_params_.latest_rate_frd_ = Eigen::Vector3d(omega_body_flu.x(), -omega_body_flu.y(), -omega_body_flu.z());
                }
                attitude_received_ = true;
            });

        // gz and vicon load odometry subscribers
        load_odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(load_topic, rclcpp::SensorDataQoS(), [this](const nav_msgs::msg::Odometry::SharedPtr msg) {
            // Load Position
            sls_offset_params_.load_pos_enu_ << msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z;

            Eigen::Vector3d lin_vel(msg->twist.twist.linear.x, msg->twist.twist.linear.y, msg->twist.twist.linear.z);
            Eigen::Vector3d ang_vel(msg->twist.twist.angular.x, msg->twist.twist.angular.y, msg->twist.twist.angular.z);

            if (use_sim_) {
                // Rotate Load Body Frame Twist to World Frame (ENU)
                Eigen::Quaterniond q_load(msg->pose.pose.orientation.w, msg->pose.pose.orientation.x, 
                                        msg->pose.pose.orientation.y, msg->pose.pose.orientation.z);
                Eigen::Matrix3d R_load_body_to_world = q_load.toRotationMatrix();

                sls_offset_params_.load_vel_enu_ = R_load_body_to_world * lin_vel;
                sls_offset_params_.load_rate_enu_ = R_load_body_to_world * ang_vel;
            } else {
                // Vicon provides velocities in world frame
                sls_offset_params_.load_vel_enu_ = lin_vel;
                sls_offset_params_.load_rate_enu_ = ang_vel;
            }

            sls_offset_params_.load_received_ = true;
        });
    }

  private:
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_callback_handle_;

    // Publishers & Subscribers
    rclcpp::Publisher<OffboardControlMode>::SharedPtr offboard_control_mode_publisher_;
    rclcpp::Publisher<VehicleAttitudeSetpoint>::SharedPtr attitude_setpoint_publisher_;
    rclcpp::Publisher<VehicleRatesSetpoint>::SharedPtr rate_setpoint_publisher_;
    rclcpp::Publisher<VehicleTorqueSetpoint>::SharedPtr torque_setpoint_publisher_;
    rclcpp::Publisher<VehicleThrustSetpoint>::SharedPtr thrust_setpoint_publisher_;
    rclcpp::Publisher<geometry_msgs::msg::PoseArray>::SharedPtr qsf_attitude_debug_publisher_; // attitude tracking debug
    rclcpp::Publisher<geometry_msgs::msg::WrenchStamped>::SharedPtr controller_output_debug_publisher_; // controller output debug
    rclcpp::Publisher<geometry_msgs::msg::Vector3Stamped>::SharedPtr inner_integral_debug_publisher_;
    rclcpp::Publisher<TrajectorySetpoint>::SharedPtr trajectory_setpoint_publisher_;
    rclcpp::Publisher<TrajectorySetpoint>::SharedPtr debug_trajectory_setpoint_publisher_;

    rclcpp::Subscription<px4_msgs::msg::VehicleLocalPosition>::SharedPtr vehicle_local_position_subscriber_;
    rclcpp::Subscription<TrajectorySetpoint>::SharedPtr trajectory_ref_subscriber_;
    rclcpp::Subscription<px4_msgs::msg::VehicleStatus>::SharedPtr vehicle_status_subscriber_;
    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr vehicle_odometry_subscriber_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr uav_odom_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr load_odom_sub_;

    // State Variables
    px4_msgs::msg::VehicleLocalPosition latest_local_pos_{};
    TrajectorySetpoint latest_ref_;
    Eigen::Vector3d latest_ref_snap_{}; // Fourth time derivative of position vector
    bool pos_received_{false};
    bool ref_received_{false};
    bool is_offboard_{false};
    rclcpp::Time start_time_;

    // Control Parameters
    std::string control_mode_;
    std::string flight_path_;

    // Lee's PID control
    double kp_,kv_,ki_;
    Eigen::Matrix3d K_p_,K_v_,K_i_;    
    Eigen::Vector3d position_integral_{Eigen::Vector3d::Zero()};
    rclcpp::Time last_position_integral_time_;
    bool first_position_integral_call_{true};
    bool reset_position_integral_{false};
    const double position_integral_limit_ = 5.0; // m*s, anti-windup

    double mass_;
    double hover_thrust_;
    const double gravity_ = 9.8066;
    std::string att_control_type_; // "Default", "QSF_offset", "Lee" (For later)
    double kR_;
    double kOmega_;
    double kI_;
    double cI_;

    // Data Source Toggles
    bool use_sim_{true};
    bool use_ekf_{false};
    bool reset_qsf_integral_{false};
    bool reset_inner_integral_{false};

    // SLS Offset Gains
    struct sls_offset_params {
        // QSF gains
        double Kx_int = 0.0, Kx_pos = 31.6228, Kx_vel = 40.9156, Kx_acc = 24.8885, Kx_jerk = 7.7316, Ky_int = 0.0, Ky_pos = 31.6228, Ky_vel = 40.9156, Ky_acc = 24.8885, Ky_jerk = 7.7316, Kz_int = 1.0,
               Kz_pos = 4.0, Kz_vel = 3.0;

        // SLS offset parameters and state variables
        Eigen::Vector3d latest_pos_enu_{}, latest_vel_enu_{}, latest_rate_enu_{}, load_pos_enu_{}, load_vel_enu_{}, load_rate_enu_{}, pend_rate_enu_{}, pend_angle_enu_{}, latest_rate_frd_{};
        bool load_received_{false};
        double load_mass_ = 0.191; // 0.191 kg for experiment
        double R_bi[9];
        Eigen::Matrix3d R_Bd;                      // Desired UAV attitude
        double l = 0.75; // 0.92, Cable length
        double q_vec[3] = {0.0, 0.0, 0.0};
        double dq[3] = {0.0, 0.0, 0.0};
        // double L_offset_[3] = {0.12, -0.12, 0.06}; // Offset of load from UAV in meters (FRD) (x, -y, -z)
        double L_offset_[3] = {0.0, 0.0, 0.0}; // for 0 offset or Lee's control
        double phi_rad_, theta_rad_, psi_rad_;
        // double tau_x_max_, tau_y_max_, tau_z_max_;

        // Inner loop tracking variables (if using thrust-torque control)
        double Td_scaler = 1.0;
        double des_thrust = 0.0;
        double Omegad1 = 0.0, Omegad2 = 0.0, Omegad3 = 0.0;
        double dOmegad1 = 0.0, dOmegad2 = 0.0, dOmegad3 = 0.0;
        double aLd1 = 0.0, aLd2 = 0.0, aLd3 = 0.0;
        double snapd1 = 0.0, snapd2 = 0.0, snapd3 = 0.0;
        double integral[3] = {0.0, 0.0, 0.0};
        double Iqxx; // 
        double Iqyy; // 
        double Iqzz;  // 
    } sls_offset_params_;

    struct sls_offset_ned_params {
        Eigen::Vector3d load_pos{}, load_vel{}, load_rate{};
        double q[3];
    };

    // thrust calibration parameter
    double calib_thrust_;
    double norm_thrust_const_, norm_thrust_offset_;

    // thrust and torque normalization parameters
    double lee_sitl_normalized_thrust_{0.0};

    // Attitude and Rate Mode Specific Parameters
    bool attitude_received_{false};
    double yaw_, ref_rate_limit_;
    double attitude_tau_; // Attitude time constant for body rate control
    Eigen::Vector4d latest_attitude_;

    // Methods
    rcl_interfaces::msg::SetParametersResult parameters_callback(const std::vector<rclcpp::Parameter> &parameters);
    void publish_offboard_control_mode();
    void publish_trajectory_setpoint();
    void publish_se3_setpoint(sls_offset_params &sls_offset_params);
    void timer_callback(void);
    Eigen::Vector3d compute_acceleration_command(const Eigen::Vector3d &p, const Eigen::Vector3d &v, const Eigen::Vector3d &p_d, const Eigen::Vector3d &v_d, const Eigen::Vector3d &a_d);

    // Eigen::Vector3d apply_QSF_integral_ctrl(const Eigen::Vector3d &p_ref);

    // QSF offset methods
    std::tuple<Eigen::Vector4d, std::pair<Eigen::Vector3d, double>, Eigen::Vector3d> apply_QSF_offset_ctrl(const Eigen::Vector3d &pos_des_ned, const Eigen::Vector3d &vel_des_ned,
                                                                                                           const Eigen::Vector3d &acc_des_ned, const Eigen::Vector3d &jerk_des_ned,
                                                                                                           const Eigen::Vector3d &snap_des_ned);
    Eigen::Vector3d sls_offset_thrust_torque_inner_loop(double thrust_command);
    // std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d> sls_offset_differential_flatness();
    void sls_offset_differential_flatness();
    sls_offset_ned_params sls_offset_enu_to_ned(sls_offset_params &sls_offset_params);
    std::pair<Eigen::Vector3d, double> sls_offset_attitude_to_body_rate_and_thrust(const Eigen::Vector4d &curr_att, const Eigen::Vector4d &ref_att, double ref_z_thrust);

    // Attitude and Rate Mode specific methods
    std::pair<Eigen::Vector3d, double> attitude_to_body_rate_and_thrust(const Eigen::Vector4d &curr_att, const Eigen::Vector4d &ref_att, const Eigen::Vector3d &ref_acc);
    Eigen::Vector4d acceleration_to_quaternion(const Eigen::Vector3d &vector_acc, const double &yaw);
    void publish_attitude_setpoints(const double &thrust_cmd, const Eigen::Vector4d &target_attitude_enu);
    void publish_rate_setpoints(const Eigen::Vector3d &rate_cmd_flu, const double &thrust_cmd);
    void publish_torque_thrust_setpoints(const Eigen::Vector3d &torque_cmd, double thrust_cmd);
    inline Eigen::Vector4d rotation_matrix_to_quaternion(const Eigen::Matrix3d &R);
    inline Eigen::Matrix3d quaternion_to_rotation_matrix(const Eigen::Vector4d &q);
    inline Eigen::Vector4d multiply_quaternion(const Eigen::Vector4d &q, const Eigen::Vector4d &p);
    void publish_qsf_attitude_debug();
    void publish_qsf_controller_output(double des_thrust, const double tau[3]);
    void publish_inner_integral_debug(const double eI[3]);
    std::pair<Eigen::Vector3d, double> f450_px4_inverse_sitl(const Eigen::Vector3d &tau_nm, double thrust_n); // F450 physical wrench -> PX4 normalized torque/thrust
};

/**
 * @brief Handle dynamic parameter updates efficiently
 */
rcl_interfaces::msg::SetParametersResult OffboardControl::parameters_callback(const std::vector<rclcpp::Parameter> &parameters) {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;

    for (const auto &param : parameters) {
        if (param.get_name() == "control_mode")
            control_mode_ = param.as_string();
        else if (param.get_name() == "Kp") {
            kp_ = param.as_double();
            K_p_ = kp_ * Eigen::Matrix3d::Identity();
        } else if (param.get_name() == "Kv") {
            kv_ = param.as_double();
            K_v_ = kv_ * Eigen::Matrix3d::Identity();
        } else if (param.get_name() == "Ki") {
            ki_ = param.as_double();
            K_i_ = ki_ * Eigen::Matrix3d::Identity();
            // avoid a thrust jump when changing/enabling Ki
            position_integral_.setZero();
            first_position_integral_call_ = true;
        } 

        else if (param.get_name() == "mass")
            mass_ = param.as_double();
        else if (param.get_name() == "hover_thrust")
            hover_thrust_ = param.as_double();
        else if (param.get_name() == "yaw_")
            yaw_ = param.as_double();
        else if (param.get_name() == "attitude_tau_")
            attitude_tau_ = param.as_double();
        else if (param.get_name() == "norm_thrust_const_")
            norm_thrust_const_ = param.as_double();
        else if (param.get_name() == "norm_thrust_offset_")
            norm_thrust_offset_ = param.as_double();
        else if (param.get_name() == "ref_rate_limit_")
            ref_rate_limit_ = param.as_double();
        else if (param.get_name() == "att_control_type_")
            att_control_type_ = param.as_string();
        else if (param.get_name() == "flight_path") {
            std::string new_flight_path = param.as_string();
            // Start the differential-flatness mission clock
            // when switching into setpoints_figure8.
            if (
                new_flight_path == "setpoints_figure8"
                && flight_path_ != "setpoints_figure8"
            ) {
                start_time_ = this->now();
            }

            flight_path_ = new_flight_path;
        }

        else if (param.get_name() == "calib_thrust_")
            calib_thrust_ = param.as_double();

        else if (param.get_name() == "Kx_int")
            sls_offset_params_.Kx_int = param.as_double();
        else if (param.get_name() == "Kx_pos")
            sls_offset_params_.Kx_pos = param.as_double();
        else if (param.get_name() == "Kx_vel")
            sls_offset_params_.Kx_vel = param.as_double();
        else if (param.get_name() == "Kx_acc")
            sls_offset_params_.Kx_acc = param.as_double();
        else if (param.get_name() == "Kx_jerk")
            sls_offset_params_.Kx_jerk = param.as_double();

        else if (param.get_name() == "Ky_int")
            sls_offset_params_.Ky_int = param.as_double();
        else if (param.get_name() == "Ky_pos")
            sls_offset_params_.Ky_pos = param.as_double();
        else if (param.get_name() == "Ky_vel")
            sls_offset_params_.Ky_vel = param.as_double();
        else if (param.get_name() == "Ky_acc")
            sls_offset_params_.Ky_acc = param.as_double();
        else if (param.get_name() == "Ky_jerk")
            sls_offset_params_.Ky_jerk = param.as_double();

        else if (param.get_name() == "Kz_int")
            sls_offset_params_.Kz_int = param.as_double();
        else if (param.get_name() == "Kz_pos")
            sls_offset_params_.Kz_pos = param.as_double();
        else if (param.get_name() == "Kz_vel")
            sls_offset_params_.Kz_vel = param.as_double();

        else if (param.get_name() == "kR_")
            kR_ = param.as_double();
        else if (param.get_name() == "kOmega_")
            kOmega_ = param.as_double();
        else if (param.get_name() == "kI_")
            kI_ = param.as_double();
        else if (param.get_name() == "cI_")
            cI_ = param.as_double();

        else if (param.get_name() == "use_sim")
            use_sim_ = param.as_bool();
        else if (param.get_name() == "use_ekf")
            use_ekf_ = param.as_bool();

        // else if (param.get_name() == "tau_x_max_")
        //     sls_offset_params_.tau_x_max_ = param.as_double();
        // else if (param.get_name() == "tau_y_max_")
        //     sls_offset_params_.tau_y_max_ = param.as_double();
        // else if (param.get_name() == "tau_z_max_")
        //     sls_offset_params_.tau_z_max_ = param.as_double();
    }
    return result;
}

/**
 * @brief Publish the offboard control mode flags.
 */
void OffboardControl::publish_offboard_control_mode() {
    OffboardControlMode msg{};
    msg.position = (control_mode_ == "position" || control_mode_ == "full");
    msg.velocity = (control_mode_ == "full");
    msg.acceleration = (control_mode_ == "acceleration" || control_mode_ == "full");
    msg.attitude = (control_mode_ == "se3" || control_mode_ == "attitude");
    msg.body_rate = (control_mode_ == "rate");
    msg.thrust_and_torque = (control_mode_ == "torque");
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    offboard_control_mode_publisher_->publish(msg);
}

/**
 * @brief Publish the computed trajectory setpoint
 */
void OffboardControl::publish_trajectory_setpoint() {
    if (!pos_received_ || !ref_received_) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Waiting for vehicle local position and trajectory reference");
        return;
    }

    // Commands
    Eigen::Vector3d a_cmd{};
    Eigen::Vector4d q_cmd{};
    std::pair<Eigen::Vector3d, double> rate_thrust_cmd{};
    Eigen::Vector3d torque_cmd{};

    // Latest odometry
    const Eigen::Vector3d p(latest_local_pos_.x, latest_local_pos_.y, latest_local_pos_.z);
    const Eigen::Vector3d v(latest_local_pos_.vx, latest_local_pos_.vy, latest_local_pos_.vz);

    // Latest reference
    Eigen::Vector3d p_ref(latest_ref_.position[0], latest_ref_.position[1], latest_ref_.position[2]);
    Eigen::Vector3d v_ref(latest_ref_.velocity[0], latest_ref_.velocity[1], latest_ref_.velocity[2]);
    Eigen::Vector3d a_ref(latest_ref_.acceleration[0], latest_ref_.acceleration[1], latest_ref_.acceleration[2]);
    Eigen::Vector3d j_ref(latest_ref_.jerk[0], latest_ref_.jerk[1], latest_ref_.jerk[2]);

    // Can add to the a_cmd types here, later (for different controller support)
    a_cmd = compute_acceleration_command(p, v, p_ref, v_ref, a_ref);

    if (attitude_received_ && ((control_mode_ == "attitude") || (control_mode_ == "rate") || (control_mode_ == "torque"))) {
        if (att_control_type_ == "QSF_offset" && sls_offset_params_.load_received_) {
            // enable flatness for mission only
            if (flight_path_ == "setpoints_figure8") sls_offset_differential_flatness();
            latest_ref_snap_ << sls_offset_params_.snapd1, sls_offset_params_.snapd2, sls_offset_params_.snapd3;

            // Apply QSF slung load offset controller for attitude and rate mode control
            std::tie(q_cmd, rate_thrust_cmd, torque_cmd) = apply_QSF_offset_ctrl(p_ref, v_ref, a_ref, j_ref, latest_ref_snap_);
        } else if (att_control_type_ == "Lee") {
            // apply Lee's geometric controller for torque mode
            publish_se3_setpoint(sls_offset_params_);
            return; // publishes torque/thrust directly
        } else {
            // Convert NED acceleration to ENU (X_enu = Y_ned, Y_enu = X_ned, Z_enu = -Z_ned)
            Eigen::Vector3d a_cmd_enu(a_cmd.y(), a_cmd.x(), -a_cmd.z());

            // Convert NED yaw to ENU yaw (pi/2 offset and inverted direction)
            double yaw_enu = M_PI_2 - yaw_;

            // Add Gravity Compensation in ENU (Gravity pulls -Z, so thrust must push +Z)
            Eigen::Vector3d thrust_vector_enu = a_cmd_enu + Eigen::Vector3d(0.0, 0.0, gravity_); // Compute desired attitude quaternion setpoints

            q_cmd = acceleration_to_quaternion(thrust_vector_enu, yaw_enu);

            // Compute desired body rate and thrust setpoints
            // First 3 indices are rates, last index is normalized thrust
            rate_thrust_cmd = attitude_to_body_rate_and_thrust(latest_attitude_, q_cmd, thrust_vector_enu);
        }
    }

    uint64_t timestamp = this->get_clock()->now().nanoseconds() / 1000;

    TrajectorySetpoint msg{};
    msg.yaw = latest_ref_.yaw;
    msg.timestamp = timestamp;

    TrajectorySetpoint debug_msg{};
    debug_msg.yaw = latest_ref_.yaw;
    debug_msg.timestamp = timestamp;

    if (control_mode_ == "position") {
        msg.position = {static_cast<float>(p_ref.x()), static_cast<float>(p_ref.y()), static_cast<float>(p_ref.z())};
        msg.velocity = {NAN, NAN, NAN};
        msg.acceleration = {NAN, NAN, NAN};

        debug_msg.position = {NAN, NAN, NAN};
        debug_msg.velocity = {NAN, NAN, NAN};
        debug_msg.acceleration = {static_cast<float>(a_cmd.x()), static_cast<float>(a_cmd.y()), static_cast<float>(a_cmd.z())};
        debug_trajectory_setpoint_publisher_->publish(debug_msg);

    } else if (control_mode_ == "acceleration") {
        msg.position = {NAN, NAN, NAN};
        msg.velocity = {NAN, NAN, NAN};
        msg.acceleration = {static_cast<float>(a_cmd.x()), static_cast<float>(a_cmd.y()), static_cast<float>(a_cmd.z())};

        debug_msg.position = {NAN, NAN, NAN};
        debug_msg.velocity = {NAN, NAN, NAN};
        debug_msg.acceleration = {NAN, NAN, NAN};
        debug_trajectory_setpoint_publisher_->publish(debug_msg);

    } else if (control_mode_ == "full") {
        msg.position = {static_cast<float>(p_ref.x()), static_cast<float>(p_ref.y()), static_cast<float>(p_ref.z())};
        msg.velocity = {static_cast<float>(v_ref.x()), static_cast<float>(v_ref.y()), static_cast<float>(v_ref.z())};
        msg.acceleration = {static_cast<float>(a_ref.x()), static_cast<float>(a_ref.y()), static_cast<float>(a_ref.z())};

        debug_msg.position = {NAN, NAN, NAN};
        debug_msg.velocity = {NAN, NAN, NAN};
        debug_msg.acceleration = {NAN, NAN, NAN};
        debug_trajectory_setpoint_publisher_->publish(debug_msg);
    } else if (control_mode_ == "attitude" && attitude_received_) {
        publish_attitude_setpoints(rate_thrust_cmd.second, q_cmd);
        msg.position = {static_cast<float>(p_ref.x() + sls_offset_params_.L_offset_[0]), static_cast<float>(p_ref.y() + sls_offset_params_.L_offset_[1]), static_cast<float>(p_ref.z() - sls_offset_params_.L_offset_[2] - sls_offset_params_.l)};
        msg.velocity = {static_cast<float>(v_ref.x()), static_cast<float>(v_ref.y()), static_cast<float>(v_ref.z())};
        msg.acceleration = {static_cast<float>(a_ref.x()), static_cast<float>(a_ref.y()), static_cast<float>(a_ref.z())};
    } else if (control_mode_ == "rate" && attitude_received_) {
        publish_rate_setpoints(rate_thrust_cmd.first, rate_thrust_cmd.second);
        msg.position = {static_cast<float>(p_ref.x() + sls_offset_params_.L_offset_[0]), static_cast<float>(p_ref.y() + sls_offset_params_.L_offset_[1]), static_cast<float>(p_ref.z() - sls_offset_params_.L_offset_[2] - sls_offset_params_.l)};
        msg.velocity = {static_cast<float>(v_ref.x()), static_cast<float>(v_ref.y()), static_cast<float>(v_ref.z())};
        msg.acceleration = {static_cast<float>(a_ref.x()), static_cast<float>(a_ref.y()), static_cast<float>(a_ref.z())};
    } else if (control_mode_ == "torque" && attitude_received_) {
        publish_torque_thrust_setpoints(torque_cmd, rate_thrust_cmd.second && (att_control_type_ == "QSF_offset"));
        msg.position = {static_cast<float>(p_ref.x() + sls_offset_params_.L_offset_[0]), static_cast<float>(p_ref.y() + sls_offset_params_.L_offset_[1]), static_cast<float>(p_ref.z() - sls_offset_params_.L_offset_[2] - sls_offset_params_.l)};
        msg.velocity = {static_cast<float>(v_ref.x()), static_cast<float>(v_ref.y()), static_cast<float>(v_ref.z())};
        msg.acceleration = {static_cast<float>(a_ref.x()), static_cast<float>(a_ref.y()), static_cast<float>(a_ref.z())};
    } else {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "Unknown control_mode '%s', falling back to full mode", control_mode_.c_str());
        msg.position = {static_cast<float>(p_ref.x()), static_cast<float>(p_ref.y()), static_cast<float>(p_ref.z())};
        msg.velocity = {static_cast<float>(v_ref.x()), static_cast<float>(v_ref.y()), static_cast<float>(v_ref.z())};
        msg.acceleration = {static_cast<float>(a_ref.x()), static_cast<float>(a_ref.y()), static_cast<float>(a_ref.z())};
    }

    trajectory_setpoint_publisher_->publish(msg);
}

/**
 * @brief Publish the computed attitude setpoint using CTU-MRS' SE(3) controller
 */
void OffboardControl::publish_se3_setpoint(OffboardControl::sls_offset_params &sls_offset_params) {
    if (!pos_received_ || !ref_received_ || !attitude_received_) return;

    Eigen::Vector3d p(sls_offset_params.latest_pos_enu_.y(), sls_offset_params.latest_pos_enu_.x(), -sls_offset_params.latest_pos_enu_.z());
    Eigen::Vector3d v(sls_offset_params.latest_vel_enu_.y(), sls_offset_params.latest_vel_enu_.x(), -sls_offset_params.latest_vel_enu_.z());
   
    Eigen::Vector3d p_ref(latest_ref_.position[0], latest_ref_.position[1], latest_ref_.position[2]);
    p_ref.z() -= sls_offset_params_.l;
    Eigen::Vector3d v_ref(latest_ref_.velocity[0], latest_ref_.velocity[1], latest_ref_.velocity[2]);
    Eigen::Vector3d a_ref(latest_ref_.acceleration[0], latest_ref_.acceleration[1], latest_ref_.acceleration[2]);
    double yaw_ref = latest_ref_.yaw;

    const Eigen::Vector3d e_p = p - p_ref;
    const Eigen::Vector3d e_v = v - v_ref;

    // Lee outer-loop position integral (linear)
    if (reset_position_integral_) {
        position_integral_.setZero();
        first_position_integral_call_ = true;
        reset_position_integral_ = false;
    }
    const rclcpp::Time now = this->now();
    if (first_position_integral_call_) {
        last_position_integral_time_ = now;
        first_position_integral_call_ = false;
    } else {
        const double dt = std::clamp((now - last_position_integral_time_).seconds(),0.001, 0.025);
        last_position_integral_time_ = now;
        if (ki_ > 0.0) {
            position_integral_ += e_p * dt;
            for (int i = 0; i < 3; ++i) {
                position_integral_[i] = std::clamp(position_integral_[i], -position_integral_limit_, position_integral_limit_);
            }
        }
    }

    // calculate desired force with integral (NED Frame)
    Eigen::Vector3d F_d = -K_p_*e_p - K_v_*e_v - K_i_*position_integral_ + mass_ * a_ref - Eigen::Vector3d(0.0, 0.0, mass_ * gravity_);

    // calculate desired attitude (rotation matrix)
    Eigen::Vector3d z_B = -F_d.normalized();

    // create desired heading vector
    Eigen::Vector3d x_C(std::cos(yaw_ref), std::sin(yaw_ref), 0.0);

    // construct orthogonal body axes
    Eigen::Vector3d y_B = z_B.cross(x_C).normalized();
    Eigen::Vector3d x_B = y_B.cross(z_B).normalized();

    Eigen::Matrix3d R_d;
    R_d.col(0) = x_B;
    R_d.col(1) = y_B;
    R_d.col(2) = z_B;

    // geometric torque control
    Eigen::Quaterniond q_enu(latest_attitude_(0), latest_attitude_(1), latest_attitude_(2), latest_attitude_(3)); // w,x,y,z
    Eigen::Quaterniond q_ned = px4_ros_com::frame_transforms::ros_to_px4_orientation(q_enu);
    q_ned.normalize();
    Eigen::Matrix3d R_ = q_ned.toRotationMatrix();
    tf2::Quaternion q_uav(q_ned.x(),q_ned.y(),q_ned.z(),q_ned.w());
    tf2::Matrix3x3 m_uav(q_uav);
    m_uav.getRPY(sls_offset_params_.phi_rad_, sls_offset_params_.theta_rad_, sls_offset_params_.psi_rad_);

    sls_offset_params_.R_Bd = R_d;
    publish_qsf_attitude_debug(); // for debug publisher
    double f = -F_d.dot(R_.col(2)); // f = -F_d R e3
    sls_offset_params_.des_thrust = -f; // for debug publisher
    double thrust_command = f / mass_;
    Eigen::Vector3d torque_cmd = sls_offset_thrust_torque_inner_loop(thrust_command);

    // use different drone mass in sdf and tune calib_thrust_ until acc = 0
    // RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 200, "[Lee] z_err=%.3f", e_p.z());
    // RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 200, "[Lee] z=%.3f vz=%.3f", p.z(), v.z());
    // sls_offset_params_.R_Bd << Eigen::Matrix3d::Identity(); // calibration test
    // double calibration_thrust = std::clamp(calib_thrust_, 0.0, 1.0);
    // publish_torque_thrust_setpoints(torque_cmd, calibration_thrust);

    double normalized_thrust;
    if (use_sim_ && att_control_type_ == "Lee") {
        // Produced together with normalized torque by f450_px4_inverse_sitl()
        normalized_thrust = lee_sitl_normalized_thrust_;
    } else {
        normalized_thrust = norm_thrust_const_ * thrust_command + norm_thrust_offset_;
        normalized_thrust = std::clamp(normalized_thrust, 0.0, 1.0);
    }
    publish_torque_thrust_setpoints(torque_cmd, normalized_thrust); 
}

Eigen::Vector3d OffboardControl::compute_acceleration_command(const Eigen::Vector3d &p, const Eigen::Vector3d &v, const Eigen::Vector3d &p_d, const Eigen::Vector3d &v_d, const Eigen::Vector3d &a_d) {

    const Eigen::Vector3d e_p = p - p_d;
    const Eigen::Vector3d e_v = v - v_d;
    return a_d - K_v_ * e_v - K_p_ * e_p; // gravity compensation already accounted for
}

OffboardControl::sls_offset_ned_params OffboardControl::sls_offset_enu_to_ned(OffboardControl::sls_offset_params &sls_offset_params) {
    // Positions
    Eigen::Vector3d pos_ned(sls_offset_params.latest_pos_enu_.y(), sls_offset_params.latest_pos_enu_.x(), -sls_offset_params.latest_pos_enu_.z());
    Eigen::Vector3d load_pos_ned(sls_offset_params.load_pos_enu_.y(), sls_offset_params.load_pos_enu_.x(), -sls_offset_params.load_pos_enu_.z());
    Eigen::Vector3d pend_angle_ned(sls_offset_params.pend_angle_enu_.y(), sls_offset_params.pend_angle_enu_.x(), -sls_offset_params.pend_angle_enu_.z());

    // Velocities
    Eigen::Vector3d vel_ned(sls_offset_params.latest_vel_enu_.y(), sls_offset_params.latest_vel_enu_.x(), -sls_offset_params.latest_vel_enu_.z());
    Eigen::Vector3d load_vel_ned(sls_offset_params.load_vel_enu_.y(), sls_offset_params.load_vel_enu_.x(), -sls_offset_params.load_vel_enu_.z());
    Eigen::Vector3d pend_rate_ned(sls_offset_params.pend_rate_enu_.y(), sls_offset_params.pend_rate_enu_.x(), -sls_offset_params.pend_rate_enu_.z());
    Eigen::Vector3d load_rate_ned(sls_offset_params.load_rate_enu_.y(), sls_offset_params.load_rate_enu_.x(), -sls_offset_params.load_rate_enu_.z());

    // UAV att. rotation matrix conversion (verified)
    // q in NED, so convert to rotmat is sls_offset_params.R_bi
    Eigen::Quaterniond q_enu(latest_attitude_(0), latest_attitude_(1), latest_attitude_(2), latest_attitude_(3)); // w,x,y,z
    Eigen::Quaterniond q_ned = px4_ros_com::frame_transforms::ros_to_px4_orientation(q_enu);
    Eigen::Vector4d attitude_ned(q_ned.w(), q_ned.x(), q_ned.y(), q_ned.z());
    Eigen::Matrix3d rotmat_uav = quaternion_to_rotation_matrix(attitude_ned);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sls_offset_params.R_bi[i * 3 + j] = rotmat_uav(i, j);
        }
    }

    // Get RPY of UAV to verify attitude
    tf2::Quaternion q_uav(attitude_ned(1), attitude_ned(2), attitude_ned(3), attitude_ned(0)); // x,y,z,w
    tf2::Matrix3x3 m_uav(q_uav);
    m_uav.getRPY(sls_offset_params.phi_rad_, sls_offset_params.theta_rad_, sls_offset_params.psi_rad_);

    // Find load angles
    // double dx, dy, dz;
    // dx = (load_pos_ned.x() - (pos_ned.x() + sls_offset_params.R_bi[0] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[1] * sls_offset_params.L_offset_[1] +
    //                           sls_offset_params.R_bi[2] * sls_offset_params.L_offset_[2])) /
    //      sls_offset_params.l;
    // dy = (load_pos_ned.y() - (pos_ned.y() + sls_offset_params.R_bi[3] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[4] * sls_offset_params.L_offset_[1] +
    //                           sls_offset_params.R_bi[5] * sls_offset_params.L_offset_[2])) /
    //      sls_offset_params.l;
    // dz = (load_pos_ned.z() - (pos_ned.z() + sls_offset_params.R_bi[6] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[7] * sls_offset_params.L_offset_[1] +
    //                           sls_offset_params.R_bi[8] * sls_offset_params.L_offset_[2])) /
    //      sls_offset_params.l;
    // sls_offset_params.alpha = asin(-dy);
    // sls_offset_params.beta = asin(dx / cos(sls_offset_params.alpha));

    // q vec calc by swing angles
    double q[3];
    // q[0] = cos(sls_offset_params.alpha)*sin(sls_offset_params.beta);
    // q[1] = -sin(sls_offset_params.alpha);
    // q[2] = cos(sls_offset_params.alpha)*cos(sls_offset_params.beta);

    // Method 2), by normalization
    double pivot_point_pos[3] = {pos_ned.x() + sls_offset_params.R_bi[0] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[1] * sls_offset_params.L_offset_[1] +
                                     sls_offset_params.R_bi[2] * sls_offset_params.L_offset_[2],
                                 pos_ned.y() + sls_offset_params.R_bi[3] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[4] * sls_offset_params.L_offset_[1] +
                                     sls_offset_params.R_bi[5] * sls_offset_params.L_offset_[2],
                                 pos_ned.z() + sls_offset_params.R_bi[6] * sls_offset_params.L_offset_[0] + sls_offset_params.R_bi[7] * sls_offset_params.L_offset_[1] +
                                     sls_offset_params.R_bi[8] * sls_offset_params.L_offset_[2]};
    double vec_load_to_pivot[3] = {load_pos_ned.x() - pivot_point_pos[0], load_pos_ned.y() - pivot_point_pos[1], load_pos_ned.z() - pivot_point_pos[2]};
    double norm = sqrt(vec_load_to_pivot[0] * vec_load_to_pivot[0] + vec_load_to_pivot[1] * vec_load_to_pivot[1] + vec_load_to_pivot[2] * vec_load_to_pivot[2]);
    q[0] = vec_load_to_pivot[0] / norm;
    q[1] = vec_load_to_pivot[1] / norm;
    q[2] = vec_load_to_pivot[2] / norm; // omit division by l, since q is normalized anyway
    sls_offset_params.q_vec[0] = q[0];
    sls_offset_params.q_vec[1] = q[1];
    sls_offset_params.q_vec[2] = q[2];

    // find pendrate = q.cross(loadVel - Pivot_Vel)/l;
    // Pivot_Vel = vel_ned + dR*L = vel_ned+ sls_offset_params.R_bi*hat(Omega)*L
    Eigen::Vector3d Omega = sls_offset_params_.latest_rate_frd_;
    Eigen::Vector3d L = {sls_offset_params.L_offset_[0], sls_offset_params.L_offset_[1], sls_offset_params.L_offset_[2]};
    Eigen::Matrix3d Rbi_;
    Rbi_ << sls_offset_params.R_bi[0], sls_offset_params.R_bi[1], sls_offset_params.R_bi[2], 
            sls_offset_params.R_bi[3], sls_offset_params.R_bi[4], sls_offset_params.R_bi[5], 
            sls_offset_params.R_bi[6], sls_offset_params.R_bi[7], sls_offset_params.R_bi[8];
    Eigen::Vector3d Pivot_Vel = vel_ned + Rbi_ * Omega.cross(L);
    load_rate_ned = Eigen::Vector3d(q[0], q[1], q[2]).cross(load_vel_ned - Pivot_Vel)/sls_offset_params.l; 

    // Manual calculation of angular rate by finite difference
    // static double conversion_last_called_ = 0.0;
    // double current_time = this->get_clock()->now().seconds();
    // double dt = current_time - conversion_last_called_;
    // conversion_last_called_ = current_time;

    // if (dt > FD_EPSILON) {
    //     sls_offset_params.dalpha = (sls_offset_params.alpha - sls_offset_params.alpha_prev) / dt;
    //     sls_offset_params.dbeta = (sls_offset_params.beta - sls_offset_params.beta_prev) / dt;
    // } else {
    //     sls_offset_params.dalpha = sls_offset_params.dalpha_prev;
    //     sls_offset_params.dbeta = sls_offset_params.dbeta_prev;
    // }

    // // calculate dq/dt by direct formula
    // const double alpha  = sls_offset_params.alpha;
    // const double beta   = sls_offset_params.beta;
    // const double dalpha = sls_offset_params.dalpha;
    // const double dbeta  = sls_offset_params.dbeta;
    // sls_offset_params.dq[0] = dbeta * std::cos(beta) * std::cos(alpha) - std::sin(beta) * dalpha * std::sin(alpha);
    // sls_offset_params.dq[1] = -dalpha * std::cos(alpha);
    // sls_offset_params.dq[2] = -dbeta * std::sin(beta) * std::cos(alpha) - std::cos(beta) * dalpha * std::sin(alpha);

    // dq = w x q
    Eigen::Vector3d q_eigen(q[0], q[1], q[2]);
    Eigen::Vector3d dq_eigen = load_rate_ned.cross(q_eigen);
    sls_offset_params.dq[0] = dq_eigen.x();
    sls_offset_params.dq[1] = dq_eigen.y();
    sls_offset_params.dq[2] = dq_eigen.z();

    // sls_offset_params.alpha_prev = sls_offset_params.alpha;
    // sls_offset_params.beta_prev = sls_offset_params.beta;
    // sls_offset_params.dalpha_prev = sls_offset_params.dalpha;
    // sls_offset_params.dbeta_prev = sls_offset_params.dbeta;

    // Create return struct
    sls_offset_ned_params sls_offset_ned{
        .load_pos = load_pos_ned,
        .load_vel = load_vel_ned,
        .load_rate = load_rate_ned,
        .q = {q[0], q[1], q[2]},
    };
    return sls_offset_ned;
}

std::tuple<Eigen::Vector4d, std::pair<Eigen::Vector3d, double>, Eigen::Vector3d> OffboardControl::apply_QSF_offset_ctrl(const Eigen::Vector3d &pos_des_ned, const Eigen::Vector3d &vel_des_ned,
                                                                                                                        const Eigen::Vector3d &acc_des_ned, const Eigen::Vector3d &jerk_des_ned,
                                                                                                                        const Eigen::Vector3d &snap_des_ned) {
    double K1_int[5] = {sls_offset_params_.Kx_int, sls_offset_params_.Kx_pos, sls_offset_params_.Kx_vel, sls_offset_params_.Kx_acc, sls_offset_params_.Kx_jerk};
    double K2_int[5] = {sls_offset_params_.Ky_int, sls_offset_params_.Ky_pos, sls_offset_params_.Ky_vel, sls_offset_params_.Ky_acc, sls_offset_params_.Ky_jerk};
    double K3_int[3] = {sls_offset_params_.Kz_int, sls_offset_params_.Kz_pos, sls_offset_params_.Kz_vel};

    double ref_traj[15] = {pos_des_ned.x(),  vel_des_ned.x(),  acc_des_ned.x(), jerk_des_ned.x(), snap_des_ned.x(), 
                           pos_des_ned.y(),  vel_des_ned.y(),  acc_des_ned.y(), jerk_des_ned.y(), snap_des_ned.y(), 
                           pos_des_ned.z(),  vel_des_ned.z(),  acc_des_ned.z(), jerk_des_ned.z(), snap_des_ned.z()};

    auto sls_ned_params = sls_offset_enu_to_ned(sls_offset_params_);

    // reset integrals b4 used
    if (reset_qsf_integral_) {
        for (int i = 0; i < 3; i++) {
            sls_offset_params_.integral[i] = 0.0;
        }
        reset_qsf_integral_ = false;
    }

    // States = {pl, q, vl, w}
    double states[12] = {sls_ned_params.load_pos.x(), sls_ned_params.load_pos.y(),  sls_ned_params.load_pos.z(),  
                         sls_ned_params.q[0], sls_ned_params.q[1], sls_ned_params.q[2],          
                         sls_ned_params.load_vel.x(),  sls_ned_params.load_vel.y(), sls_ned_params.load_vel.z(), 
                         sls_ned_params.load_rate.x(), sls_ned_params.load_rate.y(), sls_ned_params.load_rate.z()};

    // Apply QSF offset control (proposed) (U-model)
    double thetad, phid;
    double Rbd[9];
    // QSF_w_offset_intctrl_U(mass_, sls_offset_params_.l, gravity_, 
    //                        K1_int, K2_int, K3_int, ref_traj, states, sls_offset_params_.psi_rad_, 
    //                        sls_offset_params_.Td_scaler, sls_offset_params_.integral, 
    //                        Rbd, &des_thrust, &phid, &thetad);

    // F-model
    double aLd[3] = {sls_offset_params_.aLd1, sls_offset_params_.aLd2, sls_offset_params_.aLd3}; 
    QSF_w_offset_intctrl(sls_offset_params_.load_mass_, mass_, sls_offset_params_.l, gravity_, 
                         K1_int, K2_int, K3_int, ref_traj, states, sls_offset_params_.psi_rad_, 
                         aLd, sls_offset_params_.integral, 
                         Rbd, &sls_offset_params_.des_thrust, &phid, &thetad);

    // Update integral
    static rclcpp::Time last_time_QSF = this->get_clock()->now();
    const rclcpp::Time now_QSF = this->get_clock()->now();
    double dt_QSF = std::clamp((now_QSF - last_time_QSF).seconds(), 0.001, 0.05); // clamp dt_QSF to [1ms, 25ms] to avoid large dt if the loop is delayed
    last_time_QSF = now_QSF;

    double integral_dt[3] = {sls_ned_params.load_pos.x() - pos_des_ned.x(), sls_ned_params.load_pos.y() - pos_des_ned.y(), sls_ned_params.load_pos.z() - pos_des_ned.z()};
    for (int i = 0; i < 3; i++) {
        // only accumulate error if accumulated error <= 100
        if (std::abs(sls_offset_params_.integral[i] + integral_dt[i] * dt_QSF) <= 100) {
            sls_offset_params_.integral[i] += integral_dt[i] * dt_QSF;

        }
    }

    // Save data
    // Filled column-wise, see codegen for reason
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sls_offset_params_.R_Bd(j, i) = Rbd[i * 3 + j];
        }
    }

    // debug only
    // thetad_deg_ = thetad * 180.0 / M_PI;
    // phid_deg_ = phid * 180.0 / M_PI;

    // Convert to desired attitudes and thrust (acceleration) for attitude/rate control
    auto thrust_command = -sls_offset_params_.des_thrust / mass_;

    // QSF matrix is NED. Convert it back to ENU so the quaternion math and publishers work
    Eigen::Vector4d q_des_ned = rotation_matrix_to_quaternion(sls_offset_params_.R_Bd);
    Eigen::Quaterniond q_ned_obj(q_des_ned(0), q_des_ned(1), q_des_ned(2), q_des_ned(3));
    Eigen::Quaterniond q_enu_obj = px4_ros_com::frame_transforms::px4_to_ros_orientation(q_ned_obj);
    Eigen::Vector4d q_des_enu(q_enu_obj.w(), q_enu_obj.x(), q_enu_obj.y(), q_enu_obj.z());
    publish_qsf_attitude_debug(); // attitude debug publish

    auto rate_thrust_cmd = sls_offset_attitude_to_body_rate_and_thrust(latest_attitude_, q_des_enu, thrust_command);
    auto torque_cmd = sls_offset_thrust_torque_inner_loop(thrust_command);
    return {q_des_enu, rate_thrust_cmd, torque_cmd};
}

Eigen::Vector3d OffboardControl::sls_offset_thrust_torque_inner_loop(double thrust_command) {
    double Omega[3] = {sls_offset_params_.latest_rate_frd_.x(), sls_offset_params_.latest_rate_frd_.y(), sls_offset_params_.latest_rate_frd_.z()};
    double Omegad[3] = {sls_offset_params_.Omegad1, sls_offset_params_.Omegad2, sls_offset_params_.Omegad3};
    double dOmegad[3] = {sls_offset_params_.dOmegad1, sls_offset_params_.dOmegad2, sls_offset_params_.dOmegad3};
    double rpy_angles[3] = {sls_offset_params_.phi_rad_, sls_offset_params_.theta_rad_, sls_offset_params_.psi_rad_};
    double q_vec_[3] = {sls_offset_params_.q_vec[0], sls_offset_params_.q_vec[1], sls_offset_params_.q_vec[2]};
    double dq_vec[3] = {sls_offset_params_.dq[0], sls_offset_params_.dq[1], sls_offset_params_.dq[2]};
    double gains[4] = {kR_, kOmega_, kI_, cI_};
    double physics_parameters[7] = {mass_, sls_offset_params_.load_mass_, gravity_, sls_offset_params_.l, sls_offset_params_.Iqxx, sls_offset_params_.Iqyy, sls_offset_params_.Iqzz};
    
    // integral paramaters
    static double eI[3] = {0.0, 0.0, 0.0}; // integral state input
    static double eI_dt[3] = {0.0, 0.0, 0.0}; // derivative of integral state input
    static bool first_call_inner_loop_ = true;
    static rclcpp::Time last_called_inner_loop_ = this->get_clock()->now();
    double dt = 0.0;

    // reset integral if not in offboard mode
    if (reset_inner_integral_) {
        for (int i = 0; i < 3; i++) eI[i] = 0.0;
        first_call_inner_loop_ = true;
        reset_inner_integral_ = false;
    } 

    if (first_call_inner_loop_) {
        first_call_inner_loop_ = false;
        last_called_inner_loop_ = this->get_clock()->now();
    } else {
        rclcpp::Time now = this->get_clock()->now();
        dt = std::clamp((now - last_called_inner_loop_).seconds(), 0.001, 0.025);
        last_called_inner_loop_ = now;
    }

    double taub[3], tau[3], rate_sp_dt[3], rate_sp_dt2[3];
    Inner_loop_geometric_PID(rpy_angles, q_vec_, dq_vec, Omega, sls_offset_params_.R_Bd.data(), Omegad, dOmegad, -mass_ * thrust_command, 
                             gains, physics_parameters, sls_offset_params_.L_offset_, eI, taub, tau, rate_sp_dt, rate_sp_dt2, eI_dt); 

    // accumlate integral after 5s to prevent inital error being mistaken as a persistent disturbance
    if((this->get_clock()->now() - start_time_).seconds() >= 5.0){       
        for(int i = 0; i < 3; i++) {
            if (std::isfinite(eI_dt[i])) eI[i] = std::clamp(eI[i] + (eI_dt[i] * dt), -10.0, 10.0);
        }
    }

    publish_inner_integral_debug(eI); // eI debugger
    publish_qsf_controller_output(sls_offset_params_.des_thrust, tau); // torque and thrust debugger

    // Normalize tau for torque and thrust setpoint to [-1, 1]
    // tau[0] = std::clamp(tau[0] / sls_offset_params_.tau_x_max_, -1.0, 1.0);
    // tau[1] = std::clamp(tau[1] / sls_offset_params_.tau_y_max_, -1.0, 1.0);
    // tau[2] = std::clamp(tau[2] / sls_offset_params_.tau_z_max_, -1.0, 1.0);

    Eigen::Vector3d tau_raw(tau[0], tau[1], tau[2]); // [N m]
    // thrust_command is acceleration [m/s^2].
    // Convert it back to positive physical thrust [N].
    const double thrust_n = mass_ * thrust_command; 
    auto normalized = f450_px4_inverse_sitl(tau_raw, thrust_n);
    lee_sitl_normalized_thrust_ = normalized.second;
    return normalized.first;

    // Eigen::Vector3d tau_vec(tau[0], tau[1], tau[2]);
    // return tau_vec;
}

std::pair<Eigen::Vector3d, double> OffboardControl::f450_px4_inverse_sitl(const Eigen::Vector3d &tau_nm, double thrust_n) {
    // ==================================================================================================
    // https://github.com/SaxionMechatronics/px4_offboard_lowlevel/blob/main/src/controller_node.cpp#L286
    // 
    // F450 Gazebo physical motor model
    //
    // model.sdf:
    //   rotor projected X/Y lever arm = 0.1626345596714 m
    //   motorConstant                 = 1.2e-5
    //   momentConstant                = 0.0137
    //
    // Physical rotor order in PX4 FRD:
    //   0: Front Right, CCW, (+a, +a)
    //   1: Back Left,   CCW, (-a, -a)
    //   2: Front Left,  CW,  (+a, -a)
    //   3: Back Right,  CW,  (-a, +a)
    //
    // Gazebo model itself is FLU, but after FLU -> FRD this rotor
    // ordering/geometry matches the PX4 4201_gz_f450 airframe.
    // ==================================================================================================

    constexpr double a  = 0.1626345596714; // projected X/Y lever arm [m]
    constexpr double kf = 1.2e-5;          // thrust_i = kf * omega_i^2
    constexpr double km = 0.0137;          // yaw torque to thrust ratio, tau_yaw_i = km * thrust_i

    // PX4 4201_gz_f450 airframe:
    // SIM_GZ_EC_MIN1..4 = 150
    // SIM_GZ_EC_MAX1..4 = 1000
    constexpr double omega_min = 150.0;    // rad/s
    constexpr double omega_max = 1000.0;   // rad/s
    constexpr double omega_min_sq = omega_min * omega_min;
    constexpr double omega_max_sq = omega_max * omega_max;
    constexpr double output_range = omega_max - omega_min;

    // =================================================================
    // 1. Keep requested collective thrust inside physical motor limits.
    //
    // At zero torque all four motors have equal speed.
    // =================================================================

    constexpr double thrust_min = 4.0 * kf * omega_min_sq;
    constexpr double thrust_max = 4.0 * kf * omega_max_sq;
    const double thrust_requested = thrust_n;   // computed by outer-loop controller

    thrust_n = std::clamp(thrust_n, thrust_min, thrust_max);

    if (std::abs(thrust_n - thrust_requested) > 1e-6) {
        RCLCPP_WARN_THROTTLE(this->get_logger(),*this->get_clock(),1000,
            "[F450 inverse] Thrust %.3f N outside feasible range "
            "[%.3f, %.3f] N; clamped to %.3f N",
            thrust_requested, thrust_min, thrust_max, thrust_n);
    }

    // ============================================================
    // 2. Physical wrench -> omega^2
    // 
    // For the F450 rotor configuration:
    // tau_x = y*Fz = y*kf*omega^2
    // 
    // tau_x = a*kf*(-w0^2 + w1^2 + w2^2 - w3^2)
    // tau_y = a*kf*( w0^2 - w1^2 + w2^2 - w3^2)
    // tau_z = km*kf*(w0^2 + w1^2 - w2^2 - w3^2)
    // F = kf*(w0^2 + w1^2 + w2^2 + w3^2)
    //
    // alpha scales torque while leaving thrust unchanged.
    // ============================================================

    auto compute_omega_sq = [&](double alpha) -> Eigen::Vector4d {
        const Eigen::Vector3d tau = alpha * tau_nm;

        const double tx = tau.x() / (a * kf);
        const double ty = tau.y() / (a * kf);
        const double tz = tau.z() / (km * kf);
        const double ft = thrust_n / kf;

        Eigen::Vector4d omega_sq;
        omega_sq[0] = 0.25 * (-tx + ty + tz + ft);
        omega_sq[1] = 0.25 * ( tx - ty + tz + ft);
        omega_sq[2] = 0.25 * ( tx + ty - tz + ft);
        omega_sq[3] = 0.25 * (-tx - ty - tz + ft);

        return omega_sq;
    };

    // ============================================================
    // 3. Check whether a requested wrench is physically feasible.
    //
    // Unlike the previous implementation, DO NOT turn negative
    // omega^2 into zero. Negative omega^2 means the desired wrench
    // cannot be produced at the requested collective thrust.
    // ============================================================

    auto is_feasible = [&](const Eigen::Vector4d &omega_sq) -> bool {
        for (int i = 0; i < 4; ++i) {
            if (!std::isfinite(omega_sq[i])) {
                return false;
            }

            if (omega_sq[i] < omega_min_sq || omega_sq[i] > omega_max_sq) {
                return false;
            }
        }
        return true;
    };

    // ============================================================
    // 4. Try alpha = 1.0 first
    // ============================================================
    double alpha = 1.0; 
    Eigen::Vector4d omega_sq = compute_omega_sq(alpha);

    // ============================================================
    // 5. If the requested torque is infeasible, preserve thrust
    //    and find the largest feasible torque scaling.
    //
    // Since omega^2 depends linearly on alpha, and alpha=0 is the
    // equal-speed collective-thrust operating point, binary search
    // finds the largest feasible alpha in [0,1].
    // ============================================================

    if (!is_feasible(omega_sq)) {
        const Eigen::Vector4d zero_torque_omega_sq = compute_omega_sq(0.0);

        // This should only happen if something is fundamentally
        // inconsistent with the thrust limits/model.
        if (!is_feasible(zero_torque_omega_sq)) {

            RCLCPP_ERROR_THROTTLE(this->get_logger(),*this->get_clock(),1000,
                "[F450 inverse] Zero-torque wrench is infeasible "
                "even after thrust limiting. "
                "F=%.3f N, omega^2=[%.1f %.1f %.1f %.1f]",
                thrust_n, zero_torque_omega_sq[0], zero_torque_omega_sq[1], zero_torque_omega_sq[2], zero_torque_omega_sq[3]);

            // Conservative fallback:
            // no torque, minimum normalized thrust.
            return {Eigen::Vector3d::Zero(), 0.0};
        }

        double lo = 0.0;
        double hi = 1.0;

        // 40 iterations is far more precision than needed here.
        for (int iter = 0; iter < 40; ++iter) {
            const double mid = 0.5 * (lo + hi);
            const Eigen::Vector4d candidate = compute_omega_sq(mid);

            if (is_feasible(candidate)) {
                lo = mid;
            } else {
                hi = mid;
            }
        }

        alpha = lo;
        omega_sq = compute_omega_sq(alpha);

        RCLCPP_WARN_THROTTLE(this->get_logger(),*this->get_clock(),500,
            "[F450 inverse] Torque request infeasible. "
            "Scaling torque by alpha=%.4f. "
            "tau_cmd=[%.3f %.3f %.3f] Nm",
            alpha, tau_nm.x(), tau_nm.y(), tau_nm.z());
    }

    // =============================================================
    // 6. omega^2 -> omega
    //
    // At this point all values should already be feasible.
    // Clamp only protects against floating point error at the limit
    // =============================================================

    Eigen::Vector4d omega;
    Eigen::Vector4d motor_norm;

    for (int i = 0; i < 4; ++i) {
        omega_sq[i] = std::clamp(omega_sq[i], omega_min_sq, omega_max_sq);
        omega[i] = std::sqrt(omega_sq[i]);

        // Saxion px4InverseSITL equivalent:
        //
        // normalized motor command =
        //      (omega - SIM_GZ_EC_MIN)
        //      -----------------------
        //      (SIM_GZ_EC_MAX - MIN)
        //
        motor_norm[i] = (omega[i] - omega_min) / output_range;

        // Numerical protection only.
        motor_norm[i] = std::clamp(motor_norm[i], 0.0, 1.0);
    }

    // ============================================================
    // 7. Invert PX4 v1.16.2 normalized F450 mixer
    //
    // This relation comes from the actual PX4 control-allocation
    // geometry in 4201_gz_f450.
    //
    // Default rotor axis = [0, 0, -1]
    //
    // src/modules/control_allocator/module.yaml
    // PX4 constructs each rotor effectiveness as:
    // where CT is thrust coefficient of rotor i
    //   thrust = CT * axis
    //   moment = CT * position.cross(axis) - CT * KM * axis
    //
    // Since
    //   position_frd = [x, y, 0]
    //   axis_frd     = [0, 0, -1]
    //
    // then:
    //   position.cross(axis) = [-y, +x, 0]
    //
    // Therefore each rotor contributes:
    //   Mx = -CT * y
    //   My = +CT * x
    //   Mz = +CT * KM
    //   Fz = -CT
    //
    // With CT = 6.5:
    //   A = CT * 0.159 = 1.0335
    //   B = CT * 0.014 = 0.091
    //   C = CT         = 6.5
    //
    // The physical PX4 effectiveness matrix is:
    //
    //             motor0   motor1   motor2   motor3
    //
    //   Mx       -A       +A       +A       -A
    //   My       +A       -A       +A       -A
    //   Mz       +B       +B       -B       -B
    //   Fz       -C       -C       -C       -C
    //
    // i.e.
    //   
    //   B_CA =
    //
    //   [ -1.0335, +1.0335, +1.0335, -1.0335 ]
    //   [ +1.0335, -1.0335, +1.0335, -1.0335 ]
    //   [ +0.0910, +0.0910, -0.0910, -0.0910 ]
    //   [ -6.5000, -6.5000, -6.5000, -6.5000 ]
    //
    // PX4 computes the pseudoinverse of this effectiveness matrix,
    // then normalizes the roll/pitch/yaw/thrust columns (ControlAllocationPseudoInverse.cpp)
    //
    // After PX4 normalization:
    //
    //   roll/pitch coefficient = 1/sqrt(2) = 0.70710678
    //   yaw coefficient        = 1
    //   thrust coefficient     = -1
    //
    // Therefore the normalized PX4 mixer is:
    //
    // Let:
    //   r  = 1/sqrt(2)
    //   ux = normalized roll torque
    //   uy = normalized pitch torque
    //   uz = normalized yaw torque
    //   Fz = normalized body-Z thrust
    //
    // Then:
    //   m0 = -r*ux + r*uy + uz - Fz
    //   m1 =  r*ux - r*uy + uz - Fz
    //   m2 =  r*ux + r*uy - uz - Fz
    //   m3 = -r*ux - r*uy - uz - Fz
    //
    // PX4 uses negative body-Z for upward thrust:
    //
    //   Fz = -thrust_norm
    //
    // so equivalently:
    //
    //   m0 = -r*ux + r*uy + uz + thrust_norm
    //   m1 =  r*ux - r*uy + uz + thrust_norm
    //   m2 =  r*ux + r*uy - uz + thrust_norm
    //   m3 = -r*ux - r*uy - uz + thrust_norm
    //
    // Solving these four equations for the normalized controls:
    //
    //   ux = (-m0 + m1 + m2 - m3) / (2*sqrt(2))
    //   uy = ( m0 - m1 + m2 - m3) / (2*sqrt(2))
    //   uz = ( m0 + m1 - m2 - m3) / 4
    //   thrust_norm = (m0 + m1 + m2 + m3) / 4
    //
    // This is why the equations below convert our normalized motor
    // commands back into the VehicleTorqueSetpoint and
    // VehicleThrustSetpoint values expected by PX4.
    //
    // IMPORTANT:
    //   model.sdf physical parameters are used earlier for:
    //
    //       physical wrench [Nm,N] -> physical rotor omega
    //
    //   PX4 airframe/control-allocation geometry is used here for:
    //
    //       normalized rotor command -> normalized PX4 torque/thrust setpoint
    //
    // These are intentionally two separate models.
    // ============================================================

    constexpr double inv_2sqrt2 = 0.35355339059327376220; // 1/(2*sqrt(2))

    Eigen::Vector3d torque_norm;
    torque_norm.x() = (-motor_norm[0] + motor_norm[1] + motor_norm[2] - motor_norm[3]) * inv_2sqrt2;
    torque_norm.y() = ( motor_norm[0] - motor_norm[1] + motor_norm[2] - motor_norm[3]) * inv_2sqrt2;
    torque_norm.z() = 0.25 * ( motor_norm[0] + motor_norm[1] - motor_norm[2] - motor_norm[3]);
    const double thrust_norm = 0.25 * (motor_norm[0] + motor_norm[1] + motor_norm[2] + motor_norm[3]);

    // ============================================================
    // 8. Reconstruct the actual physical wrench requested from
    //    the selected rotor speeds. This is debug verification.
    // ============================================================

    const double reconstructed_tx = a * kf * (-omega_sq[0] + omega_sq[1] + omega_sq[2] - omega_sq[3]);
    const double reconstructed_ty = a * kf * ( omega_sq[0] - omega_sq[1] + omega_sq[2] - omega_sq[3]);
    const double reconstructed_tz = km * kf * ( omega_sq[0] + omega_sq[1] - omega_sq[2] - omega_sq[3]);
    const double reconstructed_thrust = kf * omega_sq.sum();

    // ============================================================
    // 9. Debug output
    // ============================================================

    RCLCPP_INFO_THROTTLE(this->get_logger(),*this->get_clock(),500,
        "[F450 inverse] "
        "alpha=%.3f "
        "tauCmd=[%.3f %.3f %.3f] "
        "tauReal=[%.3f %.3f %.3f] "
        "Fcmd=%.3f Freal=%.3f "
        "omega=[%.1f %.1f %.1f %.1f] "
        "uTau=[%.3f %.3f %.3f] uT=%.3f",
        alpha, tau_nm.x(), tau_nm.y(), tau_nm.z(),
        reconstructed_tx, reconstructed_ty, reconstructed_tz,
        thrust_n, reconstructed_thrust,
        omega[0], omega[1], omega[2], omega[3], 
        torque_norm.x(), torque_norm.y(), torque_norm.z(), thrust_norm);

    return {torque_norm, thrust_norm};
}

void OffboardControl::sls_offset_differential_flatness() {
    // fig8
    double t = this->get_clock()->now().seconds() - start_time_.seconds(); // clock from publisher
    double Od[3], dOd[3], ddRL[3];
    // double xipd[3], dxipd[3], d2xipd[3], d3xipd[3], d4xipd[3];
    // diff_flatness_mission_QSF(t, sls_offset_params_.load_mass_, mass_, sls_offset_params_.l, gravity_, sls_offset_params_.psi_rad_, sls_offset_params_.L_offset_, T, A, B, Od, dOd, xipd, dxipd, d2xipd,
    //                           d3xipd, d4xipd, ddRL, &sls_offset_params_.Td_scaler);

    // flatness based on low angular acc.
    double aLd[3], snapd[3];
    double Omega[3] = {sls_offset_params_.latest_rate_frd_.x(), sls_offset_params_.latest_rate_frd_.y(), sls_offset_params_.latest_rate_frd_.z()};
    Flatness_mission_spfig8(t, sls_offset_params_.load_mass_, mass_, gravity_, sls_offset_params_.l, sls_offset_params_.L_offset_, 
                            sls_offset_params_.phi_rad_, sls_offset_params_.theta_rad_, sls_offset_params_.psi_rad_, Omega, 
                            /*A=*/1.5, /*B=*/1.0, /*omega=*/0.4,
                            Od, dOd, aLd, snapd);

    // Store outputs
    sls_offset_params_.Omegad1 = Od[0];
    sls_offset_params_.Omegad2 = Od[1];
    sls_offset_params_.Omegad3 = Od[2];
    sls_offset_params_.dOmegad1 = dOd[0];
    sls_offset_params_.dOmegad2 = dOd[1];
    sls_offset_params_.dOmegad3 = dOd[2];
    sls_offset_params_.aLd1 = aLd[0];
    sls_offset_params_.aLd2 = aLd[1];
    sls_offset_params_.aLd3 = aLd[2];
    sls_offset_params_.snapd1 = snapd[0];
    sls_offset_params_.snapd2 = snapd[1];
    sls_offset_params_.snapd3 = snapd[2]; // output snap since not included in publisher's msg

    // Eigen::Vector3d pos_des(xipd[0], xipd[1], xipd[2]);
    // Eigen::Vector3d vel_des(dxipd[0], dxipd[1], dxipd[2]);
    // Eigen::Vector3d acc_des(d2xipd[0], d2xipd[1], d2xipd[2]);
    // Eigen::Vector3d jerk_des(d3xipd[0], d3xipd[1], d3xipd[2]);
    // Eigen::Vector3d snap_des(d4xipd[0], d4xipd[1], d4xipd[2]);
}

std::pair<Eigen::Vector3d, double> OffboardControl::sls_offset_attitude_to_body_rate_and_thrust(const Eigen::Vector4d &curr_att, const Eigen::Vector4d &ref_att, double ref_z_thrust) {
    // Geometric attitude controller based on Technical report
    // Brescianini, Dario, Markus Hehn, and Raffaello D'Andrea. Nonlinear
    // quadrocopter attitude control: Technical report. ETH Zurich, 2013.

    const Eigen::Vector4d inverse(1.0, -1.0, -1.0, -1.0);
    const Eigen::Vector4d q_inv = inverse.asDiagonal() * curr_att;

    // Performs q_e = q^(-1) * q_d
    const Eigen::Vector4d qe = multiply_quaternion(q_inv, ref_att);

    // Compute desired body rates
    Eigen::Vector3d desired_rate;
    desired_rate(0) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(1);
    desired_rate(1) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(2);
    desired_rate(2) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(3);

    // Clamp desired rate
    for (auto &rate : desired_rate) {
        if (std::abs(rate) > ref_rate_limit_) {
            rate = std::copysign(ref_rate_limit_, rate);
        }
    }

    // Use the calibrated thrust-motor curve to compute the final normalized
    // thrust command
    const auto normalized_thrust = std::max(0.0, std::min(1.0, norm_thrust_const_ * ref_z_thrust + norm_thrust_offset_));

    // RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,
    //     "[Thrust Debug] ref_acc(Z): %.3f | zb(Z): %.3f | ref_z_thrust: %.3f
    //     | norm_thrust: %.3f | hover_param: %.3f", ref_acc(2), zb(2),
    //     ref_z_thrust, normalized_thrust, hover_thrust_);
    return {desired_rate, normalized_thrust};
}

Eigen::Vector4d OffboardControl::acceleration_to_quaternion(const Eigen::Vector3d &vector_acc, const double &yaw) {
    Eigen::Vector4d quat;
    Eigen::Vector3d zb_des, yb_des, xb_des, proj_xb_des;
    Eigen::Matrix3d rotmat;

    // Given a desired yaw angle ψ_d, the projected desired heading direction in
    // the horizontal plane is defined as:
    proj_xb_des << std::cos(yaw), std::sin(yaw), 0.0;

    // The desired body z-axis is aligned with the desired thrust direction
    zb_des = vector_acc / vector_acc.norm();

    // The desired body y-axis is constructed from the cross product between the
    // desired thrust direction and the projected heading direction
    yb_des = zb_des.cross(proj_xb_des) / (zb_des.cross(proj_xb_des)).norm();

    // The desired body x-axis is then the cross product between the desired
    // body y and z axes
    xb_des = yb_des.cross(zb_des) / (yb_des.cross(zb_des)).norm();

    // Then The desired rotation matrix is formed as follows:
    rotmat << xb_des(0), yb_des(0), zb_des(0), xb_des(1), yb_des(1), zb_des(1), xb_des(2), yb_des(2), zb_des(2);

    // Lastly the rotation matrix is converted to a quaternion
    quat = rotation_matrix_to_quaternion(rotmat);
    return quat;
}

std::pair<Eigen::Vector3d, double> OffboardControl::attitude_to_body_rate_and_thrust(const Eigen::Vector4d &curr_att, const Eigen::Vector4d &ref_att, const Eigen::Vector3d &ref_acc) {
    // Geometric attitude controller based on Technical report
    // Brescianini, Dario, Markus Hehn, and Raffaello D'Andrea. Nonlinear
    // quadrocopter attitude control: Technical report. ETH Zurich, 2013.

    const Eigen::Vector4d inverse(1.0, -1.0, -1.0, -1.0);
    const Eigen::Vector4d q_inv = inverse.asDiagonal() * curr_att;

    // Performs q_e = q^(-1) * q_d
    const Eigen::Vector4d qe = multiply_quaternion(q_inv, ref_att);

    // Compute desired body rates
    Eigen::Vector3d desired_rate;
    desired_rate(0) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(1);
    desired_rate(1) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(2);
    desired_rate(2) = (2.0 / attitude_tau_) * std::copysign(1.0, qe(0)) * qe(3);

    // Clamp desired rate
    for (auto &rate : desired_rate) {
        if (std::abs(rate) > ref_rate_limit_) {
            rate = std::copysign(ref_rate_limit_, rate);
        }
    }

    // Get the current body z-axis
    const Eigen::Matrix3d rotation_matrix = quaternion_to_rotation_matrix(curr_att);
    const Eigen::Vector3d zb = rotation_matrix.col(2);

    // Project desired acceleration onto zb to get desired thrust magnitude (we
    // only care about the thrust component along the z-axis)
    const auto desired_thrust = ref_acc.dot(zb);

    // Use the calibrated thrust-motor curve to compute the final normalized
    // thrust command
    const auto normalized_thrust = std::max(0.0, std::min(1.0, norm_thrust_const_ * desired_thrust + norm_thrust_offset_));

    // RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 500,
    //     "[Thrust Debug] ref_acc(Z): %.3f | zb(Z): %.3f | desired_thrust: %.3f
    //     | norm_thrust: %.3f | hover_param: %.3f", ref_acc(2), zb(2),
    //     desired_thrust, normalized_thrust, hover_thrust_);
    return {desired_rate, normalized_thrust};
}

void OffboardControl::publish_inner_integral_debug(const double eI[3]) {
    geometry_msgs::msg::Vector3Stamped msg{};

    msg.header.stamp = this->get_clock()->now();
    msg.header.frame_id = "frd";

    msg.vector.x = eI[0];
    msg.vector.y = eI[1];
    msg.vector.z = eI[2];

    inner_integral_debug_publisher_->publish(msg);
}

void OffboardControl::publish_qsf_controller_output(double des_thrust, const double tau[3]) {
    geometry_msgs::msg::WrenchStamped msg{};

    msg.header.stamp = this->now();
    msg.header.frame_id = "frd";

    // QSF thrust output [N]
    msg.wrench.force.x = 0.0;
    msg.wrench.force.y = 0.0;
    msg.wrench.force.z = des_thrust; // thrust force magnitude

    // Inner-loop torque output before normalization [N m]
    msg.wrench.torque.x = tau[0];
    msg.wrench.torque.y = tau[1];
    msg.wrench.torque.z = tau[2];

    controller_output_debug_publisher_->publish(msg);
}

void OffboardControl::publish_qsf_attitude_debug() {
    geometry_msgs::msg::PoseArray msg;

    msg.header.stamp = this->now();
    msg.header.frame_id = "ned";

    msg.poses.resize(2);

    // --------------------------------------------------
    // pose[0]: desired attitude R_Bd
    // R_Bd is already NED / FRD
    // --------------------------------------------------
    Eigen::Vector4d q_des_vec = rotation_matrix_to_quaternion(sls_offset_params_.R_Bd);

    Eigen::Quaterniond q_des_ned(q_des_vec(0), q_des_vec(1), q_des_vec(2),q_des_vec(3));
    q_des_ned.normalize();

    msg.poses[0].orientation.w = q_des_ned.w();
    msg.poses[0].orientation.x = q_des_ned.x();
    msg.poses[0].orientation.y = q_des_ned.y();
    msg.poses[0].orientation.z = q_des_ned.z();


    // --------------------------------------------------
    // pose[1]: actual UAV attitude
    // latest_attitude_ is stored as ENU / FLU,
    // so convert back to NED / FRD.
    // --------------------------------------------------
    Eigen::Quaterniond q_actual_enu(latest_attitude_(0), latest_attitude_(1), latest_attitude_(2), latest_attitude_(3));
    q_actual_enu.normalize();
    Eigen::Quaterniond q_actual_ned = px4_ros_com::frame_transforms::ros_to_px4_orientation(q_actual_enu);
    q_actual_ned.normalize();

    msg.poses[1].orientation.w = q_actual_ned.w();
    msg.poses[1].orientation.x = q_actual_ned.x();
    msg.poses[1].orientation.y = q_actual_ned.y();
    msg.poses[1].orientation.z = q_actual_ned.z();

    qsf_attitude_debug_publisher_->publish(msg);
}

void OffboardControl::publish_attitude_setpoints(const double &thrust_cmd, const Eigen::Vector4d &target_attitude_enu) {
    VehicleAttitudeSetpoint msg{};
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    // Transform from ENU to NED
    Eigen::Quaterniond q_enu(target_attitude_enu(0), target_attitude_enu(1), target_attitude_enu(2), target_attitude_enu(3));
    Eigen::Quaterniond target_att_ned = px4_ros_com::frame_transforms::ros_to_px4_orientation(q_enu);

    // Assign quaternions
    msg.q_d[0] = static_cast<float>(target_att_ned.w());
    msg.q_d[1] = static_cast<float>(target_att_ned.x());
    msg.q_d[2] = static_cast<float>(target_att_ned.y());
    msg.q_d[3] = static_cast<float>(target_att_ned.z());

    // Assign thrust
    msg.thrust_body[0] = 0.0f;
    msg.thrust_body[1] = 0.0f;
    msg.thrust_body[2] = static_cast<float>(-thrust_cmd);

    attitude_setpoint_publisher_->publish(msg);
}

void OffboardControl::publish_rate_setpoints(const Eigen::Vector3d &rate_cmd_flu, const double &thrust_cmd) {
    VehicleRatesSetpoint msg{};
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;

    // Convert FLU (ROS2 body) to FRD (px4 body) frame
    // Baselink frame = FLU for ROS2, aircraft frame = FRD for px4
    Eigen::Vector3d body_rate_frd = px4_ros_com::frame_transforms::baselink_to_aircraft_body_frame(rate_cmd_flu);

    // Assign RPY
    msg.roll = static_cast<float>(body_rate_frd(0));
    msg.pitch = static_cast<float>(body_rate_frd(1));
    msg.yaw = static_cast<float>(body_rate_frd(2));

    // Assign thrust
    msg.thrust_body[0] = 0.0f;
    msg.thrust_body[1] = 0.0f;
    msg.thrust_body[2] = static_cast<float>(-thrust_cmd);

    rate_setpoint_publisher_->publish(msg);
}

void OffboardControl::publish_torque_thrust_setpoints(const Eigen::Vector3d &torque_cmd, double thrust_cmd) {
    // Thrust and torque control
    VehicleTorqueSetpoint torque_msg{};
    torque_msg.xyz = {static_cast<float>(torque_cmd(0)), static_cast<float>(torque_cmd(1)), static_cast<float>(torque_cmd(2))};
    torque_msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    torque_setpoint_publisher_->publish(torque_msg);

    VehicleThrustSetpoint thrust_msg{};
    thrust_msg.xyz = {0.0, 0.0, static_cast<float>(-thrust_cmd)};
    thrust_msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    thrust_setpoint_publisher_->publish(thrust_msg);
}

inline Eigen::Vector4d OffboardControl::rotation_matrix_to_quaternion(const Eigen::Matrix3d &R) {
    // Computes the rotation matrix to quaternion conversion
    Eigen::Vector4d quat;
    double tr = R.trace();
    if (tr > 0.0) {
        double S = sqrt(tr + 1.0) * 2.0; // S=4*qw
        quat(0) = 0.25 * S;
        quat(1) = (R(2, 1) - R(1, 2)) / S;
        quat(2) = (R(0, 2) - R(2, 0)) / S;
        quat(3) = (R(1, 0) - R(0, 1)) / S;
    } else if ((R(0, 0) > R(1, 1)) & (R(0, 0) > R(2, 2))) {
        double S = sqrt(1.0 + R(0, 0) - R(1, 1) - R(2, 2)) * 2.0; // S=4*qx
        quat(0) = (R(2, 1) - R(1, 2)) / S;
        quat(1) = 0.25 * S;
        quat(2) = (R(0, 1) + R(1, 0)) / S;
        quat(3) = (R(0, 2) + R(2, 0)) / S;
    } else if (R(1, 1) > R(2, 2)) {
        double S = sqrt(1.0 + R(1, 1) - R(0, 0) - R(2, 2)) * 2.0; // S=4*qy
        quat(0) = (R(0, 2) - R(2, 0)) / S;
        quat(1) = (R(0, 1) + R(1, 0)) / S;
        quat(2) = 0.25 * S;
        quat(3) = (R(1, 2) + R(2, 1)) / S;
    } else {
        double S = sqrt(1.0 + R(2, 2) - R(0, 0) - R(1, 1)) * 2.0; // S=4*qz
        quat(0) = (R(1, 0) - R(0, 1)) / S;
        quat(1) = (R(0, 2) + R(2, 0)) / S;
        quat(2) = (R(1, 2) + R(2, 1)) / S;
        quat(3) = 0.25 * S;
    }
    return quat;
}

inline Eigen::Vector4d OffboardControl::multiply_quaternion(const Eigen::Vector4d &q, const Eigen::Vector4d &p) {
    // Multiplies two quaternions
    Eigen::Vector4d quat;
    quat << p(0) * q(0) - p(1) * q(1) - p(2) * q(2) - p(3) * q(3), p(0) * q(1) + p(1) * q(0) - p(2) * q(3) + p(3) * q(2), p(0) * q(2) + p(1) * q(3) + p(2) * q(0) - p(3) * q(1),
        p(0) * q(3) - p(1) * q(2) + p(2) * q(1) + p(3) * q(0);
    return quat;
}

inline Eigen::Matrix3d OffboardControl::quaternion_to_rotation_matrix(const Eigen::Vector4d &q) {
    // Converts a quaternion to a rotation matrix
    Eigen::Matrix3d rotmat;
    rotmat << q(0) * q(0) + q(1) * q(1) - q(2) * q(2) - q(3) * q(3), 2 * q(1) * q(2) - 2 * q(0) * q(3), 2 * q(0) * q(2) + 2 * q(1) * q(3),

        2 * q(0) * q(3) + 2 * q(1) * q(2), q(0) * q(0) - q(1) * q(1) + q(2) * q(2) - q(3) * q(3), 2 * q(2) * q(3) - 2 * q(0) * q(1),

        2 * q(1) * q(3) - 2 * q(0) * q(2), 2 * q(0) * q(1) + 2 * q(2) * q(3), q(0) * q(0) - q(1) * q(1) - q(2) * q(2) + q(3) * q(3);
    return rotmat;
}

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto offboard_node = std::make_shared<OffboardControl>("/fmu/");
    rclcpp::spin(offboard_node);
    rclcpp::shutdown();
    return 0;
}
