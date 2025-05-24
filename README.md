# Otto Robot - DPRG Quick Trip Competition

Otto bipedal robot programmed for the Dallas Personal Robotics Group (DPRG) Quick Trip competition.

## Competition Overview
**Quick Trip Challenge**: Robot must walk straight for 10 feet (120 inches) as accurately as possible.
- **Distance**: 120 inches (10 feet)
- **Scoring**: Based on distance traveled and deviation from straight line
- **Competition Type**: Non-wheeled robots
- **Rules**: [DPRG Roborama Quick Trip Rules](https://dprg.org)

## Hardware
- Otto Robot Kit (bipedal walking robot)
- MPU6050 Gyroscope/Accelerometer for heading correction
- Arduino Nano compatible microcontroller
- 4 servo motors for leg movement

## Current Status - 2025 Competition
- **Development Phase**: Active development and testing
- **Latest Version**: Rev-U Enhanced Debug with comprehensive diagnostics
- **Testing Status**: Square walk validation and servo calibration completed
- **Competition Date**: TBD

## Repository Organization
- **2025/**: Current year development and competition entry
- **Shared-Code/**: Libraries and utilities used across programs
- **Project-Documentation/**: Repository organization and development workflow

## Code Versions
- **Rev-U Enhanced**: Latest with detailed debug output and serial plotter
- **Rev-T**: Previous stable version
- **Rev-S**: Backup version
- **Square Walk Test**: Mechanical validation program
- **Servo Calibration**: Servo tuning utility

## Quick Start
1. Clone this repository
2. Open Arduino IDE
3. Navigate to `2025/Development/Rev-U/` for latest code
4. Upload to Otto robot and follow calibration instructions

## Competition Strategy
1. **Gyroscope Navigation**: Use MPU6050 for heading maintenance
2. **Calibrated Steps**: Precise step length measurement (1.5" per step)
3. **Real-time Correction**: Continuous heading adjustment
4. **Mechanical Validation**: Square walk test for servo verification

## Competition Results
### 2025 Results
- **Status**: In development
- **Practice Runs**: TBD
- **Final Competition**: TBD

## Development Workflow
1. **Local Development**: Work in regular Arduino directory
2. **Version Control**: Copy stable versions to this repository
3. **Documentation**: Update hardware configs and test results
4. **Photos/Videos**: Upload to Google Drive and link in documentation

## Links
- **Hardware Documentation**: See `2025/Hardware/`
- **Test Results**: See `2025/Documentation/`
- **DPRG Website**: https://dprg.org

## Latest Changes
- Enhanced gyroscope integration with detailed debugging
- Added mechanical testing suite for servo validation
- Improved calibration process for competition reliability
- Organized repository for multi-year development
- Added serial plotter output for real-time visualization
