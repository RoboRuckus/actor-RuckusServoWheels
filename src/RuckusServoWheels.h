/*
 * This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
 * 
 * External libraries needed:
 * RoboRuckusMovement: https://github.com/RoboRuckus/util-RoboRuckusMovement
 * ArduinoJSON: https://arduinojson.org/
 * ESP32Servo: https://github.com/madhephaestus/ESP32Servo
 * 
 * Contributors: Sam Groveman
 */
#pragma once
#include <ArduinoJson.h>
#include <RoboRuckusMovement.h>
#include <ESP32Servo.h>

/// @brief Class for RoboRuckus bot movement via CR servos
class RuckusServoWheels : public RoboRuckusMovement {
	public:
		RuckusServoWheels(String Name, int RightPin, int LeftPin, String ConfigFile = "RuckusServoWheels.json");
		RuckusServoWheels(String Name, int RightFrontPin, int LeftFrontPin, int RightRearPin, int LeftRearPin, String ConfigFile = "RuckusServoWheels.json");
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);

	protected:
		/// @brief Stores if this device uses four wheels (two otherwise)
		bool four_wheels = false;
		
		/// @brief Stores path to settings file
		String config_path;

		/// @brief Configuration for servo wheels
		struct {
			/// @brief Name of currently assigned navigation sensor
			String navSensor = "None";

			/// @brief Pins used by wheels
			int frontRightPin, frontLeftPin, rearRightPin, rearLeftPin;

			/// @brief Forward speeds of wheels
			int frontRightForward = 15, frontLeftForward = 165, rearRightForward = 15, rearLeftForward = 165;

			/// @brief Backward speeds of wheels
			int frontRightBackward = 165, frontLeftBackward = 15, rearRightBackward = 165, rearLeftBackward = 15;

			/// @brief Wheel zero positions
			int frontRightZero = 90, frontLeftZero = 90, rearRightZero = 90, rearLeftZero = 90;

			/// @brief Minimum servo pulse
			int servoMin = 544;

			/// @brief Maximum servo pulse
			int servoMax = 2400;

			/// @brief JSON string that contains the upper and lower limits for settings, can be used to make sliders in interface
			String limits = R"({
				"frontRightForward": {"min": 0, "max": 180, "increment": 1},
				"frontLeftForward": {"min": 0, "max": 180, "increment": 1},
				"rearRightForward": {"min": 0, "max": 180, "increment": 1},
				"rearLeftForward": {"min": 0, "max": 180, "increment": 1},
				"frontRightBackward": {"min": 0, "max": 180, "increment": 1},
				"frontLeftBackward": {"min": 0, "max": 180, "increment": 1},
				"rearRightBackward": {"min": 0, "max": 180, "increment": 1},
				"rearLeftBackward": {"min": 0, "max": 180, "increment": 1},
				"frontRightZero": {"min": 80, "max": 100, "increment": 1},
				"frontLeftZero": {"min": 80, "max": 100, "increment": 1},
				"rearRightZero": {"min": 80, "max": 100, "increment": 1},
				"rearLeftZero": {"min": 80, "max": 100, "increment": 1},
				"linearTime": {"min": 500, "max": 2000, "increment": 10},
				"turnTime": {"min": 250, "max": 2000, "increment": 10},
				"driftBoost": {"min": 0, "max": 20, "increment": 1}
			})";
		} wheel_config;

		/// @brief Servos used by wheels
		Servo front_right, front_left, rear_right, rear_left;

		void startMove();
		void endMove();
		bool shouldStop();
		void correctDrift();

		/// @brief Possible states of a compound (slide) move
		enum compoundMoveState {START, LEFT, RIGHT, FORWARD};

		/// @brief True when the move being executed is a compound (slide) move
		bool compoundMove = false;

		/// @brief Stores the type of (slide) move being executed
		RuckusCommunicator::MoveTypes compoundMoveType;

		/// @brief Tracks which state of a compound (slide) move is currently active
		compoundMoveState currentMoveState = START;

		/// @brief Stores the magnitude for compound (slide) moves
		int compoundMoveMagnitude;

		bool checkForEnd();
		void resetMove();
};
