int input_setpoint = 2047;
int output_setpoint = 2047;
float p_gain = 1.;
float i_gain = 0.;
float i_term = 0;

void setup() {
  Serial.begin(115200);

  ADC->ADC_MR |= 0x80; // these lines set free running mode on adc 7 (pin A0)
  ADC->ADC_CR = 2;
  ADC->ADC_CHER = 0x80;
  analogWriteResolution(12);
}

int read_adc() {
  while ((ADC->ADC_ISR & 0x80) == 0); // wait for conversion
  int a0 = ADC->ADC_CDR[7];            // read data
  return a0;
}

int calculate_output(int reading) {
  float error = reading - input_setpoint;
  i_term += i_gain * error;
  if (i_term < -4096) {
    i_term = -4096;
  }
  else if (i_term > 4095) {
    i_term = 4095;
  }
  float p_term = p_gain * error;
  return int(p_term + i_term) + input_setpoint;
}

void read_gain() {
  if (Serial.available() > 1) {
     int first_byte = Serial.read();
     if (first_byte == 'P') {
      p_gain = float(Serial.read()-'5');
     }
     else if (first_byte == 'I') {
      i_gain = float(Serial.read()-'5');
      i_gain = i_gain/20;
     }
  }
}

void loop() {
  read_gain();
  int rep = 10;
  int t = micros();
  for (int i = 0; i < rep; i++) {
    int reading = read_adc();
    int output = calculate_output(reading);
    if (output < 0) {
      output = 0;
    }
    else if (output > 4095) {
      output = 4095;
    }
    analogWrite(DAC0, output);
  }
  //Serial.println(i_gain);
  //Serial.println(p_gain);
}
