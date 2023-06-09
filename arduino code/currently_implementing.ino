#include <vector>
#include <algorithm>
#include <array>

// Initialize variabels

// general setup
//-------------------------------------------------------------
int control_DIGITALPIN = 10;
int calibration_DIGITALPIN = 12; // Usage has all been commented
//-------------------------------------------------------------


// PID controller variable
//-------------------------------------------------------------
double control = 2046;
double p_gain = -10;
double i_gain = 0; // -0.000001;
double d_gain = 0;
double intError, rateError;  
int element;
double ADC_avg = 0;
double correction = 0;
double i_correction = 0;
double p_correction = 0;
double d_correction = 0;
unsigned long elapsedTime;
unsigned long currentTime;
unsigned long previousTime = -100000;
//-------------------------------------------------------------

// peak average algorithm variable
//-------------------------------------------------------------
int ARRAY_SIZE = 100; // do not go beyond 20000 element (rough estimate of arduino memory space)
double DUTY_CYCLE = 10;                                              ; // in percentage
int threshold_ = 10;
double Error_signal = 0;
int sampling_window;
//-------------------------------------------------------------

// serial communication variable
//-------------------------------------------------------------
const byte numChars = 10;
char receivedChars[numChars];
boolean newData = false;
String sep_string = "\n ------------------------------------------------";
//-------------------------------------------------------------

// calibration variable
//-------------------------------------------------------------
int min_setpoint = 0;
int max_setpoint = 4095;
int TTL_has_light = 0; // When TTL is HIGH and there is light, set this to 1;
                      // When TTL is TTL_has_light and there is light, set this to 0;
//-------------------------------------------------------------

// flag variable
//-------------------------------------------------------------
boolean on = true; 
boolean calibration = false; 
boolean stabilization_on = true;
//-------------------------------------------------------------




void setup() {
  Serial.begin(115200); // baud rate
  randomSeed(16); // randomSeed for testing (currently not implemented)
  pinMode(control_DIGITALPIN, INPUT);
  pinMode(calibration_DIGITALPIN, INPUT);
  pinMode(A2, INPUT); // This pin monitors the output of DAC1
  ADC->ADC_MR |= 0x80; // these lines set free running mode 
  ADC->ADC_CR = 2;
  ADC->ADC_CHER = 0xA0; // set free running mode on pin A0 and A2. 0xA0 in binary is 10100000 
                        // (for A0 alone, use 0x80, which in binary is 10000000).
                        // pin A0 correspond to channel 7, CH6: A1, CH5: A2, CH4: A3 and so on 

  analogWriteResolution(12);
  analogWrite(DAC1, control);

  setpoint = 400; // initial setpoint, can be changed; for current use, please do not go above 1000;
  Serial.print(String(micros())); // debugging
}




// joining all array element into a string for inspection
String join(int arr[], int size){
  String output;
  int count = 0;
  for (int i=0; i<size; i++){
    output += String(arr[i]);
    output += "  ";
    count++;
    if (count == 10) {output += "\n"; count = 0;}
  }
  return output;
}




// read out the ADC value
int read_adc() {
  while ((ADC->ADC_ISR & 0x80) == 0); // wait for conversion
  int a0 = ADC->ADC_CDR[7];    // read data
// Serial.println(a0);
 return a0;
}




// read out the DAC value (in voltage)
int read_DAC1() { // DAC1 is connected to A2
  while ((ADC->ADC_ISR & 0x20) == 0); // wait for conversion
  int a2 = ADC->ADC_CDR[5];    // read data
 return a2;
}




// push a string to serial that shows up on the serial monitor (can also be extracted using libraries like pyserial)
void writeString(String stringData) { // Used to serially push out a String with Serial.write()

  for (int i = 0; i < stringData.length(); i++)
  {
    Serial.write(stringData[i]);   
  }
}




// a simply bool to int function that might be redundant
int bool_to_int(boolean state){
  if (state == true){
    return 1;
  }
  else{
    return 0;
  }
}




// receive user input through serial; normally by typing command into the serial monitor in the format: <command>; for a detailed list of available commands, see next section
void rec_serial_data() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}




