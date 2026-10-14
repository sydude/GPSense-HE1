//T56_Gear_Indicator_v1.6.ino

int current_gear = 0;  // 0 = N, 1-6 = gears 1-6; reverse handled separately
const int ADC_MAX = 4095;
const float HALL_MIN_VOLTS = 0.6;
const float HALL_MAX_VOLTS = 4.8;
const int HALL_MIN_VALUE = ADC_MAX * (HALL_MIN_VOLTS / 5.0);
const int HALL_MAX_VALUE = ADC_MAX * (HALL_MAX_VOLTS / 5.0);
const int HALL_CENTER_VALUE = 2055;  // Measured approximate DRV5055 quiescent ADC value
const int gearOut[7] = { 0, 655, 1309, 1964, 2619, 3273, 3928 };
// Position	Nominal
// Neutral=0.00v / 1st=0.82v / 2nd=1.63v / 3rd=2.45v / 4th=3.27v / 5th=4.08v / 6th=4.90v
// Voltage Tolerance = 0.10 V for all positions.

const int GEAR_1_BIAS = 400;
const int GEAR_2_BIAS = 550;
const int GEAR_6_BIAS = 150;

const unsigned long SENSOR_INTERVAL_MS = 5;  // How often to evaluate sensor readings
const int REQUIRED_STABLE_READS = 10;  // Candidate must remain unchanged this many evaluations
const int ROW_ENTER_THRESHOLD = 200;  // Raw ADC-count difference required to leave Neutral
const int ROW_EXIT_THRESHOLD = 120;   // Raw ADC-count difference required to stay out of Neutral
const bool TEST_MODE = true;  // true = enable Serial bench-test output
const unsigned long SERIAL_INTERVAL_MS = 100;  // Serial output interval
void setup() {
  // A0 is DAC out; A1 is currently unassigned and available
  pinMode(A2, INPUT); // Gear 5 sensor
  pinMode(A3, INPUT); // Gear 3 sensor
  pinMode(A4, INPUT); // Gear 1 sensor
  pinMode(A5, INPUT); // Gear 6 sensor
  pinMode(A6, INPUT); // Gear 4 sensor
  pinMode(A7, INPUT); // Gear 2 sensor
  analogReadResolution(12);
  analogWriteResolution(12); // 12-bit DAC via DAC0 on Nano R4 pin A0
  if (TEST_MODE) {
    Serial.begin(115200);
    Serial.println("T56 Gear Sensor Bench Test");
  }
}
void loop() {
  int hall_1 = analogRead(A4); // Gear 1
  int hall_3 = analogRead(A3); // Gear 3
  int hall_5 = analogRead(A2); // Gear 5
  int hall_2 = analogRead(A7); // Gear 2
  int hall_4 = analogRead(A6); // Gear 4
  int hall_6 = analogRead(A5); // Gear 6
  int sensor1 = constrain(map(hall_1, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  int sensor3 = constrain(map(hall_3, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  int sensor5 = constrain(map(hall_5, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  int sensor2 = constrain(map(hall_2, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  int sensor4 = constrain(map(hall_4, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  int sensor6 = constrain(map(hall_6, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100), 0, 100);
  // Hall sensors are arranged like this. Reverse is handled separately in hardware.
  //
  //      A4    A3    A2
  //      1     3     5     R
  //      │     │     │     │
  //      ├─────┼─────│─────┘
  //      │     │     │
  //      2     4     6
  //      A7    A6    A5
  static unsigned long last_sensor_evaluation = 0;
  static unsigned long last_serial_output = 0;
  static int last_candidate_gear = 0;
  static int stable_read_count = 0;
  if (millis() - last_sensor_evaluation >= SENSOR_INTERVAL_MS) {
    last_sensor_evaluation += SENSOR_INTERVAL_MS;
    // Top-row magnet polarity causes ADC values to rise above quiescent.
    // Bottom-row magnet polarity causes ADC values to fall below quiescent.
    int top_row_raw_avg = (hall_1 + hall_3 + hall_5) / 3;
    int bottom_row_raw_avg = (hall_2 + hall_4 + hall_6) / 3;
    int top_row_activity = top_row_raw_avg - HALL_CENTER_VALUE;
    int bottom_row_activity = HALL_CENTER_VALUE - bottom_row_raw_avg;
    // Positive = top-row activity is stronger.
    // Negative = bottom-row activity is stronger.
    // Near zero = Neutral.
    int row_difference = top_row_activity - bottom_row_activity;
    int active_row_threshold;
    if (current_gear == 0) {
      active_row_threshold = ROW_ENTER_THRESHOLD;
    }
    else {
      active_row_threshold = ROW_EXIT_THRESHOLD;
    }
    int candidate_gear = 0;
    // NEUTRAL
    if (abs(row_difference) < active_row_threshold) {
      candidate_gear = 0;
    }
    // TOP ROW - GEARS 1, 3, 5
    // Stronger magnetic response means a HIGHER ADC reading.
    else if (row_difference > 0) {
      if ((hall_1 + GEAR_1_BIAS) >= hall_3 &&
          (hall_1 + GEAR_1_BIAS) >= hall_5) {
        candidate_gear = 1;
      }
      else if (hall_3 >= hall_1 &&
              hall_3 >= hall_5) {
        candidate_gear = 3;
      }
      else {
        candidate_gear = 5;
      }
    }
    // BOTTOM ROW - GEARS 2, 4, 6
        // Stronger magnetic response means a LOWER ADC reading.
// BOTTOM ROW - GEARS 2, 4, 6
// Stronger magnetic response means a LOWER ADC reading.
    else {
      if ((hall_2 - GEAR_2_BIAS) <= hall_4 &&
          (hall_2 - GEAR_2_BIAS) <= (hall_6 - GEAR_6_BIAS)) {
        candidate_gear = 2;
      }
      else if ((hall_6 - GEAR_6_BIAS) <= hall_2 &&
              (hall_6 - GEAR_6_BIAS) <= hall_4) {
        candidate_gear = 6;
      }
      else {
        candidate_gear = 4;
      }
    }
    // Candidate must remain stable before becoming current_gear.
    if (candidate_gear == last_candidate_gear) {
      if (stable_read_count < REQUIRED_STABLE_READS) {
        stable_read_count++;
      }
      if (stable_read_count >= REQUIRED_STABLE_READS) {
        current_gear = candidate_gear;
      }
    }
    else {
      last_candidate_gear = candidate_gear;
      stable_read_count = 1;
    }
    // Bench-test Serial output.
    if (TEST_MODE && millis() - last_serial_output >= SERIAL_INTERVAL_MS) {
      last_serial_output += SERIAL_INTERVAL_MS;
      Serial.print("Gear:");
      if (current_gear == 0) {
        Serial.print("N");
      }
      else {
        Serial.print(current_gear);
      }
      Serial.print(" Candidate:");
      if (candidate_gear == 0) {
        Serial.print("N");
      }
      else {
        Serial.print(candidate_gear);
      }
      Serial.print(" Stable:");
      Serial.print(stable_read_count);
      Serial.print("/");
      Serial.print(REQUIRED_STABLE_READS);
      Serial.print(" RAW 1:");
      Serial.print(hall_1);
      Serial.print(" 2:");
      Serial.print(hall_2);
      Serial.print(" 3:");
      Serial.print(hall_3);
      Serial.print(" 4:");
      Serial.print(hall_4);
      Serial.print(" 5:");
      Serial.print(hall_5);
      Serial.print(" 6:");
      Serial.print(hall_6);
      Serial.print(" % 1:");
      Serial.print(sensor1);
      Serial.print(" 2:");
      Serial.print(sensor2);
      Serial.print(" 3:");
      Serial.print(sensor3);
      Serial.print(" 4:");
      Serial.print(sensor4);
      Serial.print(" 5:");
      Serial.print(sensor5);
      Serial.print(" 6:");
      Serial.print(sensor6);
      Serial.print(" TopAvg:");
      Serial.print(top_row_raw_avg);
      Serial.print(" BottomAvg:");
      Serial.print(bottom_row_raw_avg);
      Serial.print(" TopAct:");
      Serial.print(top_row_activity);
      Serial.print(" BottomAct:");
      Serial.print(bottom_row_activity);
      Serial.print(" Diff:");
      Serial.println(row_difference);
    }
  }
  analogWrite(A0, gearOut[current_gear]); // 0 to 4095
}