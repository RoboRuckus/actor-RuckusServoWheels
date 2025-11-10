#include"RuckusServoWheels.h"

/// @brief Creates a RoboRuckus LED matrix controller
/// @param Name The device name
/// @param RightPin The pin used by the right wheel
/// @param LeftPin The pin used by the left wheel
/// @param ConfigFile The name of the config file
RuckusServoWheels::RuckusServoWheels(String Name, int RightPin, int LeftPin, String ConfigFile) : RoboRuckusMovement(Name) {
	config_path = "/settings/sen/" + ConfigFile;
	wheel_config.frontLeftPin = LeftPin;
	wheel_config.frontRightPin = RightPin;
}

/// @brief Creates a RoboRuckus LED matrix controller
/// @param Name The device name
/// @param LeftFrontPin The pin used by the front left wheel
/// @param RightFrontPin The pin used by the front right wheel
/// @param RightRearPin The pin used by the rear right wheel
/// @param LeftRearPin The pin used by the rear left wheel
/// @param ConfigFile The name of the config file
RuckusServoWheels::RuckusServoWheels(String Name, int RightFrontPin, int LeftFrontPin, int RightRearPin, int LeftRearPin, String ConfigFile) : RoboRuckusMovement(Name) {
	config_path = "/settings/sen/" + ConfigFile;
	wheel_config.frontLeftPin = LeftFrontPin;
	wheel_config.frontRightPin = RightFrontPin;
	wheel_config.rearLeftPin = LeftRearPin;
	wheel_config.rearRightPin = RightRearPin;
	four_wheels = true;
}