// A list of available commands that can be recognized by serial
void changeValues(int array[], int array2[]) {
    if (newData == true) {
		// set p gain
        if (receivedChars[0] == 'P' && receivedChars[1] == '=') {
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(2);
           p_gain = temp2.toDouble();
        }

		// set i gain
        else if (receivedChars[0] == 'I' && receivedChars[1] == '=') {
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(2);
           i_gain = temp2.toDouble();

        }

		// set d gain
        else if (receivedChars[0] == 'D' && receivedChars[1] == '=') {
           Serial.println(rateError);
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(2);
           d_gain = temp2.toDouble();
        }

		// turn on stabilization
        else if (receivedChars[0] == 'S' && receivedChars[1] == 'T') {
           stabilization_on = true;
        }

		// turn off stabilization
        else if (receivedChars[0] == 'N' && receivedChars[1] == 'S' && receivedChars[2] == 'T') {
           stabilization_on = false;
        }

		// print current sampling window (in µs)
        else if (receivedChars[0] == 'W' && receivedChars[1] == '?') {
           String temp1 = String(sampling_window);
           String toWrite = "sampling_window: " + temp1;
           writeString(toWrite);
        }

		// set sampling window (in number of ADC read, each reading takes ~2µs)
        else if (receivedChars[0] == 'W' && receivedChars[1] == '=') {
          String temp1 = String(receivedChars);
          String temp2 = temp1.substring(2);
          ARRAY_SIZE = temp2.toInt();
        }

		// set percentage of ADC readings used to calculate peak average
        else if (receivedChars[0] == 'D' && receivedChars[1] == 'C' && receivedChars[2] == '=') {
          String temp1 = String(receivedChars);
          String temp2 = temp1.substring(3);
          DUTY_CYCLE = temp2.toDouble();
        }

		// set the control value (0~4095 map to 0.5~2.7V DAC)
        else if (receivedChars[0] == 'T' && receivedChars[1] == '=') {
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(2);
           control = temp2.toDouble();
        }
        
		// print current p gain
        else if (receivedChars[0] == 'P' && receivedChars[1] == '?') {
          String p_temp = String(p_gain);
          String p_out = p_temp + "\n";
          writeString(p_out);
        }

		// print current i gain
        else if (receivedChars[0] == 'I' && receivedChars[1] == '?') {
          String i_temp = String(i_gain);
          String i_out = i_temp + "\n";
          writeString(i_out);
        }

		// print current d gain
        else if (receivedChars[0] == 'D' && receivedChars[1] == '?') {
          String d_temp = String(d_gain);
          String d_out = d_temp + "\n";
          writeString(d_out);
        }

		// continue/start the whole program
        else if (receivedChars[0] == 'B') {
          on = true;
        }        

		// pause the whole program
        else if (receivedChars[0] == 'E') {
          on = false;
        }

		// set the setpoint
        else if (receivedChars[0] == 'S'){ //receive new setpoint 
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(1);
           setpoint = temp2.toInt();     
        }

		// turn on calibration (currently not available)
        else if (receivedChars[0] == 'C') {
           calibration = true;
//           String _min = String(min_setpoint);
//           String _max = String(max_setpoint);
//           String min_max = _min + "," + _max + "\n" + "setpoint: " + String(setpoint) + "\n";
//           writeString(min_max);
        }    

		// print the current ADC peak average and one most recent adc reading
        else if (receivedChars[0] == 'A' && receivedChars[1] == 'D' && receivedChars[2] == 'C'){
           String adc_avg = String("Max: ") + String(ADC_avg) + "\n";
           String a0_value = String("A0 Reading: ") + String(float(read_adc())) + "\n";
           writeString(a0_value + adc_avg);
        }

		// print the most recent DAC reading and control value
        else if (receivedChars[0] == 'D' && receivedChars[1] == 'A' && receivedChars[2] == 'C'){
           String a2_value = String("A2 Reading: ") + String((float(read_DAC1())/4096.0*3.3 - 0.5)*4096/2.2) + "\n";
           String control_value = String("Control Value: ") + String(float(control)) + "\n";
           String toWrite = a2_value + control_value;
           writeString(toWrite);
        }
        
		// set the DAC1 output (not frequently used)
        else if (receivedChars[0] == 'D'){ // set the DAC1 output (int from 550 to 2075) 
           String temp1 = String(receivedChars);
           String temp2 = temp1.substring(1);
           int dac = temp2.toInt();
           analogWrite(DAC1, dac);
        }

		// print the current setpoint 
        else if (receivedChars[0] == 'R') { 
           String s_temp = String(setpoint);
           String s_out = s_temp + "\n"; 
           writeString(s_out);
        }

		// print min and max setpoint
        else if (receivedChars[0] == 'M'){ //return min_setpoint and max_setpoint
           String _min = String(min_setpoint);
           String _max = String(max_setpoint);
           String min_max = _min + "," + _max + "\n";
           writeString(min_max);
        }

// not sure what this part does but never use it
/*
        else if (receivedChars[0] = '?') {
          int condition = bool_to_int(on);
          String state = String(condition);
          writeString(state);
//          if (on == true){
//            String _true = "Stop";
//            String state = _true + "\n";
//            writeString(state);
//          }
//          else {
//            String _false = "Start";
//            String state = _false + "\n";
//            writeString(state);
//          }
        }
*/




		// print out lots of important stuff
        else if (receivedChars[0] == '?') {
          String s_temp = String(setpoint);
          String setpoint_out = "setpoint: " + s_temp;
          
          String adc_avg = String("adc_avg: ") + String(ADC_avg);
          String a0_value = String("current A0 Reading: ") + String(float(read_adc()));
          String adc_out = adc_avg + "   " + a0_value;

          String a2_value = String("current A2 Reading: ") + String((float(read_DAC1())/4096.0*3.3 - 0.5)*4096/2.2);
          String control_value = String("Control Value: ") + String(float(control));
          String dac_out = a2_value + "   " + control_value;

          String errSig = String("Error Signal: ") + String(Error_signal);
          String corr = String("Correction: ") + String(correction);
          String intErr = String("Int Error: ") + String(intError);
          String ratErr = String("Rate Error: ") + String(rateError);
          String p_cor = String("p_correction: ") + String(p_correction);
          String i_cor = String("i_correction: ") + String(i_correction);
          String d_cor = String("d_correction: ") + String(d_correction);

          String toWrite = sep_string + setpoint_out + "\n" + adc_out + "\n" + dac_out + 
          "\n" + corr + "   " + errSig + "   " + intErr + "   " + ratErr + "\n" + p_cor + "\n"
          + i_cor + "\n" + d_cor + "\n";

          writeString(toWrite);
          }
          



		// print out the recent ADC reading array and peaks
        else if (receivedChars[0] == '.' && receivedChars[1] == '?') {
          String adc_readings_string = sep_string +"\n adc_readings: \n" + join(array, ARRAY_SIZE);
          String adc_readings_max_string = sep_string + "\n adc_readings_max: \n" + join(array2, element); 
          String toWrite = adc_readings_string + adc_readings_max_string;
          writeString(toWrite);
        }
        
        // else if (receivedChars[0]
        
      newData = false;
  }
}




