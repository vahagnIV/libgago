const int FL_DIGITAL_PIN1 = 2;
const int FL_DIGITAL_PIN2 = 4;
const int FL_PWN_PIN = 3;

const int FR_DIGITAL_PIN1 = 7;
const int FR_DIGITAL_PIN2 = 8;
const int FR_PWN_PIN = 5;

const int RL_DIGITAL_PIN1 = 12;
const int RL_DIGITAL_PIN2 = 13;
const int RL_PWN_PIN = 6;

const int RR_DIGITAL_PIN1 = 10;
const int RR_DIGITAL_PIN2 = 11;
const int RR_PWN_PIN = 9;


// ================= Commands ============================
const char FL_FORWARD_CMD = 9;
const char FL_STOP_CMD = 10;
const char FL_SETSPEED_CMD = 11;
const char FL_BACKWARD_CMD = 12;

const char FR_FORWARD_CMD = 13;
const char FR_STOP_CMD = 14;
const char FR_SETSPEED_CMD = 15;
const char FR_BACKWARD_CMD = 16;

const char RL_FORWARD_CMD = 17;
const char RL_STOP_CMD = 18;
const char RL_SETSPEED_CMD = 19;
const char RL_BACKWARD_CMD = 20;

const char RR_FORWARD_CMD = 21;
const char RR_STOP_CMD = 22;
const char RR_SETSPEED_CMD = 23;
const char RR_BACKWARD_CMD = 24;

const char APPLY_CMD = 25;

const char SYNC_CMD = 26;
const char RESET_CMD = 27;

enum Direction{
  FORWARD,
  BACKWARD,
  STOP
};

struct MotorState{
  MotorState(int dgl_pin1, int dgl_pin2, int anlg_pin): 
          digital_pin1(dgl_pin1), 
          digital_pin2(dgl_pin2),
          pwn_pin(anlg_pin), 
          speed(255), 
          direction(STOP){    
  }

  void ApplyState(){
    if (direction == FORWARD){      
      digitalWrite(digital_pin1, HIGH);
      digitalWrite(digital_pin2, LOW);
      analogWrite(pwn_pin, speed);
    }
    else if(direction == BACKWARD){
      digitalWrite(digital_pin1, LOW);
      digitalWrite(digital_pin2, HIGH);
      analogWrite(pwn_pin, speed);
    }
    else{
      digitalWrite(digital_pin1, LOW);
      digitalWrite(digital_pin2, LOW);
      analogWrite(pwn_pin, LOW);
    }
  }
  
  unsigned char speed;
  Direction direction; 
  const int digital_pin1;
  const int digital_pin2;
  const int pwn_pin;
};

struct State {
  State(): 
      fl(FL_DIGITAL_PIN1, FL_DIGITAL_PIN2, FL_PWN_PIN), 
      fr(FR_DIGITAL_PIN1, FR_DIGITAL_PIN2, FR_PWN_PIN), 
      rl(RL_DIGITAL_PIN1, RL_DIGITAL_PIN2, RL_PWN_PIN), 
      rr(RR_DIGITAL_PIN1, RR_DIGITAL_PIN2, RR_PWN_PIN){}
  MotorState fl;
  MotorState fr;
  MotorState rl;
  MotorState rr;

  void ApplyState(){
    fl.ApplyState();
    fr.ApplyState();
    rl.ApplyState();
    rr.ApplyState();    
  }  
};


State state;

void setup() {
  pinMode(FL_DIGITAL_PIN1, OUTPUT);
  pinMode(FL_DIGITAL_PIN2, OUTPUT);
  pinMode(FL_PWN_PIN, OUTPUT);
  pinMode(FR_DIGITAL_PIN1, OUTPUT);
  pinMode(FR_DIGITAL_PIN2, OUTPUT);
  pinMode(FR_PWN_PIN, OUTPUT);
  pinMode(RL_DIGITAL_PIN1, OUTPUT);
  pinMode(RL_DIGITAL_PIN2, OUTPUT);
  pinMode(RL_PWN_PIN, OUTPUT);
  pinMode(RR_DIGITAL_PIN1, OUTPUT);
  pinMode(RR_DIGITAL_PIN2, OUTPUT);
  pinMode(RR_PWN_PIN, OUTPUT);
  
  Serial.begin(9600);
  while (! Serial);
  state.ApplyState();
}

int read_speed(){
  while(!Serial.available())
          delay(1);
  return (unsigned char)Serial.read();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    char cmd = Serial.read();   
    switch(cmd){
      case APPLY_CMD:
        state.ApplyState();
        break;
      case FL_FORWARD_CMD:
        state.fl.direction = FORWARD;
        break;
      case FL_STOP_CMD:
        state.fl.direction = STOP;
        break;
      case FL_BACKWARD_CMD:
        state.fl.direction = BACKWARD;
        break;
      case FL_SETSPEED_CMD:
        state.fl.speed =  read_speed();        
        break;
      case RL_FORWARD_CMD:
        state.rl.direction = FORWARD;
        break;
      case RL_STOP_CMD:
        state.rl.direction = STOP;
        break;
      case RL_BACKWARD_CMD:
        state.rl.direction = BACKWARD;
        break;
      case RL_SETSPEED_CMD:        
        state.rl.speed = read_speed();
        break;
      case FR_FORWARD_CMD:
        state.fr.direction = FORWARD;
        break;
      case FR_STOP_CMD:
        state.fr.direction = STOP;
        break;
      case FR_BACKWARD_CMD:
        state.fr.direction = BACKWARD;
        break;
      case FR_SETSPEED_CMD:
        state.fr.speed =  read_speed();        
        break;
      case RR_FORWARD_CMD:
        state.rr.direction = FORWARD;
        break;
      case RR_STOP_CMD:
        state.rr.direction = STOP;
        break;
      case RR_BACKWARD_CMD:
        state.rr.direction = BACKWARD;
        break;
      case RR_SETSPEED_CMD:
        state.rr.speed =  read_speed();        
        break;
      case SYNC_CMD:
        Serial.write(1);   
        break;     
      case RESET_CMD:
        state.fl.direction = state.fr.direction = state.rl.direction = state.rr.direction = STOP;
        state.fl.speed = state.fr.speed = state.rl.speed = state.rr.speed = 255;
        state.ApplyState();
        break;
    }
  }

}
