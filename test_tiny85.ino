#define ON_DURATION_THRESHOLD  100
#define OFF_DURATION_THRESHOLD 100
#define OFF_FAILSAFE_DURATION_THRESHOLD 4000

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(1, OUTPUT); //LED on Model B
  pinMode(2, INPUT);
}

short sound_on = 0;
short sound_changed = 0;
unsigned long current_stamp = 0;
unsigned long first_detected = 0;
unsigned long last_detected = 0;
unsigned long last_decision = 0;
short is_detected = 0;

// the loop routine runs over and over again forever:
void loop() {
  //delay(10);              
  current_stamp = millis();
  is_detected = (analogRead(1) * (5/1023.0)) > 0.05; //PIN2 is first analog port  
  if(is_detected){
    if(first_detected == 0){
      first_detected = current_stamp;
    }
    last_detected = current_stamp;    
  }
  sound_changed = 0;
  if(last_detected - first_detected > ON_DURATION_THRESHOLD && first_detected != 0){
      sound_on = 1;
      sound_changed = 1;      
  }else if(current_stamp - last_detected > OFF_DURATION_THRESHOLD ||
           current_stamp - last_decision > OFF_FAILSAFE_DURATION_THRESHOLD) {
      sound_on = 0;
      sound_changed = 1;
  }

  if(sound_changed){
      first_detected = 0;      
      last_decision = current_stamp;
      if(sound_on){
        digitalWrite(1, HIGH);   // turn the LED on (HIGH is the voltage level)
      }else{    
        digitalWrite(1, LOW);    // turn the LED off by making the voltage LOW    
      }
  }
          
}
