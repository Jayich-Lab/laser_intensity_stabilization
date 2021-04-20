# Laser Intensity Stabilization
This project is under development.

## Introduction

We are aiming to stabilize intensity of laser to a fluctuation range of plus-minus 1% using Acousto-optic Modulator (AOM) for a long timescale (in the order of days). This stabilization will improve the precision of experiments that use laser beams.

## Schematics
<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/00ad7da555807ec47b507d962022d027033bcd3a/Intensity%20stablization%20schematic%20iteration%201.jpg" width="800">

- The laser beam will first go through the AOM, which determines the intensity that goes through. We use the zeroth order beam. 
- 10 percent of the power will be recieved by the photodiode, and a divider divides the power signal into DC and AC. The divided signals are the live signals that will be analyzed in the PID program.
- A PID program is built in Red Pitaya. It collects signals from the photodiode and compare it to a setpoint which we are aiming for the laser to be stabilized at. The PID program will then return a DC gain signal that will be sent to a voltage variable attenuator.
- The attenuator receives a DC gain signal as well as an RF reference signal, and it will output an RF gain with its amplitude related to the DC input corresponding to the figure below.
- The RF gain is then amplified by an op-amp and put back into the AOM to alter the intensity that goes through.

<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/f4062dce6361fdb36354567c01299cb552100ea4/Screen%20Shot%202021-04-20%20at%208.15.58%20AM.png" width="400">

## Goal
Our goal for this research is to find a laser stabilization method that works for any lasers with any desired intensity. Users are able to manually set the desired intensity. The proportional and integral constants in the PID controller will be able to change on its own corresponding to the input signals.
