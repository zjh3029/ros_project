#include <ros/ros.h>
#include "std_msgs/String.h"
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_datatypes.h>
#include <boost/thread.hpp>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

struct POSE
{
  double X;
  double Y;
  double Z;
  double or_x;
  double or_y;
  double or_z;
  double or_w;
};
POSE pose2 = {-8.15833854675, 3.15512728691, 0.0,  0.0, 0.0, -0.740479961141, 0.672078438241};
POSE pose1 = {-0.484616458416, 2.13149046898, 0.0,  0.0, 0.0, -0.749884700297, 0.661568542375};

void spinThread(){
  ros::spin();
}


void pass_voice(std::string input)
{
    ros::NodeHandle nh;//创建一个句柄，并自动初始化  
    ros::Publisher pub = nh.advertise<std_msgs::String>("/voice_msg",1000); //将std_msgs::String类型的消息发布给话题chatter，参数1000是消息队列的长度  
    ros::Rate rate(10);//指定自循环的频率  1/10  
    int count=0;
     while(count<20)  //当按下Ctrl+c命令时，返回false  
    {  
        std_msgs::String msg;  
        msg.data =input;  
        std::cout<<"I heard:  "<<input<<std::endl;
        pub.publish(msg);  
        ros::spinOnce();  
        rate.sleep();  
        ++count;  
    }  
    return;
}


void setGoal(POSE pose)
{
  MoveBaseClient ac("move_base");//give some time for connections to register
  sleep(2.0);

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 2 meters forward
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  goal.target_pose.pose.position.x = pose.X;
  goal.target_pose.pose.position.y = pose.Y;
  goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(M_PI);
  ROS_INFO("Sending goal");
  ac.sendGoal(goal);
  ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    {ROS_INFO("You have reached the goal!");
    pass_voice("arrived");}
  else
    {ROS_INFO("The base failed to move!!!");
    pass_voice("failed");}
    return;
}


void Callback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
  std::string str=msg->data.c_str();
  if (str=="bedroom")
  {
    setGoal(pose1);
  }
  return;
}

int main(int argc, char** argv){
  ros::init(argc, argv, "setgoal");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("/nav_setgoal", 1000, Callback);

  //boost::thread spin_thread = boost::thread(boost::bind(&spinThread));
  ros::spin();
  return 0;
}