// calibrator;  not being used right now
void calibrator() {

  if (calibration == true){
    // potential issues:
    // 1. the ADC input already gets out of bound
    // 2. the input signal is being pulsed; 
    // 3. be careful that if the pulse is too fast the calibrator may not have enough time to change the DAC and sample data from ADC
    boolean step1 = true;
    boolean step2 = true;
  
    analogWrite(DAC1, 0);
    delay(5);
    
    while (step1) {
      if (digitalRead(control_DIGITALPIN) == TTL_has_light) {
        min_setpoint = read_adc();
        if (digitalRead(control_DIGITALPIN) == TTL_has_light) {step1 = false;}  
      }
    }
  
    analogWrite(DAC1, 4095);
    delay(5);
  
    while (step2) {
      if (digitalRead(control_DIGITALPIN) == TTL_has_light) {
        max_setpoint = read_adc();
        if (digitalRead(control_DIGITALPIN) == TTL_has_light) {step2 = false;}  
      }
    }
    
    setpoint = (max_setpoint + min_setpoint)/2;
    calibration = false;
    Serial.println(String(setpoint));
  }
}




// main stabilizer; adc sampling, sorting, averaging, PID loop, double-chekc
void stabilizer(int arr[], int arr2[]) {
    int n = 0;
    unsigned long t_start = micros();
    while(n < ARRAY_SIZE){
      arr[n] = read_adc();
      n++;
    }

    currentTime = micros();
    sampling_window = currentTime - t_start;
    elapsedTime = (double)(currentTime - previousTime);
    
    int temp_arr[ARRAY_SIZE];
    std::copy(arr, arr+ARRAY_SIZE, temp_arr);
    std::sort(temp_arr, temp_arr + ARRAY_SIZE, std::greater<int>());
    std::copy(temp_arr, temp_arr + element, arr2);

    for (int k=0; k<element; k++){ADC_avg += arr2[k];}
    ADC_avg = ADC_avg / element;
    if (ADC_avg < 0.005 * setpoint) {return;}  // Test if the stuck situation happes
    correction = (ADC_avg - setpoint) / ADC_avg;
// -------------------------------------
    intError += correction * elapsedTime / 1000000; // Be careful with our micros()
    rateError = (correction - lastError)/elapsedTime * 1000000;

    i_correction = i_gain*intError;
    p_correction = p_gain*correction;
    d_correction = d_gain*rateError;
    
    double sig = p_correction + i_correction + d_correction;
    //
    Error_signal = sig;
    control = control + sig;
    lastError = correction;
    previousTime = currentTime; 
    
    if (control >= 4095) 
      { control = 4095;
        }
    if (control <= 0) 
      { control = 0; 
        }

    if (stabilization_on == true) {analogWrite(DAC1, int(round(control)));}
}

  

void loop(){
// calibrator not included yet
  on = true; // Should comment this line out
  if (on == true){
    element = int(ARRAY_SIZE * DUTY_CYCLE / 100);
	// initialize the main ADC sample array outside the stabilization function so that one can call it in other functions; we attempted with vectors and std::array but they don't work as we expected.
    int adc_readings[ARRAY_SIZE];
    int adc_readings_max[element];
    stabilizer(adc_readings, adc_readings_max); 
    rec_serial_data();
    changeValues(adc_readings, adc_readings_max);
  }
}
