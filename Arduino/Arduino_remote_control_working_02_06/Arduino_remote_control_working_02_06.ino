// Updated to work with the new drivers using 4 pwm pins. 02/06/2019
// Confirmed movement in all directions with C# remote control application
// Servo - Version: Latest
#include <Servo.h>

/*
  Code for controlling the platform for snow robot project
*/
//defining pins for motor driver
//motor A1 and 2 - belt left side
#define enA 43
#define in1 6
#define in2 7
//motor B1 and 2 - belt right side
#define enB 42
#define in3 4
#define in4 5


Servo snowBlower;
Servo snowblowerMotor;
Servo chuteAngle;
Servo chuteRotation;
Servo brushAngle;
Servo brushRotation;

int brushRot = 90;
char incomingByte;
char part;
char function;
int value = -1;
char inData[5];
byte index = 0;


void setup()
{
    //motors
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    
    //servos
    snowBlower.attach(36);
	snowBlower.write(55);
    chuteAngle.attach(33);
    chuteAngle.write(90);   // asd
    chuteRotation.attach(32);
    chuteRotation.write(92); // asd


    snowblowerMotor.attach(37);

    Serial.begin(9600);
    Serial.println("Hello world!");
    //Serial.flush();
}

/**
   Functions for movement of snowblower
   *PWM is mapped to account for the motor drivers reverse PWM signal*
**/
void goForward(int pwm)
{
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
  analogWrite(in1, pwm);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, pwm);
  
}

void goBackward(int pwm)
{
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
  analogWrite(in1, 0);
  analogWrite(in2, pwm);
  analogWrite(in3, pwm);
  analogWrite(in4, 0);
  
}

void fullStop()
{
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);

}

void turnLeft(int pwm)
{
  digitalWrite(enA, HIGH); //HIGH
  digitalWrite(enB, HIGH); // HIGH
  analogWrite(in1, 0);
  analogWrite(in2, pwm); // pwm
  analogWrite(in3, 0);
  analogWrite(in4, pwm); // pwm
  

  

}

void turnRight(int pwm)
{
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
  analogWrite(in1, pwm);
  analogWrite(in2, 0);
  analogWrite(in3, pwm);
  analogWrite(in4, 0);
      //int pwm2 = map(pwm, 0, 255, 255, 0);




}

/**
   Functions for Snowblower
**/

void stopSnowblower()
{ 
    runSnowblower(0);
    //int sbangle = map(45, 0, 100, 0, 180);
    //snowblowerMotor.write(sbangle);
    //delay(500);
}

void runSnowblower(int motorSpeed)
{
    int sbangle;
    
    for (int setmotorSpeed = 0; setmotorSpeed <= motorSpeed; setmotorSpeed += 5) {
            sbangle = map(setmotorSpeed, 0, 100, 90, 180);
            snowblowerMotor.write(sbangle);
            delay(200);
    }
   // }
    //}
}

//Max Angle is currently 45, but could be set to less, depending on how far it should be possible to lift it.
void liftSnowblower(int angle)
{
    snowBlower.write(angle);
    //if (angle <= 45) {
    //    snowBlower.write(angle);
    //}
    //else if (angle > 45) {
    //    snowBlower.write(45);
    //}
    delay(5);
}

void resetSnowblowerLift()
{
    snowBlower.write(0);
    delay(5);
}

//0 to 180
void setChuteAngle(int angle)
{
    chuteAngle.write(angle);
    delay(5);
}
//0 to 180
void setChuteRotation(int angle)
{
    chuteRotation.write(angle);
  //  Serial.println("angle recieved");
  //  Serial.println(angle);
    delay(5);
}

void areset()
{
    for (int i; i > 5; i++) {
        inData[i] = 0;
        delay(1);
    }
    value = -1;
    index = 0;
    delay(5);
    Serial.println("reset() finished");
}

void reset() {}


void loop()
{
    //Receive message from COM port
    //Decode the message and assign value for variable of the switch cases
    if (Serial.available() > 0) {
        Serial.print("Serial: ");
        Serial.println(Serial.available());
        if (index > 2) {
            value = Serial.parseInt();
        }
        else if (index < 2) {
            inData[index] = Serial.read();
        }
        //Serial.println("Serial Read:");
        //    Serial.println(inData[index]);
        //    Serial.println(index);
        index++;
        part = inData[0];
        function = inData[1];
      
    }
//    Serial.print("Part: ");
//        Serial.println(part);
//        Serial.print("Func: ");
//        Serial.println(function);
//        Serial.print("Value: ");
//        Serial.println(value);
    delay(25);
    // (part)
    //case default stand still
    /*
     MOVEMENT
    */

    if (part == 'm' && value >= 0) {

        //    Serial.println("function:");
        //    Serial.println(function);
        //    Serial.println("value:");
        //    Serial.println(value);
        //    Serial.println("part:");
        //    Serial.println(part);

        switch (function) {

        case 'f':
            goForward(value);
            Serial.println("Go forward");
            //Serial.println(value);
            reset();
            //delay(500); // Necessary? Go forward for how long?
           // fullStop(0);
            break;

        //turn left
        case 'l':
            turnLeft(value);
            Serial.println("Go left");
            reset();
            break;

        //case turn right
        case 'r':
            turnRight(value);
            Serial.println("Go right");
            reset();
            break;

        //case reverse
        case 'b':
            goBackward(value);
            Serial.println("Go back");
            reset();
            break;

        //case reverse
        case 's':
            fullStop();
            Serial.println("Stop");
            //areset();
            break;

        default:
            fullStop();
            Serial.println("function:");
            Serial.println(function);
            Serial.println("value:");
            Serial.println(value);
            Serial.println("part:");
            Serial.println(part);
            Serial.println("Case - default");
            //areset();
            break;
        }
        areset();
    }

    /*
     SNOWBLOWER
    */
    else if (part == 's' && value >= 0) {
        switch (function) {
        //case turn off snowblower off

        case 's':
            stopSnowblower();
            Serial.println("Stop SB");
            reset();
            break;

        //case on - motor C
        case 'r':
            runSnowblower(value);
            Serial.println("Run SB");
            reset();
            break;

        //case lift
        case 'l':
            liftSnowblower(value);
            Serial.println("Lift SB");
            reset();
            break;

        //reset lift
        case 'R':
            resetSnowblowerLift();
            Serial.println("Reset SB");
            reset();
            break;

        //case direction (left-right) shute
        case 'd':
            setChuteRotation(value);
            Serial.println("Set Rot");
            //areset();   
            break;
        //case angle (up-down) shute

        case 'a':
            setChuteAngle(value);
            Serial.println("Set Angl");
            //areset();
            break;

        default:
            Serial.println("Case - default");
            reset();
            //stopSnowblower(0);
            break;
        }
        areset();
    }

}

