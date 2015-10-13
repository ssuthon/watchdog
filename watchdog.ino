#define ON_DURATION_THRESHOLD  100
#define OFF_DURATION_THRESHOLD 100
#define OFF_FAILSAFE_DURATION_THRESHOLD 4*1000 //4 seconds
#define MA_DURATION 30*60*1000  //30 mins
#define MA_DURATION_WHEN_RESET 60000  //1 min
#define ALARM_THRESHOLD 8*1000  //8 seconds
#define PIN_LED 1
#define PIN_AUDIO_PHYSICAL 2
#define PIN_AUDIO_ANALOG 1  //physical pin = 2
#define PIN_RELAY 4
#define PIN_SWITCH 5

void onMonitorAlarm();
void onReset();
void showEvent(int);

unsigned long ma_duration_end = 0;
unsigned short ma_state = 0;


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(PIN_LED, OUTPUT); //LED on Model B
  pinMode(PIN_AUDIO_PHYSICAL, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);  //digital  
}

short sound_on = 0;
short sound_changed = 0;
unsigned long current_stamp = 0;
unsigned long first_detected = 0;
unsigned long last_detected = 0;
unsigned long last_decision = 0;
unsigned long ma_monitor_tick = 0;

short is_detected = 0;

// the loop routine runs over and over again forever:
void loop() {
  current_stamp = millis();
  is_detected = (analogRead(PIN_AUDIO_ANALOG) * (5/1023.0)) > 0.1; 
  if(is_detected){    
    if(first_detected == 0){
      first_detected = current_stamp;
    }
    last_detected = current_stamp;    
  }
  sound_changed = 0;
  
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
      if(ma_duration_end > 0){
        ma_monitor_tick ++;
      }
      
      showEvent(ma_state ? 2 : 1);
      
  }

  if(digitalRead(PIN_SWITCH) == HIGH){    
    /*if((ma_duration_end - current_stamp) < MA_DURATION * 0.1){
      ma_duration_end =  current_stamp + MA_DURATION;  
      ma_monitor_tick = 0;     
    }*/
  }
  
  if(ma_monitor_tick > 5){
    ma_duration_end = 0;    
  }
  
  ma_state = (ma_duration_end > current_stamp); 
    
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
  showEvent(30);
  ma_duration_end = current_stamp + MA_DURATION_WHEN_RESET;  
  digitalWrite(PIN_RELAY, HIGH);
  last_decision = current_stamp;
  ma_monitor_tick = 0;
}

void showEvent(int times){
  for(int i = 0; i < times; i++){
      digitalWrite(PIN_LED, HIGH); 
      delay(50);
      digitalWrite(PIN_LED, LOW); 
      delay(50);
  }
}

