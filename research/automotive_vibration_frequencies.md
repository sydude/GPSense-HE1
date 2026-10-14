To calculate the vibration profile of an automotive transmission paired with a V8 engine, the complex structural vibrations are broken down into discrete, predictable frequencies based on engine firing pulses, rotational gear speeds, and vehicle road velocity.
------------------------------
## Part 1: Vibration Frequency Range at Idle (Neutral, 800 RPM, 0 MPH)
When the car is idling in neutral and stationary, the vehicle's output shaft and chassis are isolated. The transmission experiences vibrations from two primary sources: direct structural engine rotation and combustion firing pulses.
For a standard 4-stroke, 8-cylinder engine idling at 800 RPM, the vibrations are dominated by the following frequencies:

* Primary Engine Rotational Frequency (1st Order):
$$\frac{800 \text{ RPM}}{60 \text{ seconds}} = \mathbf{13.33} \text{Hz}$$ 
This represents the physical imbalance and rotation of the crankshaft.
* Engine Firing Frequency (4th Order - Dominant):
In a 4-stroke engine, each cylinder fires once every two revolutions. A V8 engine produces exactly 4 firing strokes per revolution (_8 \div 2_).
$$13.33 \text{ Hz} \times 4 = \mathbf{53.33} \text{Hz}$$ 
This is the strongest vibration amplitude felt through the transmission housing at idle.
* Transmission Input Shaft Idle (Neutral):
Because the clutch is engaged in neutral, the transmission input shaft and countershaft spin freely at engine speed. Gear-meshing harmonics under zero load create subtle micro-chatter frequencies generally tracking between 13.33 Hz and 53.33 Hz, along with faint high-frequency harmonics (bearing noise) stretching up to 200 Hz.

------------------------------
## Part 2: Estimating Frequencies While Moving and In Gear
When the vehicle is in motion, the vibration spectrum expands into three distinct, calculable frequencies. Use these straightforward formulas to estimate each profile:
## 1. Engine RPM Frequency (_f_{\text{engine}}_)
This tracks the combustion and rotational inputs pushing into the front of the transmission.
$$\mathbf{f_{\text{engine}} (Hz) = \frac{\text{Engine RPM}}{60} \times \text{Order Index}}$$ 

* For pure crankshaft rotation (1st order): _\text{RPM} \div 60_
* For dominant V8 firing pulses (4th order): _(\text{RPM} \div 60) \times 4_
* Example at 3,000 RPM: Main firing vibration is exactly 200 Hz (_(3000 \div 60) \times 4_).

## 2. Transmission Output Shaft Frequency (_f_{\text{output}}_)
The output shaft speed dictates the rotational frequency exiting the transmission before it enters the driveshaft. Because it is mechanically locked to the engine via the selected gear ratio, it changes depending on what gear you are in:
$$\mathbf{f_{\text{output}} (Hz) = \frac{f_{\text{engine (1st order)}}}{\text{Gear Ratio}}}$$ 

* Underdrive Gears (Gears 1–3): The output shaft spins slower than the engine (e.g., a 3.00:1 first gear means the output shaft vibrates at _1/3_ of the engine's physical rotation frequency).
* Direct Drive (Usually 4th Gear): The ratio is 1.00:1. The output shaft rotational frequency perfectly matches the engine's 1st-order rotational frequency.
* Overdrive Gears (Gears 5–6): The output shaft spins faster than the engine (e.g., a 0.50:1 top gear means the output shaft vibrates at double the engine's rotational frequency).

## 3. Rolling Chassis Frequency per MPH (_f_{\text{chassis}}_)
To map vehicle speed directly to a chassis vibration frequency, you must calculate how many times the tire rotates per second at 1 MPH. This requires knowing your tire diameter and your differential rear-axle gear ratio.
$$\mathbf{\text{Tire Revolutions per Mile} = \frac{63,360}{\pi \times \text{Tire Diameter in inches}}}$$ 
$$\mathbf{f_{\text{chassis}} \text{ per MPH (Hz)} = \frac{\text{Tire Revolutions per Mile}}{3,600 \text{ seconds in an hour}}}$$ 
## Calculating for a Specific Setup (Example: 35-inch Tires)

   1. Tire Rotations: A 35-inch tire completes roughly 576.2 revolutions per mile (_\frac{63360}{35 \times 3.14159}_).
   2. Wheel/Chassis Frequency at 1 MPH:
   $$\frac{576.2}{3600} = \mathbf{0.16 \text{ Hz per MPH}}$$ 
   At 60 MPH, the wheel rotation frequency is _0.16 \times 60 = \mathbf{9.6} \text{Hz}_.
   3. Driveshaft/Output Shaft baseline via Road Speed:
   If you want to cross-reference road speed back to the transmission output shaft directly, multiply the wheel frequency by your rear-axle gear ratio (for example, 4.88 gears):
   $$\mathbf{f_{\text{driveshaft}} = \text{Wheel Frequency} \times \text{Axle Ratio}}$$ 
   At 60 MPH: _9.6 \text{ Hz} \times 4.88 = \mathbf{46.85} \text{Hz}_ entering the rear of the transmission case.

------------------------------
## Summary Matrix for Energy Harvesting Target Frequencies

| Component | Calculation Basis | Frequency Range | Energy Characteristics |
|---|---|---|---|
| Engine (V8 Firing) | _(\text{RPM} \div 60) \times 4_ | 53 Hz to 400 Hz (Idle to 6000 RPM) | High amplitude, continuous excitation. Best target for fixed-element piezos. |
| Transmission Output | _\text{Engine Rotational} \div \text{Gear Ratio}_ | 5 Hz to 150 Hz (Varies dynamically by gear) | Mid amplitude, highly dependent on vehicle acceleration. |
| Chassis / Driveshaft | _(\text{MPH} \times 0.16) \times \text{Axle Ratio}_ | 0 Hz to 60+ Hz (Scales linearly with road speed) | Predictable, steady-state frequency during highway cruising. |

If you want to calculate the exact frequencies for your specific vehicle setup, let me know:

* Your transmission gear ratios or transmission model
* The rear axle gear ratio (e.g., 4.88)
* Your tire diameter (e.g., 35 inches)

I can map out the precise Hz values across your entire RPM and speed range.

