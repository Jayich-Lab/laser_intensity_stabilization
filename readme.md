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
  - [Experiment Setup Schematics](#Experiment-Setup-Schematics)
  - [Acousto-Optics Modulator (AOM)](#Acousto-Optics-Modulator-AOM)
  - [Red Pitaya](#Red-Pitaya)
  - [Voltage Variable Attenuator (VVA)](#Voltag-Variable-Attenuator-VVA)
- [Feedback Loop](#Feedback-Loop)
  - [PID Controller](#PID-Controller)
  - [Continuous (CW) Feedback](#Continuous-CW-Feedback)
  - [Pulsed Feedback](#Pulsed-Feedback)
- [Iterations](#Iterations)
  - [PI Stabilization](#PI-Stabilization)
  - [I Stabilization](#I-Stabilization)
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
## Experiment Setup Schematics
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/b2fe35d18d52c8cc31f6680785b2dc8ce0ade054/Experiment%20Setup%20with%20number.png" width="1000">
</div>

First, the laser beam goes through the fiber and the two fiber ports, and then it goes through [Acousto-Optic Modulator (AOM)](#Acousto-Optics-Modulator-AOM) [1], which diffracts the light to different intensity. Next, 10 percent of the power is recieved by the photodiode [2], and a divider [3] divides the power signal into DC and AC. The divided signals are the live signals that will be analyzed in the PID program. A PID program is built in [Red Pitaya](#Red-Pitaya) [4]. It collects signals from the photodiode and compare it to a setpoint voltage, which we are aiming for the laser to be stabilized at, to create an error signal. The Red Pitaya will then return this DC gain error signal and send it to a [voltage variable attenuator (VVA)](#Voltage-Variable-Attenuator-VVA) [5]. The attenuator receives a DC gain signal as well as an RF reference signal, and it will output an attenuate RF gain with attenuation determined by the input control voltage. The RF gain is then amplified by an op-amp [7] and put back into the AOM to alter the intensity that goes through it.

## Acousto-Optics Modulator (AOM)
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/77af682c2d3fe3e8ef1c32c5de5513a498ebd6c6/Acoustco-Optic%20effect.png" width="300">
</div>

An AOM uses the acousto-optic effect, which is similar to the Bragg diffraction, to diffract and shift the frequency of the incident light. The acousto-optic effect describes the diffraction of light by acoustic waves. The basis of this effect is a general effect of photoelasticity consisting in the change of the medium permittivity under the action of a mechanical strain. With the acousto-optic effect, the amount of light diffracted by the sound wave depends on the intensity of the sound. Thus, we use the intensity of the sound to modulate the intensity of the light.

For our project, we choose the first order beam from the AOM. The difference between each order beam is that they have different dynamic range. The dynamic range is the ratio between the largest and smallest values of the intensity that can be ouputted by the the AOM. For the zeroth order beam, the intensity can be attenuated from 100 percent to 20 percent, which provides a dynamic range from 100 to 300; while for the first order beam, the intensity can be attenuated from 85 percent to 0 percent, which basically provides a dynamic range from 100 all the way to infinity. Thus, we choose the first order beam for our project to have a larger dynamic range.

## Red Pitaya
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/772316f9d438e699e2c2b01aafd8a261f7e7b2d2/RedPitaya_STEMLab_FPGA_v1.1_frontview.jpg" width="400">
</div>

Red Pitaya is an affordable, PC based, multi instrument. It acts as the brain of our project, which takes up the role of PID controller for the feedback loop (Discussed in [Feedback Loop Section](#Feedback-Loop)). Signal came from the photodiode is received by the DAC protal of the Red Pitaya, and after the PID program an output signal will be sent out from the ADC protal to the voltage variable attenuator (VVA).

## Voltage Variable Attenuator (VVA)
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3fd1acefe8639c5e60ba01c673978963c45f419/VVA.png" width="400">
</div>

Attenuators are transmission line components used to reduce the input power in a system by a predetermined amount. It is operated throughout its entire dynamic range. Different from fixed attenuators, variable attenuators can be controlled to vary the attenuation level of the device with a number of different methods. A voltage varaible attenuator, the VVA, has its attenuation controlled by the voltage. The typical attenuation of the VVA we use, ZX73-2500+ attenuator, is shown in the figure below.
<br>
<div align="center">
<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/f4062dce6361fdb36354567c01299cb552100ea4/Screen%20Shot%202021-04-20%20at%208.15.58%20AM.png" width="500">
</div>

The purpose of using a VVA in our project is to diminish the failure of the feedback loop caused by the overcompensation of the PID program. An overcompensation of the PID program means that the PID overcompensates for every positive errors and creates an even larger error in the negative direction (and vice-versa). With a VVA in the circuit, the overcompensation from the PID feedback will not directly affect the AOM and will not cause serious failure in the stabilization. For this purpose, we prefer a relatively flat and linear attenuation curve for our attenuator. From the figure above we can see that the attenuation of our ZX73-2500+ attenuator is non-linear, with the curve relatively linear at control voltage range 0 to 0.5 volts and at high voltage. To achieve high precision, we require the input control signal at higher voltage. We choose 4-12 volts as our range of control voltage.

# Feedback Loop
## PID Controller
<div align="center">
<img src="https://user-images.githubusercontent.com/77765489/120875179-66ab4f00-c55f-11eb-840d-cb8d25912a90.png" width="600">
</div>

## Code


## Continuous (CW) Feedback
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3fd1acefe8639c5e60ba01c673978963c45f419/pulsed%20feedback.png" width="600">
</div>


## Pulsed Feedback
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3fd1acefe8639c5e60ba01c673978963c45f419/pulsed%20feedback%20fig.png" width="600">
</div>


# Iterations
## PI Stabilization
<div align="center">
<img src ="https://user-images.githubusercontent.com/77765489/119746311-0f153100-be45-11eb-9ecc-4c6befbecd9d.png" width = "800">
</div>

## I Stabilization
<div align="center">
<img src ="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/main/802nm_stabilization.png" width = "800">
</div>
**Most recent and important data from the Intensity Stabilization project is this graph above**

Using BIFROST laser light (20% intensity fluctuation without stablilization), we stablilized the laser to average within $\pm$ 0.1% of our setpoint intensity. Extreme peakks during stablization still stayed within $\pm$ 0.3% of setpoint.  Long term drift of intensity from BIFROST is no longer a problem with this intensity stabilizer.

###Observations:
1. It was found that **omitting** the proportional term for PID control was more stable than including proportional control.
2. The lower the control voltage (shown in the bottom graph as it slopes downward), the less stablity we get due to the VVA response curve at different voltages.


# Future Plans
For P 3/2 life measurement, we need a pulsed feedback that turns on and off for millliseconds. Currently we use double AOM CW feedback as the setup; however, there are a few issues to be solved. First, the double AOM CW feedback has an intensity delay, which causes failure in the PID control; second, the paused feedback must be run from DACO. In addition to this, the second AOM's thermal fluctuations will be slightly different from the first AOM, causing de-optimization of our stablility.

**A possible solution for the issues may be to use [Arduino DUE] (https://store.arduino.cc/usa/due) as the PID controller with the same experimental setup.**  This will bypass computer-to-RedPitaya pulsing and instead allow for independent pulse feedback.

# Contributors
This project is made for CS 15C class by Chaoshe Zhang, Sam Gebretsadkan, Vivian Liao and Rei Landsberger.
