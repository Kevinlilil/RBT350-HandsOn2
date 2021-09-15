#include "Arduino.h"
#include "C610Bus.h"

long last_command = 0; // To keep track of when we last commanded the motors
C610Bus<CAN2> bus; // Initialize the Teensy's CAN bus to talk to the motors

int LOOP_DELAY_MILLIS = 2; // Wait for 0.002s between motor updates.

const float m1_offset = 0.0;
const float m2_offset = 0.0;

// Step 5. Implement your own PD controller here.
float pd_control(float pos, float vel, float target, float Kp, float Kd) {
  // return 0.0; // YOUR CODE HERE
  return Kp*(target-pos) + Kd*(-vel);
}

// This code waits for the user to type s before executing code.
void setup()
{
  while(!Serial.available()){}
  Serial.println("Press s to start.");
  while(true){
    char c = Serial.read();
    if(c=='s'){
      Serial.println("Starting code.");
      break;
    }
  }
}
void loop()
{
    bus.PollCAN(); // Check for messages from the motors.

    long now = millis();

    if(Serial.available()) {
      if(Serial.read() == 's') {
        bus.CommandTorques(0, 0, 0, 0, C610Subbus::kIDZeroToThree);
        Serial.println("Stopping.");
        while(true){}
      }
    }

    if (now - last_command >= LOOP_DELAY_MILLIS)
    {
        float m0_pos = bus.Get(0).Position(); // Get the shaft position of motor 0 in radians.
        float m0_vel = bus.Get(0).Velocity(); // Get the shaft velocity of motor 0 in radians/sec.
        Serial.print("m0_pos: ");
        Serial.print(m0_pos);
        Serial.print("\tm0_vel: ");
        Serial.print(m0_vel);

        float m0_current = 0.0;
        float m1_current = 0.0;
        float m2_current = 0.0;

        // Step 4. Uncomment for bang-bang control
        // if(m0_pos < 0) {
        //   m0_current = 800;
        // } else {
        //   m0_current = -800;
        // }

        // Step 8. Change the target position to something periodic
        // float time = millis() / 1000.0; // millis() returns the time in milliseconds since start of program

        // Step 5. Your PD controller is run here.
        float Kp = 1000.0;
        float Kd = 100.0;
        float target_position = 0.;
        m0_current = pd_control(m0_pos, m0_vel, target_position, Kp, Kd);

        // Step 10. Program periodic motion for all three motors.

        // Step 9. Program PID control for the two other motors.
        float m1_pos = bus.Get(1).Position();
        float m1_vel = bus.Get(1).Velocity();
        float m2_pos = bus.Get(2).Position();
        float m2_vel = bus.Get(2).Velocity();
        Serial.print("\tm1_pos: ");
        Serial.print(m1_pos);
        Serial.print("\tm1_vel: ");
        Serial.print(m1_vel);
        Serial.print("\tm2_pos: ");
        Serial.print(m2_pos);
        Serial.print("\tm2_vel: ");
        Serial.print(m2_vel);
        // m1_current = YOUR PID CODE
        // m2_current = pid

        // This next line will cause the motor to turn. Make sure it is mounted safely. 
        // Only call this function once per loop! Calling it multiple times will override the last command.
        bus.CommandTorques(m0_current, m1_current, m2_current, 0, C610Subbus::kIDZeroToThree);

        last_command = now;
        Serial.println();
    }
}