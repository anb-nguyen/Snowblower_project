/*
  updated 10/06/19
  Snnowblower arduino code
  Eivind Moe - eivimoe@hotmail.com
  Lars Olav Brekke - larsolav94@hotmail.com 
 */

#include <Servo.h>
#include <string.h>
#include <ros.h>
#include <std_msgs/String.h>



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
char inData[7];
byte index = 0;

// Initialize ROS node & publisher
ros::NodeHandle  nh;


std_msgs::String str_msg;
std_msgs::String toggle_msg;
ros::Publisher pub("robot_feedback", &str_msg);

void messageCb( const std_msgs::String& toggle_msg){
  String temp; // Create temporary string for conversion
  temp = toggle_msg.data;  // Store string member from msg in temp string
  temp.toCharArray(inData, 7); // Convert to inData char array
  
  part = inData[0];
  function = inData[1];
 
  value = 100*(inData[3] - '0') + 10*(inData[4] - '0') + (inData[5] - '0'); // Converting rest of array to int
}

ros::Subscriber<std_msgs::String> sub("robot_movement", messageCb );

char hello[13] = "hello world!";

void setup()
{
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);


  //servos
  snowBlower.attach(36);
  snowBlower.write(90);
  chuteAngle.attach(33);
  chuteRotation.attach(32);

  snowblowerMotor.attach(37);
  
  
  nh.initNode();
  nh.advertise(pub);
  nh.subscribe(sub);
  liftSnowblower(25);
}

void loop()
{
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
        //str_msg.data = "Case - f";
        str_msg.data = inData; // to print received message directly back to rasp
        reset();
        break;

      //turn left
      case 'l':
        turnLeft(value);
        str_msg.data = "Case - l";
        reset();
        break;

      //case turn right
      case 'r':
        turnRight(value);
        str_msg.data = "Case - r";
        reset();
        break;

      //case reverse
      case 'b':
        goBackward(value);
        str_msg.data = "Case - b";
        reset();
        break;

      //case stop
      case 's':
        fullStop(0);
        str_msg.data = "Case - s";
        reset();
        break;
        
      default:
        fullStop(0);
        str_msg.data = "Case - def";
        reset();
        break;
      }
    }

 
  else if (part == 's' && value >= 0) {
    switch (function) {
      //case turn off snowblower off

      case 's':
        stopSnowblower();
        str_msg.data = "Case - s";
        reset();
        break;

      //case on - motor C
      case 'r':
        runSnowblower(value);
        str_msg.data = "Case - r";
        reset();
        break;

      //case lift
      case 'l':
        liftSnowblower(value);
        str_msg.data = "Case - l";
        reset();
        break;

      //reset lift
      case 'R':
        resetSnowblower();
        str_msg.data = "Case - R";
        reset();
        break;

      //case direction (left-right) shute
      case 'd':
        setChuteRotation(value);
        str_msg.data = "Case - d";
        //areset();

        for (int i; i > 5; i++) {
          inData[i] = 0;
          delay(1);
        }
        value = -1;
        index = 0;
        delay(5);

        break;
      //case angle (up-down) shute

      case 'a':
        setChuteAngle(value);
        str_msg.data = "Case - a";
        reset();
        break;

      default:
        str_msg.data = "Case - def";
        reset();
        //stopSnowblower(0);
        break;
    }
    
  }
  else if(part == 'a'){
        stopAll();
        str_msg.data = "case - stopAll";
        reset(); 
  }


  pub.publish(&str_msg);
  inData[0] = 'a'; // Stop if no message received from rasp
  nh.spinOnce();
}

/**
   Functions for movement of snowblower
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

void fullStop(int pwm)
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
}

/**
   Functions for Snowblower
**/

void stopSnowblower()
{
  int sbangle = map(45, 0, 100, 0, 180);
  snowblowerMotor.write(sbangle);
  delay(500);
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

void resetSnowblower()
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
  str_msg.data = "reset finished";
  pub.publish(&str_msg);
}

void reset() {}

void stopAll(){
  fullStop(0);
  delay(5);
  stopSnowblower();
  delay(5);
  setChuteRotation(0);
  delay(5);
}

