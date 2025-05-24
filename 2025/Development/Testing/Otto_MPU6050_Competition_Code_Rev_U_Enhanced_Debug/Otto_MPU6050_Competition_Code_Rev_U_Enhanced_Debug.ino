/*
*   Otto_MPU6050_Competition_Code_Rev_U_Enhanced_Debug
    Otto_MPU6050_Competition_Code_Rev_U - Enhanced Debug + Serial Plotter
    Memory optimized with useful debug info and real-time visualization
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

// Competition parameters
const float TARGET_DISTANCE = 120.0;
const float STEP_LENGTH = 1.5;
const int TOTAL_STEPS = TARGET_DISTANCE / STEP_LENGTH;

// Speed parameters
const int MIN_SPEED = 800;
const int MAX_SPEED = 600;
const int ACCEL_STEPS = 15;
const int DECEL_STEPS = 15;
const int CORRECTION_DELAY = 1200;

// MPU6050 variables
float gyroZ_offset = 0;
float heading = 0;
float target_heading = 0;
unsigned long last_gyro_time = 0;

// Movement variables
int current_step = 0;
int current_speed = MIN_SPEED;
bool walking = false;

// Performance tracking
float max_heading_error = 0;
int correction_count = 0;
unsigned long step_start_time = 0;

// Function declarations
void preCompetitionSequence();
void startSequence();
void calibrateGyro();
void updateHeading();
void takeStepWithCorrection();
int calculateSpeed();
float readGyroZ();
float readGyroZ_Raw();
void printPlotterData();

void setup() {
  Serial.begin(9600);
  
  Otto.init(LEFT_LEG, RIGHT_LEG, LEFT_FOOT, RIGHT_FOOT, true, BUZZER);
  
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);
  
  delay(100);
  
  Serial.println(F("=== OTTO COMPETITION REV U ==="));
  Serial.print(F("Distance: ")); Serial.print(TARGET_DISTANCE);
  Serial.print(F("in, Steps: ")); Serial.print(TOTAL_STEPS);
  Serial.print(F(", Step size: ")); Serial.print(STEP_LENGTH); Serial.println(F("in"));
  Serial.print(F("Speed: ")); Serial.print(MAX_SPEED); Serial.print(F("-")); Serial.print(MIN_SPEED);
  Serial.print(F("ms, Correction: ")); Serial.print(CORRECTION_DELAY); Serial.println(F("ms"));
  Serial.println();
  
  preCompetitionSequence();
  calibrateGyro();
  startSequence();
}

void loop() {
  if (walking && current_step < TOTAL_STEPS) {
    step_start_time = millis();
    updateHeading();
    current_speed = calculateSpeed();
    takeStepWithCorrection();
    current_step++;
    
    // Progress with performance stats
    if (current_step % 10 == 0) {
      float distance_traveled = current_step * STEP_LENGTH;
      float progress_percent = (float)current_step * 100.0 / TOTAL_STEPS;
      
      Serial.print(F("=== PROGRESS STEP ")); Serial.print(current_step); Serial.println(F(" ==="));
      Serial.print(F("Distance: ")); Serial.print(distance_traveled, 1); 
      Serial.print(F("in (")); Serial.print(progress_percent, 1); Serial.println(F("%)"));
      Serial.print(F("Corrections: ")); Serial.print(correction_count);
      Serial.print(F(", Max error: ")); Serial.print(max_heading_error, 2); Serial.println(F("°"));
      Serial.print(F("Current heading: ")); Serial.print(heading, 2); 
      Serial.print(F("°, Speed: ")); Serial.print(current_speed); Serial.println(F("ms"));
      Serial.println();
    }
    
  } else if (current_step >= TOTAL_STEPS) {
    Otto.home();
    Otto.sing(S_superHappy);
    delay(500);
    Otto.sing(S_happy);
    
    Serial.println(F("=== JOURNEY COMPLETE! ==="));
    Serial.print(F("Final heading: ")); Serial.print(heading, 2); Serial.println(F("°"));
    Serial.print(F("Total corrections: ")); Serial.println(correction_count);
    Serial.print(F("Max heading error: ")); Serial.println(max_heading_error, 2);
    float accuracy = 100.0 * (TOTAL_STEPS - correction_count) / TOTAL_STEPS;
    Serial.print(F("Accuracy: ")); Serial.print(accuracy, 1); Serial.println(F("%"));
    walking = false;
  }
  
  delay(50);
}

void startSequence() {
  Otto.sing(S_buttonPushed);
  delay(1000);
  
  Serial.println(F("Starting countdown..."));
  for (int i = 3; i > 0; i--) {
    Serial.print(F("Starting in ")); Serial.println(i);
    Otto.sing(S_happy_short);
    delay(800);
  }
  
  heading = 0;
  target_heading = 0;
  current_step = 0;
  walking = true;
  last_gyro_time = millis();
  max_heading_error = 0;
  correction_count = 0;
  
  Otto.sing(S_superHappy);
  Serial.println(F("=== WALKING STARTED ==="));
  Serial.println(F("Serial Plotter Data Format:"));
  Serial.println(F("RawGyro FilteredGyro Heading TargetHeading HeadingError CorrectionFlag"));
  Serial.println();
}

void preCompetitionSequence() {
  Serial.println(F("Pre-competition sequence..."));
  Otto.home();
  delay(500);
  Otto.sing(S_happy);
  delay(300);
  Otto.sing(S_superHappy);
  delay(500);
  Serial.println(F("Ready for calibration!"));
}

void calibrateGyro() {
  float sum = 0;
  int samples = 1500;
  
  Serial.println(F("=== GYROSCOPE CALIBRATION ==="));
  Serial.println(F("CRITICAL: Keep Otto completely still!"));
  Serial.println(F("Measuring sensor bias..."));
  
  // Countdown with user instructions
  for (int i = 3; i > 0; i--) {
    Serial.print(F("Calibrating in ")); Serial.print(i); Serial.println(F(" seconds - DO NOT MOVE OTTO"));
    delay(1000);
  }
  
  Serial.println(F("Calibration in progress..."));
  float min_reading = 1000, max_reading = -1000;
  
  for (int i = 0; i < samples; i++) {
    float reading = readGyroZ_Raw();
    sum += reading;
    if (reading < min_reading) min_reading = reading;
    if (reading > max_reading) max_reading = reading;
    delay(3);
    
    if (i % 200 == 0) {
      Serial.print(F("Progress: ")); Serial.print(i * 100 / samples);
      Serial.print(F("% | Reading: ")); Serial.println(reading, 4);
    }
  }
  
  gyroZ_offset = sum / samples;
  float range = max_reading - min_reading;
  
  Serial.println(F("=== CALIBRATION RESULTS ==="));
  Serial.print(F("Calculated offset: ")); Serial.print(gyroZ_offset, 6); Serial.println(F(" deg/s"));
  Serial.print(F("Reading range: ")); Serial.print(range, 4); Serial.println(F(" deg/s"));
  
  // Quality assessment
  Serial.print(F("Calibration quality: "));
  if (range < 0.5) {
    Serial.println(F("EXCELLENT - Very stable sensor"));
  } else if (range < 1.5) {
    Serial.println(F("GOOD - Acceptable for walking"));
  } else if (range < 3.0) {
    Serial.println(F("FAIR - May have some drift"));
  } else {
    Serial.println(F("POOR - High noise/vibration detected"));
  }
  
  // Verification test
  Serial.println(F("Verification test (should be near 0.000):"));
  float test_sum = 0;
  for (int i = 0; i < 6; i++) {
    float test = readGyroZ();
    test_sum += test;
    Serial.print(F("Test ")); Serial.print(i+1); Serial.print(F(": ")); 
    Serial.print(test, 4); Serial.println(F(" deg/s"));
    delay(250);
  }
  
  float avg_error = test_sum / 6;
  Serial.print(F("Average error: ")); Serial.print(avg_error, 4); Serial.println(F(" deg/s"));
  Serial.println(F("Calibration complete!"));
  Serial.println();
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

void updateHeading() {
  unsigned long current_time = millis();
  float dt = (current_time - last_gyro_time) / 1000.0;
  
  if (dt > 0.02) {
    float gyroZ = readGyroZ();
    
    static float filtered_gyroZ = 0;
    filtered_gyroZ = 0.7 * filtered_gyroZ + 0.3 * gyroZ;
    
    float previous_heading = heading;
    float integration_amount = filtered_gyroZ * dt;
    
    // CRITICAL FIX: Always integrate (no deadband)
    heading += integration_amount;
    
    // Keep heading in reasonable range
    if (heading > 180) heading -= 360;
    if (heading < -180) heading += 360;
    
    last_gyro_time = current_time;
    
    // Update performance tracking
    float abs_heading = abs(heading);
    if (abs_heading > max_heading_error) {
      max_heading_error = abs_heading;
    }
    
    // Detailed debug every 5 readings
    static int debug_count = 0;
    debug_count++;
    if (debug_count >= 5) {
      Serial.print(F("GYRO: Raw=")); Serial.print(gyroZ, 2);
      Serial.print(F(" Filt=")); Serial.print(filtered_gyroZ, 2);
      Serial.print(F(" dt=")); Serial.print(dt, 3);
      Serial.print(F(" integ=")); Serial.print(integration_amount, 4);
      Serial.print(F(" H=")); Serial.print(heading, 2);
      Serial.print(F(" chg=")); Serial.print(heading - previous_heading, 3);
      
      // Movement analysis
      if (abs(gyroZ) > 3.0) {
        Serial.print(gyroZ > 0 ? F(" STRONG_R") : F(" STRONG_L"));
      } else if (abs(gyroZ) > 1.0) {
        Serial.print(gyroZ > 0 ? F(" mod_r") : F(" mod_l"));
      } else if (abs(gyroZ) > 0.2) {
        Serial.print(gyroZ > 0 ? F(" weak_r") : F(" weak_l"));
      } else {
        Serial.print(F(" still"));
      }
      
      Serial.println();
      debug_count = 0;
    }
    
    // Serial plotter output every update for smooth visualization
    printPlotterData();
  }
}

void printPlotterData() {
  static unsigned long last_plot_time = 0;
  unsigned long current_time = millis();
  
  // Output plotter data every 100ms for smooth visualization
  if (current_time - last_plot_time > 100) {
    float gyroZ = readGyroZ();
    static float filtered_gyroZ = 0;
    filtered_gyroZ = 0.7 * filtered_gyroZ + 0.3 * gyroZ;
    
    float heading_error = target_heading - heading;
    int correction_flag = (abs(heading_error) > 2.5) ? 10 : 0; // Visual flag for corrections
    
    // Format for Arduino Serial Plotter: value1,value2,value3
    Serial.print(gyroZ, 2); Serial.print(F(","));
    Serial.print(filtered_gyroZ, 2); Serial.print(F(","));
    Serial.print(heading, 2); Serial.print(F(","));
    Serial.print(target_heading, 1); Serial.print(F(","));
    Serial.print(heading_error, 2); Serial.print(F(","));
    Serial.println(correction_flag);
    
    last_plot_time = current_time;
  }
}

int calculateSpeed() {
  int speed;
  
  if (current_step < ACCEL_STEPS) {
    float progress = (float)current_step / ACCEL_STEPS;
    speed = MIN_SPEED - (MIN_SPEED - MAX_SPEED) * progress;
  } else if (current_step > TOTAL_STEPS - DECEL_STEPS) {
    int decel_step = TOTAL_STEPS - current_step;
    float progress = (float)decel_step / DECEL_STEPS;
    speed = MIN_SPEED - (MIN_SPEED - MAX_SPEED) * progress;
  } else {
    speed = MAX_SPEED;
  }
  
  return speed;
}

void takeStepWithCorrection() {
  float heading_error = target_heading - heading;
  
  Serial.print(F("STEP ")); Serial.print(current_step);
  Serial.print(F(": H=")); Serial.print(heading, 2);
  Serial.print(F("° E=")); Serial.print(heading_error, 2);
  Serial.print(F("° Spd=")); Serial.print(current_speed);
  Serial.print(F("ms -> "));
  
  if (abs(heading_error) < 2.5) {
    Serial.println(F("FORWARD"));
    Otto.walk(1, current_speed, FORWARD);
  } else {
    correction_count++;
    
    // REMOVED SWAPPING - using normal LEFT/RIGHT logic
    if (heading > target_heading + 2.5) {
      // Otto drifted RIGHT, correct by turning LEFT
      Serial.print(F("DRIFT_RIGHT: correct with Otto.turn(LEFT) H="));
      Serial.println(heading, 2);
      Otto.turn(1, CORRECTION_DELAY, LEFT); // NORMAL: was RIGHT
      delay(150);
      Otto.walk(1, current_speed, FORWARD);
      
    } else if (heading < target_heading - 2.5) {
      // Otto drifted LEFT, correct by turning RIGHT
      Serial.print(F("DRIFT_LEFT: correct with Otto.turn(RIGHT) H="));
      Serial.println(heading, 2);
      Otto.turn(1, CORRECTION_DELAY, RIGHT); // NORMAL: was LEFT
      delay(150);
      Otto.walk(1, current_speed, FORWARD);
    }
  }
  
  unsigned long step_time = millis() - step_start_time;
  Serial.print(F("  Step took: ")); Serial.print(step_time); Serial.println(F("ms"));
  
  delay(100);
}

void takeStepWithAsymmetricCorrection() {
  float heading_error = target_heading - heading;
  heading_error = constrain(heading_error, -10, 10);
  
  if (abs(heading_error) < 1.0) {
    Otto.walk(1, current_speed, FORWARD);
  } else {
    int left_delay = current_speed;
    int right_delay = current_speed;
    
    if (heading_error > 0) {
      left_delay += (int)(heading_error * 10);
    } else {
      right_delay += (int)(abs(heading_error) * 10);
    }
    
    customStep(left_delay, right_delay);
  }
}

void customStep(int left_delay, int right_delay) {
  Otto.walk(1, (left_delay + right_delay) / 2, FORWARD);
}