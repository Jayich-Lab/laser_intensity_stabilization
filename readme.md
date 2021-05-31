# Laser Intensity Stabilization

[Development Notebook](https://docs.google.com/document/d/1RtXi4E6TiI_Ld4MZSRgFrHrQVlKGbRzDl3qZmrDYwTk/edit#)

We are aiming to stabilize intensity of laser to a fluctuation range of plus-minus 1% using Acousto-optic Modulator (AOM) for a long timescale (in the order of days). This stabilization will improve the precision of experiments that use laser beams.

## Table of Contents
- [Motivation](#Motivation)
  - [P 3/2 Life Measurement](#P-32-Life-Measurement)
  - [Problems](#Problems)
  - [Goal](#Goal)
- [List of Components](#List-of-Components)
- [Schematics](#Schematics)
  - [Feedback Loop Idea](#Feedback-Loop-Idea)
  - [Experiment Setup Schematics](#Experiment-Setup-Schematics)
  - [Acousto-Optics Modulator (AOM)](#Acousto-Optics-Modulator-AOM)
  - [Red Pitaya](#Red-Pitaya)
  - [Voltage Variable Attenuator (VVA)](#Voltag-Variable-Attenuator-VVA)
- [Feedback Loop](#Feedback-Loop)
- [First Iteration](#First-Iteration)
- [Future Plans](#Future-Plans)
- [Contributors](#Contributors)

# Motivation
## P 3/2 Life Measurement

## Problems
Laser intensity drifts with time, and the amplitude drift could result in un-precise energy level transition, which affects energy shift induced by ac-stark shift of detuned laser. This unstable laser intensity affects the precision of Ra-226 excitation.

## Goal
Our goal for this research is to find a laser stabilization method that works for any lasers with any desired intensity. Users are able to manually set the desired intensity. The proportional and integral constants in the PID controller will be able to change on its own corresponding to the input signals.

# List of Components

- Photo Diode: [PDA10A2](https://www.thorlabs.com/thorproduct.cfm?partnumber=PDA10A2)
- Acousto Optic Modulator: [ATM-802DA1](https://intraaction.com/wp-content/themes/Divi/pdf/ATMModSeries08991.pdf)
- Laser: [SYNCROLASE-100](https://en.yuyiparts.com/aerotech-syncrolase-100-helium-neon-laser.html)
- Voltage Controlled Oscillator: [ZX95-100-S+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZX95-100-S%2B)
- Voltage Variable Attenuator: [ZX73-2500-S+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZX73-2500-S%2B)
- RF Amplifier: [ZHL-1-2W+](https://www.minicircuits.com/WebStore/dashboard.html?model=ZHL-1-2W%2B)

# Schematics
## Feedback Loop Idea
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/04776fa809f75d422d323a4c438cc6fed354564e/feedback%20loop%20logic%20flow.png" width="500">

The laser beam will be received by a detector, and the signal will be sent to the PID controller. A control signal will then be sent to the laser intensity controller to adjust the laser intensity. The adjusted laser beam will again be received by the detector and teh loop forms to keep the intensity stable. 

## Experiment Setup Schematics
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/b2fe35d18d52c8cc31f6680785b2dc8ce0ade054/Experiment%20Setup%20with%20number.png" width="800">

First, the laser beam goes through fiber between two fiber ports. Then the beam goes through Acousto-Optic Modulator (AOM) [1], which acts as the controller of the laser intensity. 
- 10 percent of the power is recieved by the photodiode [2], and a divider [3] divides the power signal into DC and AC. The divided signals are the live signals that will be analyzed in the PID program.
- A PID program is built in Red Pitaya [4]. It collects signals from the photodiode and compare it to a setpoint which we are aiming for the laser to be stabilized at. The PID program will then return a DC gain signal that will be sent to a voltage variable attenuator [5].
- The attenuator receives a DC gain signal as well as an RF reference signal, and it will output an RF gain with its amplitude related to the DC input corresponding to the figure below.
- The RF gain is then amplified by an op-amp [7] and put back into the AOM to alter the intensity that goes through.

## Acousto-Optics Modulator (AOM)
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/77af682c2d3fe3e8ef1c32c5de5513a498ebd6c6/Acoustco-Optic%20effect.png" width="300">
  
An AOM uses the acousto-optic effect, which is similar to the Bragg diffraction, to diffract and shift the frequency of the incident light. The acousto-optic effect describes the diffraction of light by acoustic waves. The basis of this effect is a general effect of photoelasticity consisting in the change of the medium permittivity under the action of a mechanical strain. With the acoustco-optic effect, the amount of light diffracted by the sound wave depends on the intensity of the sound. Thus, we use the intensity of the sound to modulate the intensity of the light.

For our project, we choose the first order beam from the AOM, since we are pursuing a maximum dynamic range for the setup. The dynamic range is the ratio between the largest and smallest values of the intensity that can be ouputed by the the AOM. For the zeroth order beam, the intensity can be attenuated from 100 percent to 20 percent, which provides a dynamic range from 100 to 300; while for the first order beam, the intensity can be attenuated from 85 percent to 0 percent, which basically provides a dynamic range from 100 all the way to infinity. Thus, we use the first order beam for our project.

## Red Pitaya

## Voltage Variable Attenuator (VVA)
<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/f4062dce6361fdb36354567c01299cb552100ea4/Screen%20Shot%202021-04-20%20at%208.15.58%20AM.png" width="400">

The attenuation of our ZX73-2500+ attenuator is non-linear. The nonlinearity would require inputing the control signal at higher voltage to achieve high precision.

# Feedback Loop

# First Iteration
<img src ="https://user-images.githubusercontent.com/77765489/119746311-0f153100-be45-11eb-9ecc-4c6befbecd9d.png" width = "800">

# Future Plans
For P 3/2 life measurement, we need a pulsed feedback that turns on and off for millliseconds. Currently we use double AOM CW feedback as the setup; however, there are two issues to be solved. First, the double AOM CW feedback has an intensity delay, which causes failure in the PID control; second, the paused feedback must be run from DACO, ?????. A possible solution for the issues may be to use Arduino as the PID controller with the same experimental setup.

# Contributors
This project is made for CS 15C class by Chaoshe Zhang, Sam Gebretsadkan, Vivian Liao and Rei Landsberger.
