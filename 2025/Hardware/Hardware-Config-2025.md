# Otto Hardware Configuration - 2025 Quick Trip

## Base Hardware
- **Robot Kit**: Otto DIY Robot Kit
- **Microcontroller**: Arduino Nano compatible board
- **Power Supply**: 4x AA batteries (6V total)
- **Servos**: 4x SG90 micro servos (9g)
- **Buzzer**: Passive buzzer for audio feedback

## Added Components for Competition
- **Gyroscope/Accelerometer**: MPU6050 6-axis sensor module
- **Connection Method**: I2C communication protocol
- **Sensor Power**: 3.3V from Arduino
- **Purpose**: Heading detection and straight-line navigation

## Pin Assignments
### Servo Connections
- **Left Leg (Hip)**: Digital Pin 2
- **Right Leg (Hip)**: Digital Pin 3  
- **Left Foot (Ankle)**: Digital Pin 4
- **Right Foot (Ankle)**: Digital Pin 5

### Audio Output
- **Buzzer**: Digital Pin 13

### MPU6050 I2C Connections
- **VCC**: Arduino 3.3V pin
- **GND**: Arduino GND pin
- **SDA**: Arduino A4 (I2C Data)
- **SCL**: Arduino A5 (I2C Clock)
- **INT**: Not connected (interrupt not used)
- **AD0**: Not connected (default I2C address)

## Servo Calibration Settings (2025)
**Note**: These values determined through testing and may need adjustment
- **Left Leg Trim**: TBD (to be measured)
- **Right Leg Trim**: TBD (to be measured)
- **Left Foot Trim**: TBD (to be measured)  
- **Right Foot Trim**: TBD (to be measured)

**Calibration Tool**: Use `Otto_Servo_CalibrationV4` program for precise adjustment

## Physical Modifications
- **Status**: No modifications to stock Otto kit
- **Mounting**: MPU6050 sensor securely attached to Otto's body
- **Wiring**: Clean routing to prevent interference with movement
- **Battery Access**: Easy access for competition day battery changes

## Known Hardware Issues (2025)
- **Servo Direction**: LEFT/RIGHT commands may be swapped - needs verification
- **Walking Drift**: Slight tendency to drift left during straight walking
- **Gyroscope Sensitivity**: Requires careful calibration on competition surface
- **Power Stability**: Monitor battery voltage during long test runs

## MPU6050 Configuration
- **I2C Address**: 0x68 (default)
- **Gyroscope Range**: ±250°/s (default)
- **Accelerometer Range**: ±2g (default, not currently used)
- **Sample Rate**: 50Hz (20ms updates)
- **Calibration**: Required before each use - 2000 samples while stationary

## Competition Day Hardware Checklist
- [ ] Fresh AA batteries installed and tested
- [ ] All servo connections secure
- [ ] MPU6050 sensor firmly mounted
- [ ] No loose wires or connections
- [ ] Buzzer working for audio feedback
- [ ] Servo calibration verified
- [ ] Gyroscope calibration completed on competition surface

## Test Results Log
### Square Walk Test Results
- **Date**: TBD
- **Performance**: TBD
- **Issues Found**: TBD
- **Corrections Made**: TBD

### Straight Line Test Results  
- **Date**: TBD
- **Distance Accuracy**: TBD
- **Deviation**: TBD
- **Notes**: TBD

## Hardware Evolution Notes
### Changes from Initial Setup
- Added MPU6050 gyroscope for navigation
- Enhanced wiring for sensor integration
- No mechanical modifications to Otto frame

### Planned Improvements for 2026
- Consider servo position feedback
- Evaluate alternative gyroscope mounting
- Investigate power supply improvements

## Photos and Documentation
- **Wiring Photos**: See `Photos/Wiring-Photos/` folder
- **Assembly Photos**: See `Photos/Build-Progress/` folder  
- **Google Drive**: [Link to be added after photo upload]

## Parts List and Sources
See `Parts-List-2025.md` for complete component list and suppliers

## Troubleshooting Guide
### Common Issues
1. **Gyroscope not responding**: Check I2C connections (SDA/SCL)
2. **Servo jitter**: Verify power supply voltage (should be ~6V)
3. **Walking instability**: Check servo calibration and trim values
4. **Code upload fails**: Verify Arduino board selection and COM port

### Debug Tools
- **Serial Monitor**: Real-time sensor and servo data
- **Serial Plotter**: Visual representation of gyroscope readings
- **Multimeter**: Voltage and continuity checking
