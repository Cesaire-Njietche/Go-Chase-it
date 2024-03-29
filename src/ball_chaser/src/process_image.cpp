#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if(!client.call(srv))
    {
      ROS_ERROR("Failed to call service command_robot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image &img)
{

    int white_pixel = 255, img_size, ball_pos, split = 0;
    bool found = false, left = false, middle = false, right = false;

    img_size = img.height * img.step;
    ball_pos = img.step/3; //Split the image into 3 sections: left, middle, right

    for(int i = 0; i < img_size - 2; i = i + 3)
    {
		
         if(img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
	 {
	 
    	    found = true;
    	    split = i%img.step;
		
    	    if(split >= 0 && split < ball_pos) //Drive the robot to the left
            {
                left = true;
            }                    
    	    else if(split >= ball_pos && split < 2*ball_pos){ //Drive the robot forward
                middle = true;
            }   
       	    else if(split >= 2*ball_pos){ //Drive the robot to the right
                right = true;
            }
	    break;
	 }
    }
    if (found)
    {
        if (left)
        {
            drive_robot(0.5, 0.5);
        }
        else if(middle){
            drive_robot(0.5, 0.0);
        }
        else if(right){
            drive_robot(0.5, -0.5);
        }
    }
    else 
    {
      drive_robot(0.0, 0.0);
		
    }

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
