# Laser Intensity Stabilization
This project is under development.

[Development Notebook](https://docs.google.com/document/d/1RtXi4E6TiI_Ld4MZSRgFrHrQVlKGbRzDl3qZmrDYwTk/edit#)

## Introduction

We are aiming to stabilize intensity of laser to a fluctuation range of plus-minus 1% using Acousto-optic Modulator (AOM) for a long timescale (in the order of days). This stabilization will improve the precision of experiments that use laser beams.

## P 3/2 Life Measurement
### Lifetime Measurement

### Problems
Laser intensity oscillates with time, and fluctuation of about 15% of total intensity power without stabilization can be detected. This unstable laser intensity affects the precision of Ra-226 excitation.

### Goal
Our goal for this research is to find a laser stabilization method that works for any lasers with any desired intensity. Users are able to manually set the desired intensity. The proportional and integral constants in the PID controller will be able to change on its own corresponding to the input signals.


## Schematics
### Feedback Loop
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3ffd6075bf42a9dc5e550fbc7e51a863ef81b96/feedback%20loop%20logic%20flow.png" width="400">

### Experiment Setup Schematics
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/8adf60ae177b7b4bb6f551d9992dfffc9aff9274/Experiment%20Setup.png" width="800">

- The laser beam will first go through the AOM, which determines the intensity that goes through. We use the zeroth order beam. 
- 10 percent of the power will be recieved by the photodiode, and a divider divides the power signal into DC and AC. The divided signals are the live signals that will be analyzed in the PID program.
- A PID program is built in Red Pitaya. It collects signals from the photodiode and compare it to a setpoint which we are aiming for the laser to be stabilized at. The PID program will then return a DC gain signal that will be sent to a voltage variable attenuator.
- The attenuator receives a DC gain signal as well as an RF reference signal, and it will output an RF gain with its amplitude related to the DC input corresponding to the figure below.
- The RF gain is then amplified by an op-amp and put back into the AOM to alter the intensity that goes through.

<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/f4062dce6361fdb36354567c01299cb552100ea4/Screen%20Shot%202021-04-20%20at%208.15.58%20AM.png" width="400">

The attenuation of our ZX73-2500+ attenuator is non-linear. The nonlinearity would require inputing the control signal at higher voltage to achieve high precision.

## Components

- Photo Diode: [PDA10A2](https://www.thorlabs.com/thorproduct.cfm?partnumber=PDA10A2)
- Acousto Optic Modulator: [ATM-802DA1](https://intraaction.com/wp-content/themes/Divi/pdf/ATMModSeries08991.pdf)
- Laser: [SYNCROLASE-100](https://en.yuyiparts.com/aerotech-syncrolase-100-helium-neon-laser.html)
- Voltage Controlled Oscillator: [ZX95-100-S+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZX95-100-S%2B)
- Voltage Variable Attenuator: [ZX73-2500-S+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZX73-2500-S%2B)
- RF Amplifier: [ZHL-1-2W+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZHL-1-2W%2B)
