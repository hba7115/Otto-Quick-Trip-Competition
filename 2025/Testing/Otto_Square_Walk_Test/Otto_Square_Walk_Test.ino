/*
*   Otto Square Walk Test - Mechanical Component Testing
*   Tests servo functionality by walking a 24-inch square
*   MPU6050 records data only - no corrections applied
*   Serial output shows position, heading, and timing data
*   Serial plotter shows position vs time
*/

#include <Wire.h>
#include <Otto.h>

Otto Otto;

// MPU6050 I2C address and registers
#define MPU6050_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define GYRO_ZOUT_H 0x47

// Pin definitions
#define BUZZER 13
#define LEFT_LEG 2
#define RIGHT_LEG 3
#define LEFT_FOOT 4
#define RIGHT_FOOT 5

// Square walk parameters
const float SQUARE_SIDE_LENGTH = 24.0; // inches
const float STEP_LENGTH = 1.5; // inches per step
const int STEPS_PER_SIDE = SQUARE_SIDE_LENGTH / STEP_LENGTH; // 16 steps per side
const int TOTAL_SIDES = 4;
const int TOTAL_STEPS = STEPS_PER_SIDE * TOTAL_SIDES;

// Movement parameters
const int WALK_SPEED = 700; // ms per step
const int TURN_SPEED = 1000; // ms per turn step
const int TURN_STEPS = 4; // Number of turn steps for 90 degrees

// MPU6050 variables (recording only)
float gyroZ_offset = 0;
float recorded_heading = 0;
unsigned long last_gyro_time = 0;

// Position tracking
float position_x = 0; // inches
float position_y = 0; // inches
float target_heading = 0; // degrees (0=North, 90=East, 180=South, 270=West)
int current_side = 0; // 0-3 for North, East, South, West
int steps_on_current_side = 0;
int total_step_count = 0;

// Timing
unsigned long test_start_time = 0;
unsigned long side_start_time = 0;

// State machine
enum TestState {
  WALKING_STRAIGHT,
  TURNING_90_DEGREES,
  TEST_COMPLETE
};
TestState current_state = WALKING_STRAIGHT;
int turn_step_count = 0;

// Function declarations
void setupMPU6050();
void calibrateGyro();
void updateHeadingRecording();
void walkSquare();
void executeTurn();
void updatePosition();
void printStatus();
void printPlotterData();
float readGyroZ();
float readGyroZ_Raw();

void setup() {
  Serial.begin(9600);
  
  Serial.println(F("=== OTTO SQUARE WALK TEST ==="));
  Serial.println(F("Mechanical Component Testing"));
  Serial.println(F("MPU6050 Recording Only - No Corrections"));
  Serial.println();
  
  // Initialize Otto
  Otto.init(LEFT_LEG, RIGHT_LEG, LEFT_FOOT, RIGHT_FOOT, true, BUZZER);
  Serial.println(F("✓ Otto servos initialized"));
  
  // Initialize MPU6050
  setupMPU6050();
  
  // Test parameters
  Serial.println(F("=== TEST PARAMETERS ==="));
  Serial.print(F("Square size: ")); Serial.print(SQUARE_SIDE_LENGTH); Serial.println(F(" inches"));
  Serial.print(F("Steps per side: ")); Serial.println(STEPS_PER_SIDE);
  Serial.print(F("Total steps: ")); Serial.println(TOTAL_STEPS);
  Serial.print(F("Step length: ")); Serial.print(STEP_LENGTH); Serial.println(F(" inches"));
  Serial.print(F("Walk speed: ")); Serial.print(WALK_SPEED); Serial.println(F(" ms"));
  Serial.print(F("Turn speed: ")); Serial.print(TURN_SPEED); Serial.println(F(" ms"));
  Serial.println();
  
  // Calibrate gyroscope
  calibrateGyro();
  
  // Ready to start
  Otto.home();
  Otto.sing(S_happy);
  Serial.println(F("=== STARTING SQUARE WALK TEST ==="));
  Serial.println(F("Otto will walk a 24-inch square"));
  Serial.println(F("Serial Plotter Format: X_Position,Y_Position,Heading,Target_Heading,Side_Number"));
  Serial.println();
  
  test_start_time = millis();
  side_start_time = millis();
  last_gyro_time = millis();
}

void loop() {
  // Always update gyroscope recording
  updateHeadingRecording();
  
  // Execute square walk state machine
  walkSquare();
  
  // Print status every 5 steps
  if (total_step_count % 5 == 0) {
    printStatus();
  }
  
  // Always output plotter data
  printPlotterData();
  
  delay(50);
}

void setupMPU6050() {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0); // Wake up MPU6050
  Wire.endTransmission(true);
  delay(100);
  Serial.println(F("✓ MPU6050 initialized"));
}

void calibrateGyro() {
  Serial.println(F("=== GYROSCOPE CALIBRATION ==="));
  Serial.println(F("Keep Otto still for calibration..."));
  delay(2000);
  
  float sum = 0;
  int samples = 1000;
  
  for (int i = 0; i < samples; i++) {
    sum += readGyroZ_Raw();
    delay(3);
    
    if (i % 100 == 0) {
      Serial.print(F("Progress: ")); 
      Serial.print(i * 100 / samples); 
      Serial.println(F("%"));
    }
  }
  
  gyroZ_offset = sum / samples;
  Serial.print(F("Gyro offset: ")); Serial.println(gyroZ_offset, 4);
  Serial.println(F("Calibration complete!"));
  Serial.println();
}

