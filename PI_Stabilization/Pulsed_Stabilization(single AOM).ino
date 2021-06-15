int input_setpoint = 2047;
int output_setpoint = 2047;
float p_gain = 1.;
float i_gain = 0.;
float i_term = 0;
int error_range = 30;
int error[list_range];
int DIGITALPIN = 1;

void setup() {
  Serial.begin(115200);
  pinMode(DIGITALPIN, INPUT);
  ADC->ADC_MR |= 0x80; // these lines set free running mode on adc 7 (pin A0)
  ADC->ADC_CR = 2;
  ADC->ADC_CHER = 0x80;

  analogWriteResolution(12);
  analogWrite(DAC0, output_setpoint);
}

int read_adc() {
  while ((ADC->ADC_ISR & 0x80) == 0); // wait for conversion
  int a0 = ADC->ADC_CDR[7];            // read data
  return a0;
}

int push(int list, int range){
  for (int i=0, i<=range-2, i++){
    list[i] = list[i+1];
  }
  list[range-1] = read_adc()-input_setpoint;
  return list
}

float average (int * array, int range)  // assuming array is int.
{
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < range ; i++)
    sum += array [i] ;
  return  ((float) sum) / range ;  // average will be fractional, so float may be appropriate.
}

void loop() {
  if (digitalRead == HIGH) {
    while (digitalRead == HIGH) {
      error = push(error, error_range);
    }
    if (digitalRead == LOW) {
      int correction = (int)(average(error, error_range));
      output_setpoint += correction;
      analogWrite(DAC0, output_setpoint);
    }
  }
}
