#include "AConfig.h"
#if(HAS_2THRUSTERSWITHFINS)
#include "Device.h"
#include "Pin.h"
#include "ThrustersAndFins2XTrailing2.h"
#include "Settings.h"
#include "Motor.h"
#include "Timer.h"
#include "openrov_servo.h"

Motor port_motor = Motor(PORT_MOTOR_PIN);
Motor starboard_motor = Motor(STARBOARD_MOTOR_PIN);
Servo port_fin;
Servo starboard_fin;

// These are the current and target settings for the servo's controling fins and motors
int new_p = MIDPOINT;
int new_s = MIDPOINT;
flotat new_p_fin = MIDPOINT;
float new_s_fin = MIDPOINT;
int p = MIDPOINT;
int s = MIDPOINT;
float p_fin = MIDPOINT;
float s_fin = MIDPOINT;

// These are the target orientations that ultimately will drive target servo settings
float trg_throttle,trg_yaw,trg_pitch,trg_roll;

// These are the target control surace setings before 2nd order adustment of roll and yaw
int trg_motor_power;
float trg_fin_pitch;

Timer controltime;
Timer thrusterOutput;

#ifdef ESCPOWER_PIN
bool canPowerESCs = true;
Pin escpower("escpower", ESCPOWER_PIN, escpower.digital, escpower.out);
#else
boolean canPowerESCs = false;
#endif

float smoothAdjustedServoPositionPercentage(float target, float current){
  return target; //intentional disable
  float x = target - current;
  int sign = (x>0) - (x<0);
  float adjustedVal = current + sign * (min(abs(target - current),  Settings::smoothingIncriment/100.0));
  return (adjustedVal);
}

void Thrusters::device_setup(){
  port_motor.reset();
  starboard_motor.reset();
  thrusterOutput.reset();
  controltime.reset();
  port_fin.attach(PORT_FIN_PIN);
  starboard_fin.attach(STARBOARD_FIN_PIN);


  #ifdef ESCPOWER_PIN
    escpower.reset();
    escpower.write(1); //Turn on the ESCs
  #endif
}