void walkSquare() {
  switch (current_state) {
    case WALKING_STRAIGHT:
      if (steps_on_current_side < STEPS_PER_SIDE) {
        // Take a step forward
        Otto.walk(1, WALK_SPEED, FORWARD);
        steps_on_current_side++;
        total_step_count++;
        updatePosition();
        
        Serial.print(F("WALK: Side ")); Serial.print(current_side + 1);
        Serial.print(F(", Step ")); Serial.print(steps_on_current_side);
        Serial.print(F("/")); Serial.print(STEPS_PER_SIDE);
        Serial.print(F(" | Pos: (")); Serial.print(position_x, 1);
        Serial.print(F(", ")); Serial.print(position_y, 1); Serial.println(F(")"));
        
      } else {
        // Finished walking this side, start turning
        if (current_side < 3) { // Don't turn after the last side
          current_state = TURNING_90_DEGREES;
          turn_step_count = 0;
          Serial.println(F("--- STARTING 90° TURN ---"));
        } else {
          current_state = TEST_COMPLETE;
        }
      }
      break;
      
    case TURNING_90_DEGREES:
      if (turn_step_count < TURN_STEPS) {
        // Execute turn step
        Otto.turn(1, TURN_SPEED, RIGHT); // Test RIGHT turns
        turn_step_count++;
        
        Serial.print(F("TURN: Step ")); Serial.print(turn_step_count);
        Serial.print(F("/")); Serial.print(TURN_STEPS);
        Serial.print(F(" | Target heading: ")); Serial.println(target_heading + 90);
        
      } else {
        // Finished turning, prepare for next side
        current_side++;
        target_heading += 90;
        if (target_heading >= 360) target_heading -= 360;
        
        steps_on_current_side = 0;
        current_state = WALKING_STRAIGHT;
        side_start_time = millis();
        
        Serial.print(F("--- TURN COMPLETE - NOW FACING "));
        switch (current_side) {
          case 1: Serial.println(F("EAST")); break;
          case 2: Serial.println(F("SOUTH")); break;
          case 3: Serial.println(F("WEST")); break;
          default: Serial.println(F("NORTH")); break;
        }
        Serial.println();
      }
      break;
      
    case TEST_COMPLETE:
      Otto.home();
      Otto.sing(S_superHappy);
      delay(500);
      Otto.sing(S_happy);
      
      unsigned long total_time = millis() - test_start_time;
      Serial.println(F("=== SQUARE WALK TEST COMPLETE ==="));
      Serial.print(F("Total time: ")); Serial.print(total_time / 1000.0, 1); Serial.println(F(" seconds"));
      Serial.print(F("Final position: (")); Serial.print(position_x, 2);
      Serial.print(F(", ")); Serial.print(position_y, 2); Serial.println(F(") inches"));
      Serial.print(F("Final recorded heading: ")); Serial.print(recorded_heading, 1); Serial.println(F("°"));
      Serial.print(F("Expected final heading: ")); Serial.print(target_heading, 1); Serial.println(F("°"));
      
      // Calculate position error
      float position_error = sqrt(position_x * position_x + position_y * position_y);
      Serial.print(F("Position error from start: ")); Serial.print(position_error, 2); Serial.println(F(" inches"));
      
      // Calculate heading error  
      float heading_error = target_heading - recorded_heading;
      if (heading_error > 180) heading_error -= 360;
      if (heading_error < -180) heading_error += 360;
      Serial.print(F("Heading error: ")); Serial.print(heading_error, 1); Serial.println(F("°"));
      
      while (true) {
        delay(1000); // Stop here
      }
      break;
  }
}

void updatePosition() {
  // Calculate position based on current heading and step
  float heading_rad = target_heading * PI / 180.0;
  position_x += STEP_LENGTH * sin(heading_rad);
  position_y += STEP_LENGTH * cos(heading_rad);
}

void updateHeadingRecording() {
  unsigned long current_time = millis();
  float dt = (current_time - last_gyro_time) / 1000.0;
  
  if (dt > 0.02) { // 50Hz max
    float gyroZ = readGyroZ();
    
    // Simple integration for recording
    static float filtered_gyroZ = 0;
    filtered_gyroZ = 0.8 * filtered_gyroZ + 0.2 * gyroZ;
    recorded_heading += filtered_gyroZ * dt;
    
    // Keep in range
    if (recorded_heading > 180) recorded_heading -= 360;
    if (recorded_heading < -180) recorded_heading += 360;
    
    last_gyro_time = current_time;
  }
}

void printStatus() {
  Serial.print(F("STATUS: Total steps=")); Serial.print(total_step_count);
  Serial.print(F(" | Side=")); Serial.print(current_side + 1);
  Serial.print(F(" | Recorded heading=")); Serial.print(recorded_heading, 1);
  Serial.print(F("° | Target=")); Serial.print(target_heading, 1);
  Serial.print(F("° | Time=")); Serial.print((millis() - test_start_time) / 1000.0, 1);
  Serial.println(F("s"));
}

void printPlotterData() {
  // Format for Serial Plotter: X,Y,RecordedHeading,TargetHeading,SideNumber
  Serial.print(position_x, 2); Serial.print(F(","));
  Serial.print(position_y, 2); Serial.print(F(","));
  Serial.print(recorded_heading, 1); Serial.print(F(","));
  Serial.print(target_heading, 1); Serial.print(F(","));
  Serial.println(current_side);
}

float readGyroZ() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(GYRO_ZOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 2, true);
  
  if (Wire.available() < 2) return 0.0;
  
  int16_t gyroZ_raw = Wire.read() << 8 | Wire.read();
  return -((gyroZ_raw / 131.0) - gyroZ_offset);
}

float readGyroZ_Raw() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(GYRO_ZOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 2, true);
  
  if (Wire.available() < 2) return 0.0;
  
  int16_t gyroZ_raw = Wire.read() << 8 | Wire.read();
  return (gyroZ_raw / 131.0);
}
