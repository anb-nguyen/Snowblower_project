/*
    Eivind Moe - eivimoe@hotmail.com
    Lars Olav Brekke - larsolav94@hotmail.com
 */



#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "laserscan/LaserScanner.h"
#include "std_msgs/String.h"
#include <sstream>
#include <string>


using namespace std;

 // define states
 enum State {forward, stop, turn};


//Define subscribers and publishers
ros::Subscriber scanSubscriber;
ros::Subscriber arduinoSubscriber;

ros::Publisher arduinoPublisher;

//msgs string for arduino communication
std_msgs::String msg;

//create a string for the data
std::stringstream ss;

bool obstacleTooClose;
int turnDirection;
string prevArduinoResponse;

void scanCallback (sensor_msgs::LaserScan scanMessage);
void arduino_Callback(const std_msgs::String::ConstPtr& arduino_msg);
void avoidObstacle();
void publishToArduino(string command);


int main(int argc, char **argv){

    obstacleTooClose = false;
    State currentState = forward;
    int loopCounter = 0;

	// Initiate new ROS node named "raspberry_pi"
	ros::init(argc, argv, "raspberry_pi");
	ros::NodeHandle n;

    //create a publisher with a topic "robot_movement" that will send a String message
	arduinoPublisher = n.advertise<std_msgs::String>("robot_movement", 1);

	//subscribe to the laser scanner topic
	scanSubscriber = n.subscribe("/scan", 10, scanCallback);

    //Create a new subscriber with topic name "robot_feedback"
    arduinoSubscriber = n.subscribe("robot_feedback", 1, arduino_Callback); 

	
	//Rate is a class the is used to define frequency for a loop. Here we send a message each second.
	ros::Rate loop_rate(100); //1 message per second, must be tweaked

    while (ros::ok()) // Keep spinning loop until user presses Ctrl+C
    {
        switch (currentState)
        {
            case forward:
                    if(obstacleTooClose){
                        publishToArduino("a"); // stop all
                        currentState = turn;
                    }
		    else {
                        publishToArduino("mf_100");
                    }
                break;
            case turn:
                if(loopCounter >= 300 && !obstacleTooClose){ // loopcounter must be changed according to number of loops it takes for the robot to turn 90 degrees
                    loopCounter = 0;
                    currentState = forward;
                }
                else{
                    // Decide whether to turn right or left
                        // turn right or left
			if (turnDirection == -1) {
                    publishToArduino("ml_100"); // turn left
		}
			else if (turnDirection == 1) {
		    publishToArduino("mr_100"); // turn right
		}
                    loopCounter++;
			cout << turnDirection << endl;
                }
                break;
            default:
                break;
        }
        
        //if lidar detects object, full stop. else move forward

       ros::spinOnce(); // Need to call this function often to allow ROS to process incoming messages

      loop_rate.sleep(); // Sleep for the rest of the cycle, 
    }

   return 0;
}

void scanCallback (sensor_msgs::LaserScan scanMessage){
	//cout<<"minimum range: " <<LaserScanner::getMinimumRange(scanMessage)<<endl;
    //cout<<"maximum range: " <<LaserScanner::getMaximumRange(scanMessage)<<endl;
    //cout<<"average range: " <<LaserScanner::getAverageRange(scanMessage,0,600)<<endl;
	turnDirection = LaserScanner::isObstacleTooClose(scanMessage,0,360,0.60);
	cout << "index of minimum range: " << LaserScanner::getIndexOfMinimumRange(scanMessage) << endl;
    if (turnDirection!=0){
        obstacleTooClose = true;
    }
    else {
      obstacleTooClose = false;
    }
    // print data or variable for more information

}

void arduino_Callback(const std_msgs::String::ConstPtr& arduino_msg)
{
    prevArduinoResponse = "";
    prevArduinoResponse = arduino_msg->data.c_str();
}

void avoidObstacle(){
    publishToArduino("a");
    //implement function to turn according to angle where object is detected
    //publishToArduino("mr_90");

}

void publishToArduino(string command){
    msg.data = command;

    arduinoPublisher.publish(msg);
}