void Thrusters::device_loop(Command command){
  if (command.cmp("mtrmod")) {
      port_motor.motor_positive_modifer = command.args[1]/100;
      starboard_motor.motor_positive_modifer = command.args[3]/100;
      port_motor.motor_negative_modifer = command.args[4]/100;
      starboard_motor.motor_negative_modifer = command.args[6]/100;
  }
  if (command.cmp("rmtrmod")) {
      Serial.print(F("mtrmod:"));
      Serial.print(port_motor.motor_positive_modifer);
      Serial.print (",");
      Serial.print(0);
      Serial.print (",");
      Serial.print(starboard_motor.motor_positive_modifer);
      Serial.print (",");
      Serial.print(port_motor.motor_negative_modifer);
      Serial.print (",");
      Serial.print(0);
      Serial.print (",");
      Serial.print(starboard_motor.motor_negative_modifer);
      Serial.println (";");
  }

  if (command.cmp("port")) {
      //ignore corrupt data
      if (command.args[1]>999 && command.args[1]<2001) {
        p = command.args[1];
      }
  }

  if (command.cmp("starboard")) {
      //ignore corrupt data
      if (command.args[1]>999 && command.args[1]<2001) {
        s = command.args[1];
      }
  }

if (command.cmp("portfin")) {
    //ignore corrupt data
    if (command.args[1]>999 && command.args[1]<2001) {
      p_fin = command.args[1];
    }
}

if (command.cmp("starboardfin")) {
    //ignore corrupt data
    if (command.args[1]>999 && command.args[1]<2001) {
      s_fin = command.args[1];
    }
}

  if (command.cmp("thro") || command.cmp("yaw")){
    if (command.cmp("thro")){
      if (command.args[1]>=-100 && command.args[1]<=100) {
        trg_throttle = command.args[1]/100.0;
      }
    }
    // The code below spreads the throttle spectrum over the possible range
    // of the motor. Not sure this belongs here or should be placed with
    // deadzon calculation in the motor code.
    if (trg_throttle>=0){
      p = 1500 + (500/abs(port_motor.motor_positive_modifer))*trg_throttle;
      s = p;
    } else {
      p = 1500 + (500/abs(port_motor.motor_negative_modifer))*trg_throttle;
      s = p;
    }
    trg_motor_power = s;

    if (command.cmp("yaw")) {
        //ignore corrupt data
        if (command.args[1]>=-100 && command.args[1]<=100) { //percent of max turn
          trg_yaw = command.args[1]/100.0;
          int turn = trg_yaw*250; //max range due to reverse range
          int sign=0;
          sign = (turn>0) - (turn<0);
          if (trg_throttle >=0){
            int offset = (abs(turn)+trg_motor_power)-2000;
            if (offset < 0) offset = 0;
            p = trg_motor_power+turn-offset;
            s = trg_motor_power-turn-offset;
          } else {
            int offset = 1000-(trg_motor_power-abs(turn));
            if (offset < 0) offset = 0;
            p = trg_motor_power+turn+offset;
            s = trg_motor_power-turn+offset;
          }


        }
    }

  }

if (command.cmp("pitch") || command.cmp("roll")){
  if (command.cmp("pitch")){
    if (command.args[1]>=-100 && command.args[1]<=100) {
      trg_pitch = command.args[1]/100.0;
    }
  }
  // set the targets for control services (fin) based on pitch target
  s_fin = trg_pitch;
  p_fin = trg_pitch;
  trg_fin_pitch = trg_pitch;

  if (command.cmp("roll")) {
      //ignore corrupt data
      if (command.args[1]>=-100 && command.args[1]<=100) { //percent of max turn
        trg_roll = command.args[1]/100.0;
        //actual roll needs to be bounded by the amount of response left
        //in the control surface after the pitch command.  IE, for max roll
        //the pilot needs to have zero pitch.  This is a point of contention
        //and we could change it to simply drive the controls to max deflection
        //independently and ignore the intended roll/pitch outcome as independent
        //controls.
        float remaining_roll = 1 - abs(trg_pitch);

        if (trg_roll >=0) {
          if (trg_fin_pitch > 0) {
            trg_roll = min(remaining_roll,trg_roll);
          }
        } else {
          if (trg_fin_pitch  < 0) {
            trg_roll = max(-remaining_roll,trg_roll);
          }
        }

        p_fin = trg_fin_pitch + trg_roll;
        s_fin = trg_fin_pitch - trg_roll;

      }
  }

}
  #ifdef ESCPOWER_PIN
    else if (command.cmp("escp")) {
      escpower.write(command.args[1]); //Turn on the ESCs
      Serial.print(F("log:escpower="));
      Serial.print(command.args[1]);
      Serial.println(';');
    }
  #endif
    else if (command.cmp("start")) {
      port_motor.reset();
      starboard_motor.reset();
    }
    else if (command.cmp("stop")) {
      port_motor.stop();
      starboard_motor.stop();
    }

  //to reduce AMP spikes, smooth large power adjustments out. This incirmentally adjusts the motors and servo
  //to their new positions in increments.  The incriment should eventually be adjustable from the cockpit so that
  //the pilot could have more aggressive response profiles for the ROV.
  if (controltime.elapsed (50)) {
    if (p_fin!=new_p_fin || s_fin!=new_s_fin) {
      new_p_fin = smoothAdjustedServoPositionPercentage(p_fin,new_p_fin);
      new_s_fin = smoothAdjustedServoPositionPercentage(s_fin,new_s_fin);

      //Probably should move the raw servo behind a fin abstraction.  Especially if we need a bias to get
      //to neutral fin deflection.
      port_fin.writeMicroseconds(constrain(MIDPOINT+(500*new_p_fin),1000,2000));
      starboard_fin.writeMicroseconds(constrain(MIDPOINT+500*new_s_fin,1000,2000));
    }

    //intentionally removed smoothing the thrust adjustments as that is handled inside the ESCs
    if (p!=new_p || s!=new_s) {
      new_p = p;
      new_s = s;
      port_motor.goms(new_p);
      starboard_motor.goms(new_s);
    }

  }

  navdata::FTHR = map((new_p + new_s) / 2, 1000,2000,-100,100);

  //The output from the motors is unique to the thruster configuration
  if (thrusterOutput.elapsed(1000)){
    Serial.print(F("motors:"));
    Serial.print(new_p);
    Serial.print(',');
    Serial.print(new_s);
    Serial.println(';');
    Serial.print(F("elevons:"));
    Serial.print(port_fin.readMicroseconds());
    Serial.print(',');
    Serial.print(starboard_fin.readMicroseconds());
    Serial.println(';');

    Serial.print(F("mtarg:"));
    Serial.print(p);
    Serial.print(',');
    Serial.print(s);
    Serial.println(';');
    Serial.print(F("elevtarg:"));
    Serial.print(p_fin);
    Serial.print(',');
    Serial.print(s_fin);
    Serial.println(';');

  }
}
#endif