/// @brief Starts a RoboRuckus LED matrix controller
/// @return True on success
bool RuckusServoWheels::begin() {
	bool result = false;
	// Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

	// Set description
	Description.type = "movement";
	Description.version = "1.0.0";
	if (!checkConfig(config_path)) {
		// Set defaults
		return setConfig(getConfig(), true);
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Gets the current config
/// @return A JSON string of the config
String RuckusServoWheels::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;
	doc["navSensor"]["current"] = wheel_config.navSensor;
	doc["navSensor"]["options"][0] = "None";
	for (int i = 0; i < RoboRuckusSensor::ruckusSensors.size(); i++) {
		doc["navSensor"]["options"][i + 1] = RoboRuckusSensor::ruckusSensors[i]->sensorName;
	}
	
	// Get pins
	doc["frontRightPin"] = wheel_config.frontRightPin;
	doc["frontLeftPin"] = wheel_config.frontLeftPin;
	if (four_wheels) {
		doc["rearRightPin"] = wheel_config.rearRightPin;
		doc["rearLeftPin"] = wheel_config.rearLeftPin;
	}

	// Get wheel forward speeds
	doc["frontRightForward"] = wheel_config.frontRightForward;
	doc["frontLeftForward"] = wheel_config.frontLeftForward;
	if (four_wheels) {
		doc["rearRightForward}"] = wheel_config.rearRightForward;
		doc["rearLeftForward"] = wheel_config.rearLeftForward;
	}

	// Get wheel backward speeds
	doc["frontRightBackward"] = wheel_config.frontRightBackward;
	doc["frontLeftBackward"] = wheel_config.frontLeftBackward;
	if (four_wheels) {
		doc["rearRightBackward}"] = wheel_config.rearRightBackward;
		doc["rearLeftBackward"] = wheel_config.rearLeftBackward;
	}

	// Get wheel zero positions
	doc["frontRightZero"] = wheel_config.frontLeftZero;
	doc["frontLeftZero"] = wheel_config.frontLeftZero;
	if (four_wheels) {
		doc["rearRightZero}"] = wheel_config.rearRightZero;
		doc["rearLeftZero"] = wheel_config.rearLeftZero;
	}

	// Get movement settings
	doc["linearTime"] = RoboRuckusMovement::move_config.linearTime;
	doc["linearDistance"] = RoboRuckusMovement::move_config.linearDistance;
	doc["linearDrift"] = RoboRuckusMovement::move_config.linearDrift;
	doc["turnTime"] = RoboRuckusMovement::move_config.turnTime;
	doc["turnDistance"] = RoboRuckusMovement::move_config.turnDistance;
	doc["turnDrift"] = RoboRuckusMovement::move_config.turnDrift;
	doc["driftBoost"] = RoboRuckusMovement::move_config.driftBoost;

	doc["servoMin"] = wheel_config.servoMin;
	doc["servoMax"] = wheel_config.servoMax;

	doc["limits"] = wheel_config.limits;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool RuckusServoWheels::setConfig(String config, bool save) {
	// Allocate the JSON document
	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}

	// Assign loaded values
	wheel_config.servoMax = doc["servoMax"].as<int>();
	wheel_config.servoMin = doc["servoMin"].as<int>();
	wheel_config.frontRightPin = doc["frontRightPin"].as<int>();
	wheel_config.frontLeftPin = doc["frontLeftPin"].as<int>();
	// Attach servos
	front_right.setPeriodHertz(50);
	front_right.attach(wheel_config.frontRightPin, wheel_config.servoMin, wheel_config.servoMax);
	front_left.setPeriodHertz(50);
	front_left.attach(wheel_config.frontLeftPin, wheel_config.servoMin, wheel_config.servoMax);
	if (four_wheels) {
		wheel_config.rearRightPin = doc["rearRightPin"].as<int>();
		wheel_config.rearLeftPin = doc["rearLeftPin"].as<int>();
		// Attach servos
		rear_right.setPeriodHertz(50);
		rear_right.attach(wheel_config.rearRightPin, wheel_config.servoMin, wheel_config.servoMax);
		rear_left.setPeriodHertz(50);
		rear_left.attach(wheel_config.rearLeftPin, wheel_config.servoMin, wheel_config.servoMax);
	}

	wheel_config.frontRightForward = doc["frontRightForward"].as<int>();
	wheel_config.frontLeftForward = doc["frontLeftForward"].as<int>();
	if (four_wheels) {
		wheel_config.rearRightForward = doc["rearRightForward"].as<int>();
		wheel_config.rearLeftForward = doc["rearLeftForward"].as<int>();
	}

	wheel_config.frontRightBackward = doc["frontRightBackward"].as<int>();
	wheel_config.frontLeftBackward = doc["frontLeftBackward"].as<int>();
	if (four_wheels) {
		wheel_config.rearRightBackward = doc["rearRightBackward"].as<int>();
		wheel_config.rearLeftBackward = doc["rearLeftBackward"].as<int>();
	}

	wheel_config.frontRightZero = doc["frontRightZero"].as<int>();
	wheel_config.frontLeftZero = doc["frontLeftZero"].as<int>();
	front_right.write(wheel_config.frontRightZero);
	front_left.write(wheel_config.frontLeftZero);
	if (four_wheels) {
		wheel_config.rearRightZero = doc["rearRightZero"].as<int>();
		wheel_config.rearLeftZero = doc["rearLeftZero"].as<int>();
		rear_right.write(wheel_config.rearRightZero);
		rear_left.write(wheel_config.rearLeftZero);
	}

	RoboRuckusMovement::move_config.linearTime = doc["linearTime"].as<int>();
	RoboRuckusMovement::move_config.linearDistance = doc["linearDistance"].as<float>();
	RoboRuckusMovement::move_config.linearDrift = doc["linearDrift"].as<float>();
	RoboRuckusMovement::move_config.turnTime = doc["turnTime"].as<int>();
	RoboRuckusMovement::move_config.turnDistance = doc["turnDistance"].as<float>();
	RoboRuckusMovement::move_config.turnDrift = doc["turnDrift"].as<float>();
	RoboRuckusMovement::move_config.driftBoost = doc["driftBoost"].as<float>();

	wheel_config.navSensor = doc["navSensor"]["current"].as<String>();
	if (wheel_config.navSensor != "None") {
		if(!assignSensor(wheel_config.navSensor)) {
			return false;
		}
	} else {
		navSensor = nullptr;
	}
	
	if (save) {
		// Save space by not storing static limits
		doc["limits"] = "";
		return saveConfig(config_path, config);
	}
	return true;
}

/// @brief Starts a move
void RuckusServoWheels::startMove() {
	if (navSensor != nullptr) {
		navSensor->startMove(currentMove);
	}
	moveStartTime = millis();
	switch (currentMove) {
		case RuckusCommunicator::MoveTypes::FORWARD:
			front_right.write(wheel_config.frontRightForward);
			front_left.write(wheel_config.frontLeftForward);
			if (four_wheels) {
				rear_right.write(wheel_config.rearRightForward);
				rear_left.write(wheel_config.rearLeftForward);
			}
			break;
		case RuckusCommunicator::MoveTypes::BACKWARD:
			front_right.write(wheel_config.frontRightBackward);
			front_left.write(wheel_config.frontLeftBackward);
			if (four_wheels) {
				rear_right.write(wheel_config.rearRightBackward);
				rear_left.write(wheel_config.rearLeftBackward);
			}
			break;
		case RuckusCommunicator::MoveTypes::TURNLEFT:
			front_right.write(wheel_config.frontRightBackward);
			front_left.write(wheel_config.frontLeftForward);
			if (four_wheels) {
				rear_right.write(wheel_config.rearRightBackward);
				rear_left.write(wheel_config.rearLeftForward);
			}
			break;
		case RuckusCommunicator::MoveTypes::TURNRIGHT:
			front_right.write(wheel_config.frontRightForward);
			front_left.write(wheel_config.frontLeftBackward);
			if (four_wheels) {
				rear_right.write(wheel_config.rearRightForward);
				rear_left.write(wheel_config.rearLeftBackward);
			}
			break;
		case RuckusCommunicator::SLIDELEFT:
		case RuckusCommunicator::SLIDERIGHT:
			compoundMove = true;
			currentMoveState = START;
			break;
	}
}

/// @brief End movement
void RuckusServoWheels::endMove() {
	resetMove();
	compoundMove = false;
}

/// @brief Checks if a movement should stop
/// @return True if the movement should stop
bool RuckusServoWheels::shouldStop() {
	bool done = false;
	if (!compoundMove) {
		done = checkForEnd();
	} else {
		switch (currentMoveState) {
			case START:
				compoundMoveType = currentMove;
				compoundMoveMagnitude = currentMagnitude;
				currentMagnitude = 1;
				if (compoundMoveType == RuckusCommunicator::SLIDELEFT) {
					currentMove = RuckusCommunicator::TURNLEFT;
					currentMoveState = LEFT;
				} else {
					currentMove = RuckusCommunicator::TURNRIGHT;
					currentMoveState = RIGHT;
				}
				startMove();
				break;
			case LEFT:
				if (checkForEnd()) {
					if (compoundMoveType == RuckusCommunicator::SLIDELEFT) {
						resetMove();
						delay(250);
						currentMove = RuckusCommunicator::FORWARD;
						currentMoveState = FORWARD;
						currentMagnitude = compoundMoveMagnitude;
						startMove();
					} else {
						done = true;
					}
				}
				break;
			case RIGHT:
				if (checkForEnd()) {
					if (compoundMoveType == RuckusCommunicator::SLIDELEFT) {
						done = true;
					} else {
						resetMove();
						delay(250);
						currentMove = RuckusCommunicator::FORWARD;
						currentMoveState = FORWARD;
						currentMagnitude = compoundMoveMagnitude;
						startMove();
					}
				}
				break;
			case FORWARD:
				if (checkForEnd()) {
					resetMove();
					delay(250);
					currentMagnitude = 1;
					if (compoundMoveType == RuckusCommunicator::SLIDELEFT) {
						currentMove = RuckusCommunicator::TURNRIGHT;
						currentMoveState = RIGHT;
					} else {
						currentMove = RuckusCommunicator::TURNLEFT;
						currentMoveState = LEFT;
						startMove();
					}
					startMove();
				}
				break;
		}
	}
	return done;
}

/// @brief Adjusts the wheel speeds based on drift measurements, if supported
void RuckusServoWheels::correctDrift() {
	if (navSensor != nullptr) {
		int frontWheelRight;
		int frontWheelLeft;
		int rearWheelRight;
		int rearWheelLeft;
		bool shouldCorrect = false;
		switch (currentMove) {
			case RuckusCommunicator::MoveTypes::FORWARD:
				shouldCorrect = navSensor->driftModes.forward;
				frontWheelRight = wheel_config.frontRightForward;
				frontWheelLeft = wheel_config.frontLeftForward;
				if (four_wheels) {
					rearWheelRight = wheel_config.rearRightForward;
					rearWheelLeft = wheel_config.rearLeftForward;
				}
				break;
			case RuckusCommunicator::MoveTypes::BACKWARD:
				shouldCorrect = navSensor->driftModes.backward;
				frontWheelRight = wheel_config.frontRightBackward;
				frontWheelLeft = wheel_config.frontLeftBackward;
				if (four_wheels) {
					rearWheelRight = wheel_config.rearRightBackward;
					rearWheelLeft = wheel_config.rearLeftBackward;
				}
				break;
		}
		if (shouldCorrect) {			
			std::tuple<RoboRuckusSensor::Direction, float> result =	navSensor->checkDrift();
			if (std::get<1>(result) >= RoboRuckusMovement::move_config.linearDrift) {
				if (std::get<0>(result) == RoboRuckusSensor::LEFT) {
					switch (currentMove) {
						case RuckusCommunicator::MoveTypes::FORWARD:
							if (frontWheelLeft < wheel_config.frontLeftZero) {
								frontWheelLeft -= RoboRuckusMovement::move_config.driftBoost;
							} else {
								frontWheelLeft += RoboRuckusMovement::move_config.driftBoost;
							}
							if (four_wheels) {
								if (rearWheelLeft < wheel_config.rearLeftZero) {
									rearWheelLeft -= RoboRuckusMovement::move_config.driftBoost;
								} else {
									rearWheelLeft += RoboRuckusMovement::move_config.driftBoost;
								}
							}
							break;
						case RuckusCommunicator::MoveTypes::BACKWARD:
							if (frontWheelRight < wheel_config.frontRightZero) {
								frontWheelRight -= RoboRuckusMovement::move_config.driftBoost;
							} else {
								frontWheelRight += RoboRuckusMovement::move_config.driftBoost;
							}
							if (four_wheels) {
								if (rearWheelRight < wheel_config.rearRightZero) {
									rearWheelRight -= RoboRuckusMovement::move_config.driftBoost;
								} else {
									rearWheelRight += RoboRuckusMovement::move_config.driftBoost;
								}
							}
							break;
					}	
				} else if (std::get<0>(result) == RoboRuckusSensor::RIGHT) {
					switch (currentMove) {
						case RuckusCommunicator::MoveTypes::FORWARD:
							if (frontWheelRight < wheel_config.frontRightZero) {
								frontWheelRight -= RoboRuckusMovement::move_config.driftBoost;
							} else {
								frontWheelRight += RoboRuckusMovement::move_config.driftBoost;
							}
							if (four_wheels) {
								if (rearWheelRight < wheel_config.rearRightZero) {
									rearWheelRight -= RoboRuckusMovement::move_config.driftBoost;
								} else {
									rearWheelRight += RoboRuckusMovement::move_config.driftBoost;
								}
							}
							break;
						case RuckusCommunicator::MoveTypes::BACKWARD:
							if (frontWheelLeft < wheel_config.frontLeftZero) {
								frontWheelLeft -= RoboRuckusMovement::move_config.driftBoost;
							} else {
								frontWheelLeft += RoboRuckusMovement::move_config.driftBoost;
							}
							if (four_wheels) {
								if (rearWheelLeft < wheel_config.rearLeftZero) {
									rearWheelLeft -= RoboRuckusMovement::move_config.driftBoost;
								} else {
									rearWheelLeft += RoboRuckusMovement::move_config.driftBoost;
								}
							}
							break;
					}	
				}
			}
			front_right.write(frontWheelRight);
			front_left.write(frontWheelLeft);
			if (four_wheels) {
				rear_right.write(rearWheelRight);
				rear_left.write(rearWheelLeft);
			}
		}
	}
}

/// @brief Custom function to check if a non-slide move should end
/// @return True if the move should stop
bool RuckusServoWheels::checkForEnd() {
	long timeMoving = millis() - moveStartTime;
	bool done = false;
	switch (currentMove) {
		case RuckusCommunicator::MoveTypes::FORWARD:
			if (timeMoving >= (currentMagnitude * RoboRuckusMovement::move_config.linearTime)) {
				done = true;
			} else {
				if (navSensor != nullptr) {
					if (navSensor->movementModes.forward) {
						std::tuple<RoboRuckusSensor::Direction, float> result =	navSensor->checkDistance();
						if (std::get<0>(result) == RoboRuckusSensor::FORWARD) {
							done = std::get<1>(result) >= (RoboRuckusMovement::move_config.linearDistance * currentMagnitude);
						}
					}
				}
			}
			break;
		case RuckusCommunicator::MoveTypes::BACKWARD:
			if (timeMoving > (currentMagnitude * RoboRuckusMovement::move_config.linearTime)) {
				done = true;
			} else {
				if (navSensor != nullptr) {
					if (navSensor->movementModes.backward) {
						std::tuple<RoboRuckusSensor::Direction, float> result =	navSensor->checkDistance();
						if (std::get<0>(result) == RoboRuckusSensor::BACKWARD) {
							done = std::get<1>(result) >= (RoboRuckusMovement::move_config.linearDistance * currentMagnitude);
						}
					}
				}
			}
			break;
		case RuckusCommunicator::MoveTypes::TURNRIGHT:
			if (timeMoving > (currentMagnitude * RoboRuckusMovement::move_config.turnTime)) {
				done = true;
			} else {
				if (navSensor != nullptr) {
					if (navSensor->movementModes.turnRight) {
						std::tuple<RoboRuckusSensor::Direction, float> result =	navSensor->checkDistance();
						if (std::get<0>(result) == RoboRuckusSensor::RIGHT) {
							done = std::get<1>(result) >= (RoboRuckusMovement::move_config.turnDistance * currentMagnitude);
						}
					}
				}
			}
			break;
		case RuckusCommunicator::MoveTypes::TURNLEFT:
			if (timeMoving > (currentMagnitude * RoboRuckusMovement::move_config.turnTime)) {
				done = true;
			} else {
				if (navSensor != nullptr) {
					if (navSensor->movementModes.turnLeft) {
						std::tuple<RoboRuckusSensor::Direction, float> result =	navSensor->checkDistance();
						if (std::get<0>(result) == RoboRuckusSensor::LEFT) {
							done = std::get<1>(result) >= (RoboRuckusMovement::move_config.turnDistance * currentMagnitude);
						}
					}
				}
			}
			break;
	}
	return done;
}

/// @brief Resets the wheels and sensor
void RuckusServoWheels::resetMove() {
	front_right.write(wheel_config.frontRightZero);
	front_left.write(wheel_config.frontLeftZero);
	if (four_wheels) {
		rear_right.write(wheel_config.rearRightZero);
		rear_left.write(wheel_config.rearLeftZero);
	}
	if (navSensor != nullptr) {
		navSensor->endMove();
	};
}