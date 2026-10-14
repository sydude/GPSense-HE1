//* Old Gearshift Code for New stripped of display code

int gearPattern[9][12] = {                  // accessed in Y (Row) , Z (Column)  order
  {0, 0, 1, 3, 3, 3, 3, 3, 3, 5, 5, 7},
  {0, 0, 1, 3, 3, 3, 3, 3, 3, 5, 5, 7},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 7},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6},
  {2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6},
  {2, 2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6}
};

int gearVolts[8] = { 75, 100, 125, 150, 175, 200, 225, 250 };  // Actual Voltage x 100 (.75 * 100 = 75)
int oldgear = 8;              // initial outside range setting forces an update at first run
int newgear = 0;              // 0 = Neutral .. 7 = Reverse

int yCalc = 0, zCalc = 0;  // 3d Sensor calibration, variables and factors
const float yOffset = 0.00, zOffset = 0;
const float yFactor = 1.00, zFactor = 1.00; // to try and scale the mT range to the mins and max's
const int yMin = -6, yMax = 6, zMin = 5, zMax = 35;

unsigned long previousMillis = 0; // store the exact system time that Neutral was selected last
const long minTime = 400;           // how long Neutral needs to be held before it can be officially displayed

int GEAR_OUT_PIN = A0;  // true analog output using DAC0 (analog pin 10)

void setup() {

  analogWriteResolution(12); // # of bits of DAC resolution, via DAC0 on Nano R4 pin A0

}

void loop() {
  unsigned long currentMillis = millis();

  sensestart:    // just a GOTO location label
  Tlv493dMagnetic3DSensor.updateData();
  if (Tlv493dMagnetic3DSensor.getY() == 0 && Tlv493dMagnetic3DSensor.getZ() == 0) { // Sensor Error Check. If sensor reads all zeroes then print 'Sensor Error'
    delay(1000);
    // resetFunc();                     // this will call the reset of the Arduino using the aforementioned reset function at Address 0
    goto sensestart; // the sensor isn't working, so for now skip the rest of the loop and go right back to the beginning
  }
  yCalc = Tlv493dMagnetic3DSensor.getY() * yFactor;
  yCalc = constrain(yCalc, yMin, yMax);
  yCalc = map(yCalc, yMin, yMax, 0, 8);
  if (Tlv493dMagnetic3DSensor.getY() <= -15) {
    zCalc = 32;
  }
  else {
    zCalc = Tlv493dMagnetic3DSensor.getZ() * zFactor;
  }
  zCalc = constrain(zCalc, zMin, zMax);
  zCalc = map(zCalc, zMin, zMax, 11, 0); // NOTE: range is REVERSED to adust for decreasing mT as Column increases
  
  newgear = gearPattern[yCalc][zCalc]; // gets the current gear from the array

  //  if (SerialUSB) {
   //  updateMonitor(); // Recomment-in this and all Serial Print lines for debugging
  //  }

  if (newgear != oldgear && newgear != 0) {               // if the gear changes AND the new gear is NOT neutral
    oldgear = newgear;                    // immediately set the new gear selection...
    updateTFT();                          // and update the display
    //    sendGearVolts();
  }  
  if (newgear != oldgear && newgear == 0) {               // if the gear changes AND the new gear is neutral, test it first to see if it sticks
    if (previousMillis == 0) {                // if this is the first time through the loop that the new gear is neutral...
      previousMillis = currentMillis;         // turn the clock 'ON' to see if Neutral 'sticks' by setting it to current time
    }
    else if (currentMillis - previousMillis > minTime) {  // if /neutral is held long enough... then lets finalize it..
      previousMillis = 0;                   // reset the timer to zero (where it will be turned 'OFF' again)...
      oldgear = newgear;                    // set the new gear selection as Neutral... 
      updateTFT();                          // and update the display
    }
  }

  // write a number XXX in ranges for 1st thru 6th gear
  analogWrite(GEAR_OUT_PIN, XXX);

}