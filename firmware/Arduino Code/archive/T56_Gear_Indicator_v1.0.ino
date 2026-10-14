int current_gear = 0;  // 0 = N, 1-6 = gears 1-6; reverse handled separately
const int ADC_MAX = 4095;
const float HALL_MIN_VOLTS = .6;
const float HALL_MAX_VOLTS = 4.8;
const int HALL_MIN_VALUE = ADC_MAX * (HALL_MIN_VOLTS / 5.0);
const int HALL_MAX_VALUE = ADC_MAX * (HALL_MAX_VOLTS / 5.0);
const int gearOut [7] = { 0, 682, 1364, 2047, 2731, 3412, 4095 };

const unsigned long SENSOR_INTERVAL_MS = 5;   // How often to evaluate the sensor readings
const int REQUIRED_STABLE_READS = 10;  // Candidate gear must remain unchanged for this many consecutive evaluations before current_gear is changed.

  // Row difference thresholds, in percentage points.
const int ROW_ENTER_THRESHOLD = 20;
const int ROW_EXIT_THRESHOLD = 12;
  // When currently in Neutral, the row averages must differ by at least 20 points before entering a gear.
  // Once already in a gear, the difference only has to remain above 12 points to stay out of Neutral.
  // This hysteresis prevents flickering at the Neutral boundary.

void setup() {
  //pinMode(A0, OUTPUT); // set pin A0 as output to send analog voltage ECU - Do NOT use PinMode for DAC use
  pinMode(A1, INPUT); // set pin A1 as input to read analog voltage from the Hall Sensor
  pinMode(A2, INPUT); // set pin A2
  pinMode(A3, INPUT); // set pin A3
  pinMode(A4, INPUT); // set pin A4
  pinMode(A5, INPUT); // set pin A5
  pinMode(A6, INPUT); // set pin A6
  analogReadResolution(12);
  analogWriteResolution(12); // # of bits of DAC resolution, via DAC0 on Nano R4 pin A0
}
void loop() {
  int hall_1 = analogRead(A1); // read pin A1, 0-4095
  int hall_2 = analogRead(A2); // pin A2
  int hall_3 = analogRead(A3); // pin A3
  int hall_4 = analogRead(A4); // pin A4
  int hall_5 = analogRead(A5); // pin A5
  int hall_6 = analogRead(A6); // pin A6
  
  int sensor1 = constrain( map ( hall_1, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100); // convert hall value to range 0-100%
  int sensor2 = constrain( map ( hall_2, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100);
  int sensor3 = constrain( map ( hall_3, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100);
  int sensor4 = constrain( map ( hall_4, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100);
  int sensor5 = constrain( map ( hall_5, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100);
  int sensor6 = constrain( map ( hall_6, HALL_MIN_VALUE, HALL_MAX_VALUE, 0, 100 ), 0, 100);

  // hall sensors are arranged like this. Reverse will be ignored, it is overridden by hardware reverse sensor
  //
  //      A1    A3    A5     
  //      1     3     5     R
  //      │     │     │     │
  //      ├─────┼─────│─────┘
  //      │     │     │      
  //      2     4     6      
  //      A2    A4    A6     

 // ============================================================
  // SENSOR COMPARISON AND GEAR IDENTIFICATION
  // ============================================================

  // These retain their values between passes through loop()
  static unsigned long last_sensor_evaluation = 0;
  static int last_candidate_gear = 0;
  static int stable_read_count = 0;

  // Only perform gear evaluation every SENSOR_INTERVAL_MS
  if (millis() - last_sensor_evaluation >= SENSOR_INTERVAL_MS) {
    last_sensor_evaluation += SENSOR_INTERVAL_MS;

    // ----------------------------------------------------------
    // DETERMINE WHICH ROW THE SHIFTER IS IN
    //
    // Top row:    sensors 1, 3, 5
    // Bottom row: sensors 2, 4, 6
    // ----------------------------------------------------------
    int top_row_avg =
      (sensor1 + sensor3 + sensor5) / 3;
    int bottom_row_avg =
      (sensor2 + sensor4 + sensor6) / 3;
    int row_difference =
      top_row_avg - bottom_row_avg;

    // ----------------------------------------------------------
    // SELECT THE APPROPRIATE NEUTRAL THRESHOLD
    // ----------------------------------------------------------
    int active_row_threshold;
    if (current_gear == 0) {
      // Currently Neutral:
      // require stronger evidence before entering a gear.
      active_row_threshold = ROW_ENTER_THRESHOLD;
    }
    else {
      // Currently in gear:
      // allow a smaller difference before returning to Neutral.
      active_row_threshold = ROW_EXIT_THRESHOLD;
    }
    // ----------------------------------------------------------
    // DETERMINE THE CANDIDATE GEAR
    // ----------------------------------------------------------
    int candidate_gear = 0;
    // NEUTRAL
    if (abs(row_difference) < active_row_threshold) {
      candidate_gear = 0;
    }
    // TOP ROW - GEARS 1, 3, 5
    else if (row_difference > 0) {
      if (hall_1 >= hall_3 &&
          hall_1 >= hall_5) {
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
    else {
      if (hall_2 >= hall_4 &&
          hall_2 >= hall_6) {
        candidate_gear = 2;
      }
      else if (hall_4 >= hall_2 &&
               hall_4 >= hall_6) {
        candidate_gear = 4;
      }
      else {
        candidate_gear = 6;
      }
    }
    // ----------------------------------------------------------
    // TIME / PERSISTENCE FILTER
    //
    // A candidate gear must be detected repeatedly before it
    // becomes the official current_gear.
    // ----------------------------------------------------------
    if (candidate_gear == last_candidate_gear) {
      // Same candidate as the previous evaluation
      if (stable_read_count < REQUIRED_STABLE_READS) {
        stable_read_count++;
      }
      // Candidate has now remained stable long enough
      if (stable_read_count >= REQUIRED_STABLE_READS) {
        current_gear = candidate_gear;
      }
    }
    else {
      // Candidate changed, so begin timing the new candidate
      // from the beginning.
      last_candidate_gear = candidate_gear;
      stable_read_count = 1;
    }
  }
  analogWrite(A0, gearOut[current_gear]); //0 to 4095
}
