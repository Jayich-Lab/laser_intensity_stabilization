# Laser Intensity Stabilization
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/a74a1729dd4d4fad07957e3e5872ce33a577457c/real%20setup.jpg" width="1000">
</div>

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
  - [Code](#Code)
  - [Continuous (CW) Feedback](#Continuous-CW-Feedback)
  - [Pulsed Feedback](#Pulsed-Feedback)
- [Iterations](#Iterations)
  - [PI Stabilization](#PI-Stabilization)
  - [I Stabilization](#I-Stabilization)
- [Future Plans](#Future-Plans)
- [Contributors](#Contributors)

# Motivation
## P 3/2 Life Measurement
<div align="center">
<img src="https://user-images.githubusercontent.com/77765489/120882021-f534c600-c589-11eb-8d38-a60a9130eace.png" width="300">
</div>
We want to measure the lifetime of the P 3/2 lifetime of the radium-226 ion. For this measurement, we need to stabilize a detuned 802 nm laser which will be used for probing the  D 5/2 to P 3/2 transitions. The need for laser intensity stabilization comes from the concept of Rabi flopping. When we raise an ion from its ground state to an excited state, it doesn’t simply remain excited; rather, it oscillates back and forth from the ground state to the excited state at a frequency known as the Rabi frequency. The Rabi frequency is directly proportional to the laser intensity. If the laser intensity is not stable, then the excitation oscillation will shift. We will end up with ions between the excited and ground state, which will increase the systematic errors in our measurements. 

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
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e600aa16eec254489eed7aad6a38697675a1e220/Experiment%20Setup%20New.png" width="1000">
</div>

First, the laser beam [1] is transmitted by the fiber and goes through the two fiber ports [2]. It then is received by the [Acousto-Optic Modulator (AOM)](#Acousto-Optics-Modulator-AOM) [3], which diffracts the light to different intensity. We use the first order beam in our project. Next, the beam coming out from the AOM goes through the PBS (Polarizing Beam Splitter) [4] and the NPBS (Non-Polarizing Beam Splitter) [5]. These two beam splitters split the light and control the amount of light that goes into the ion trap [6] and the photodiode [7]. We choose an NPBS that splits 90 percent of the light and lets 10 percent of the power go into the photodiode. The photodiode is the detector of laser intensity in our project. It converts the light signal into voltage, which can then be received and analyzed by the PID program in Red Pitaya. A divider [8] is put between the photodiode and the Red Pitaya to divide this power signal into DC and AC. A PID program is built in [Red Pitaya](#Red-Pitaya) [9]. It collects signals from the photodiode and compares it to a setpoint voltage to create an error signal. The Red Pitaya will then return this DC gain error signal and send it to a [voltage variable attenuator (VVA)](#Voltage-Variable-Attenuator-VVA) [10]. VVA receives this DC gain signal as well as an RF reference signal sent from the VCO (voltage control oscillator) [11], and it will output an attenuated RF gain with attenuation determined by the input control voltage (see the attenuation graph in the VVA section). The RF gain is then amplified by an RF amplifier [12] and sent back into the AOM to alter the intensity that goes through it.


## Acousto-Optics Modulator (AOM)
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/77af682c2d3fe3e8ef1c32c5de5513a498ebd6c6/Acoustco-Optic%20effect.png" width="300">
</div>

An AOM uses the acousto-optic effect, which is similar to the Bragg diffraction, to diffract and shift the frequency of the incident light. The acousto-optic effect describes the diffraction of light by acoustic waves. The basis of this effect is a general effect of photoelasticity consisting in the change of the medium permittivity under the action of a mechanical strain. With the acousto-optic effect, the amount of light diffracted by the sound wave depends on the intensity of the sound. Thus, we use the intensity of the sound to modulate the intensity of the light.

For our project, we choose the first order beam from the AOM. The difference between each order beam is that they have different dynamic ranges. The dynamic range is the ratio between the largest and smallest values of the intensity that can be outputted by the AOM. For the zeroth order beam, the intensity can be attenuated from 100 percent to 20 percent, which provides a dynamic range from 100 to 300; while for the first order beam, the intensity can be attenuated from 85 percent to 0 percent, which provides a dynamic range from 100 all the way to infinity. Thus, we choose the first order beam for our project to have a larger dynamic range.

The intensity of the first order beam is controlled by the control signal that goes into the AOM. When the control signal increases, the intensity increases correspondingly. We adjust the intensity of the laser using this property of the AOM.

## Red Pitaya
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/772316f9d438e699e2c2b01aafd8a261f7e7b2d2/RedPitaya_STEMLab_FPGA_v1.1_frontview.jpg" width="400">
</div>

Red Pitaya is an affordable, PC based, multi instrument. It acts as the brain of our project, which takes up the role of PID controller for the feedback loop (Discussed in Feedback Loop Section). Signal coming from the photodiode is received by the DAC portal of the Red Pitaya, and after the PID program an output signal will be sent out from the ADC portal to the voltage variable attenuator (VVA).

## Voltage Variable Attenuator (VVA)
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3fd1acefe8639c5e60ba01c673978963c45f419/VVA.png" width="400">
</div>

Attenuators are transmission line components used to reduce the input power in a system by a predetermined amount. It is operated throughout its entire dynamic range. Different from fixed attenuators, variable attenuators can be controlled to vary the attenuation level of the device with a number of different methods. A voltage variable attenuator, the VVA, has its attenuation controlled by the voltage. The typical attenuation of the VVA we use, ZX73-2500+ attenuator, is shown in the figure below.
<br>
<div align="center">
<img src="https://github.com/vivian-liao/laser_intensity_stabilization/blob/f4062dce6361fdb36354567c01299cb552100ea4/Screen%20Shot%202021-04-20%20at%208.15.58%20AM.png" width="500">
</div>

From the figure above, we can see that the attenuation of our ZX73-2500+ attenuator is non-linear, with the curve relatively linear and flat at control voltage range 0 to 0.5 volts and at high voltage. These linear regions imply that at these two control voltage ranges, the attenuation of the VVA does not change dramatically under large control voltage change. This property of the VVA helps to diminish the overcompensation of the PID program and prevent it from destroying the whole feedback loop. An overcompensation of the PID program means that the PID overcompensates for every positive error and creates an even larger error in the negative direction (and vice-versa). With a VVA in the circuit, the overcompensation from the PID feedback can be diminished and it will not cause serious failure in the stabilization. To achieve high precision, we require the input control signal at higher voltage. We choose 4-12 volts as our range of control voltage in our project.


# Feedback Loop
## PID Controller
<div align="center">
<img src="https://user-images.githubusercontent.com/77765489/120875179-66ab4f00-c55f-11eb-840d-cb8d25912a90.png" width="600">
</div>
To summarize, the laser goes into the photodiode, and the photodiode generates a voltage signal in the range of ± 4 volts. The red pitaya can only take a voltage from -1 to 1 volt, so we place a ¼ voltage divider in front of the pitaya. The red pitaya is remotely controlled using a client on a computer via an internal network. 
After receiving the signal, the pitaya runs a python script (discussed in the next section), compares the input voltage to the setpoint voltage, and applies a control signal, C, which depends on the proportional integral and differential terms of the error signal. The relationship can be seen in the equation above. We neglected the differential term for our setup as we weren’t concerned with stabilizing at high frequency (laser fluctuations tended to be low frequency). The control signal from the pitaya goes through an operational amplifier which increases the control range from ± 1 volt to 4 and 12 volts. The Voltage Variable Attenuator (VVA) section explains why we want to be in the 4 -12 voltage range. The control signal was also sent to an oscilloscope at the same time. The majority of the data was taken on an oscilloscope. The data from the oscilloscope was read by a server on the computer and sent to the red pitaya. The computer (server and client) are constantly communicating with the red pitaya during  the feedback loop. 
 

## Code
The programming was mainly done on the red pitaya using python and the PyRPL module, which has python defined classes for controlling the pitaya outputs and inputs. The PyRPL code was written onto a server, and a client was then made using that server. 
There was also a simpler script made for an independent laser intensity stabilization setup using an Arduino due. The code is above under “PI_Stabilization.ino”. The Arduino presents versatility as it uses c + +, a widely accessible programming language. On the other hand, the red pitaya can either be programmed using the limited PYRPL module or the native language of the pitaya, Verilog, which is less accessible. 
Either the Arduino or the pitaya is sufficient for stabilization. Still, when it comes to pulsed feedback (discussed shortly), the Arduino is a better choice as C + + is easier to work with than Verilog. 


## Continuous (CW) Feedback
<div align="center">
<img src="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e3fd1acefe8639c5e60ba01c673978963c45f419/pulsed%20feedback.png" width="600">
</div>
We need to identify a clear distinction between continuous and pulsed feedback. In this experiment, we are applying continuous waveform (CW) feedback. We are continuously reading data and stabilizing it over time. The picture above illustrates a summary of our CW feedback in a rectangle on the right. Generally, in ion trap experiments, laser beams are sent to the ion trap in pulse sequences (on the order of microseconds). We can create a stabilized pulsing laser by placing a second AOM after the laser is stabilized. The second AOM will receive a signal that turns it “on” or “off,” This way, we have a pseudo pulsed feedback setup. 

## Pulsed Feedback
<div align="center">
<img src="https://user-images.githubusercontent.com/77765489/120880065-cfed8b00-c57c-11eb-9de1-02c7f2a7fd75.png" width="600">
</div>
We created pseudo pulsed feedback, but what does pulsed feedback look like? Unlike CW feedback, in pulsed feedback, we only take data on short intervals (µs). The above graph models a pulse sequence where the laser is on for µs and off for ms. During the period the laser is on, a microcontroller collects data and generates an error signal E1. When the laser is off, the microcontroller outputs a constant dc offset. After the laser is turned on again, a fixed dc voltage (red line) is outputted based on the error signal E1. This process continues for all pulses.
*Note only CW feedback was used in this project 

# Iterations
## PI Stabilization
<div align="center">
<img src ="https://user-images.githubusercontent.com/77765489/119746311-0f153100-be45-11eb-9ecc-4c6befbecd9d.png" width = "800">
</div>
Laser intensity was stabilized to ±0.2%, which surpasses our goal of ± 1%. The blue plot is the signal received by the photodiode, the orange curve is the control signal output by the red pitaya, and the green line is the stabilized signal.  We took the data in 8 hours and found no drifts in the stabilization, which indicates that our setup is robust since it maintains the same intensity while environmental parameters such as temperature change. Using only the P and I terms, we were able to minimize the initial noise by a factor of 200. 

<div align="center">
<img src ="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/7792d7656eb247637dfc02c92de9fb8e0821be40/zoom%20in.png" width = "500">
</div>
Looking more closely at the stabilized signal, we can see no long-term drift; the signal only oscillates at high frequency. This high-frequency noise can be accounted for by the internal noise of the photodiode. The ±0.2% fluctuation in our data was partly correlated to the photodiode uncertainty; the rest of the noise comes from the control signal output by the pitaya. The above data set was preliminary data and demonstrated the sufficiency of our stabilization for the P 3/2 life measurement. 

## I Stabilization
<div align="center">
<img src ="https://github.com/Jayich-Lab/laser_intensity_stabilization/blob/e600aa16eec254489eed7aad6a38697675a1e220/I%20stabilization.png" width = "800">
</div>

Using 802 nm laser light (20% intensity fluctuation without stablilization), we stablilized the laser to average within ± 0.1% of our setpoint intensity. Extreme peaks during stablization still stayed within ± 0.3% of setpoint.  Long term drift of intensity from 802 nm is no longer a problem with this intensity stabilizer. The graph above was taken  on the laser that was used for the P 3/2 life time measurement. 

Observations:
1. It was found that **omitting** the proportional term for PID control was more stable than including proportional control.
2. The lower the control voltage (shown in the bottom graph as it slopes downward), the less stablity we get due to the VVA response curve at different voltages.


# Future Plans
For P 3/2 life measurement, we need a pulsed feedback that turns on and off for milliseconds. Currently we use double AOM CW feedback as the setup; however, there are a few issues to be solved. First, the double AOM CW feedback has an intensity delay, which causes failure in the PID control; second, the paused feedback must be run from DACO. In addition to this, the second AOM's thermal fluctuations will be slightly different from the first AOM, causing de-optimization of our stablility.

**_A possible solution for the issues may be to use [Arduino DUE](https://store.arduino.cc/usa/due) as the PID controller with the same experimental setup._**  This will bypass computer-to-RedPitaya pulsing and instead allow for independent pulse feedback.

# Contributors
This project is made for CS 15C class by Chaoshen Zhang, Sam Gebretsadkan, Vivian Liao and Rei Landsberger.
