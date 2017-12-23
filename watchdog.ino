
#define MA_DURATION 1800000  //30 mins
#define MA_DURATION_WHEN_RESET 120000  //2 min
#define ALARM_THRESHOLD 10000  //10 seconds

#define ON_DURATION_THRESHOLD  100
#define OFF_DURATION_THRESHOLD 200
#define CHANGE_DURATION_THRESHOLD 200
#define OFF_FAILSAFE_DURATION_THRESHOLD 4000 //4 seconds
#define PIN_LED 13
#define PIN_DIGITAL 14
#define PIN_RELAY 7
#define PIN_SWITCH 4

void onMonitorAlarm();
void onReset();
void showEvent(int);

unsigned long ma_duration_end = 0;
unsigned short ma_state = 0;

short sound_on = 0;
short sound_changed = 0;
unsigned long current_stamp = 0;
unsigned long first_detected = 0;
unsigned long last_detected = 0;
unsigned long last_decision = 0;
unsigned long ma_monitor_tick = 0;

int is_detected = 0;



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(PIN_LED, OUTPUT); //LED on Model B
  pinMode(PIN_DIGITAL, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);  //digital  

  digitalWrite(PIN_RELAY, HIGH);

  //start with MA mode
  ma_duration_end =  millis() + MA_DURATION_WHEN_RESET;  
  ma_monitor_tick = 0;
  ma_state = 1;
}

// the loop routine runs over and over again forever:
void loop() {
  current_stamp = millis();
  is_detected = digitalRead(PIN_DIGITAL); 
  if(is_detected){    
    if(first_detected == 0){
      first_detected = current_stamp;
    }
    last_detected = current_stamp;    
  }
  sound_changed = 0;
  
  if((current_stamp - last_decision) < CHANGE_DURATION_THRESHOLD){
    return;
  }
  
  if((last_detected - first_detected) > ON_DURATION_THRESHOLD && first_detected != 0){
      sound_changed = sound_on != 1;
      sound_on = 1;
            
  }
  if((current_stamp - last_detected) > OFF_DURATION_THRESHOLD /*||
           current_stamp - last_decision > OFF_FAILSAFE_DURATION_THRESHOLD*/) {
      sound_changed = sound_on != 0;
      sound_on = 0;      
  }
  
  

  if(sound_changed){ 
      first_detected = 0;      
      last_decision = current_stamp;      
      /*if(sound_on){
          digitalWrite(PIN_LED, HIGH);           
      }else{    
          digitalWrite(PIN_LED, LOW);
      }*/
      if(ma_state){
        ma_monitor_tick ++;
      }
      
      showEvent(ma_state ? 2 : 1);
      
  }

  if(digitalRead(PIN_SWITCH) == HIGH){    
    if((ma_duration_end - current_stamp) < MA_DURATION * 0.1){
      ma_duration_end =  current_stamp + MA_DURATION;  
      ma_monitor_tick = 0;    
      showEvent(4); 
    }
  }
  
  if(ma_monitor_tick > 5){
    ma_duration_end = 0;    
  }
  
  ma_state = (ma_duration_end > current_stamp) ? 1 : 0; 
    
  if((current_stamp - last_decision) > ALARM_THRESHOLD){
    onMonitorAlarm();
  }
}


void onMonitorAlarm(){  
  if(ma_state){
    //ignore
  }else{    
    onReset();
  }
}

void onReset(){
  showEvent(10);
  ma_duration_end = current_stamp + MA_DURATION_WHEN_RESET;  
  digitalWrite(PIN_RELAY, LOW);
  delay(2000);
  digitalWrite(PIN_RELAY, HIGH);
  last_decision = current_stamp;
  ma_monitor_tick = 0;
}

void showEvent(int times){
  for(int i = 0; i < times; i++){
      digitalWrite(PIN_LED, HIGH); 
      delay(100);
      digitalWrite(PIN_LED, LOW); 
      delay(50);
  }
}

