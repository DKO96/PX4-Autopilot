/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#pragma once

// PX4 includes
#include <px4_platform_common/module_params.h>

// Libraries
#include <lib/rover_control/RoverControl.hpp>
#include <lib/slew_rate/SlewRate.hpp>
#include <math.h>

// uORB includes
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/actuator_motors.h>
#include <uORB/topics/rover_steering_setpoint.h>
#include <uORB/topics/rover_throttle_setpoint.h>

/**
 * @brief Class for mecanum actuator control.
 */
class MecanumActControl : public ModuleParams
{
public:
	/**
	 * @brief Constructor for MecanumActControl.
	 * @param parent The parent ModuleParams object.
	 */
	MecanumActControl(ModuleParams *parent);
	~MecanumActControl() = default;

	/**
	 * @brief Generate and publish actuatorMotors setpoints from roverThrottleSetpoint/roverSteeringSetpoint.
	 */
	void updateActControl();

	/**
	 * @brief Stop the vehicle by sending 0 commands to motors and servos.
	 */
	void stopVehicle();

protected:
	/**
	 * @brief Update the parameters of the module.
	 */
	void updateParams() override;

private:
	/**
	 * @brief Compute normalized motor commands based on normalized setpoints.
	 * @param throttle_body_x Normalized speed in body x direction [-1, 1].
	 * @param throttle_body_y Normalized speed in body y direction [-1, 1].
	 * @param speed_diff_normalized Speed difference between left and right wheels [-1, 1].
	 * @return Motor speeds [-1, 1].
	 */
	Vector4f computeInverseKinematics(float throttle_body_x, float throttle_body_y, const float speed_diff_normalized);

	// uORB subscriptions
	uORB::Subscription _actuator_motors_sub{ORB_ID(actuator_motors)};
	uORB::Subscription _rover_steering_setpoint_sub{ORB_ID(rover_steering_setpoint)};
	uORB::Subscription _rover_throttle_setpoint_sub{ORB_ID(rover_throttle_setpoint)};

	// uORB publications
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};

	// Variables
	hrt_abstime _timestamp{0};
	float _throttle_x_setpoint{NAN};
	float _throttle_y_setpoint{NAN};
	float _speed_diff_setpoint{NAN};

	// Controllers
	SlewRate<float> _adjusted_throttle_x_setpoint{0.f};
	SlewRate<float> _adjusted_throttle_y_setpoint{0.f};

	// Parameters
	DEFINE_PARAMETERS(
		(ParamInt<px4::params::CA_R_REV>)           _param_r_rev,
		(ParamFloat<px4::params::RO_ACCEL_LIM>)     _param_ro_accel_limit,
		(ParamFloat<px4::params::RO_DECEL_LIM>)     _param_ro_decel_limit,
		(ParamFloat<px4::params::RO_MAX_THR_SPEED>) _param_ro_max_thr_speed
	)
};
