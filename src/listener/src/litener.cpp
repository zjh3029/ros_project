#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>

void pub_topic_move(double input_vel,double input_angular)
{
    ros::NodeHandle nh;//创建一个句柄，并自动初始化  
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",1000); //将std_msgs::String类型的消息发布给话题chatter，参数1000是消息队列的长度  
    ros::Rate rate(10);//指定自循环的频率  1/10  
    while(ros::ok())  //当按下Ctrl+c命令时，返回false  
    {  
        geometry_msgs::Twist msg;
        msg.linear.x = input_vel;
        msg.angular.z = input_angular;
        pub.publish(msg);
        ros::spinOnce();  
        rate.sleep();  
    }  
    return;
}

void nav_setgoal(std::string str)
{
    ros::NodeHandle nh;//创建一个句柄，并自动初始化  
    ros::Publisher pub = nh.advertise<std_msgs::String>("/nav_setgoal",1000); //将std_msgs::String类型的消息发布给话题chatter，参数1000是消息队列的长度  
    ros::Rate rate(10);//指定自循环的频率  1/10  
    while(ros::ok())  //当按下Ctrl+c命令时，返回false  
    {  
        std_msgs::String msg; 
        msg.data =str;
        pub.publish(msg);
        ros::spinOnce();  
        rate.sleep();  
    }  
    return;
}

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
  std::string str=msg->data.c_str();
  if (str=="move_forward")
  {
    pub_topic_move(0.2,0.2);
  }
  if (str=="move_stop")
  {
    pub_topic_move(0.0,0.0);
  }
    if (str=="bedroom")
  {
    nav_setgoal("bedroom");
  }
}

int main(int argc, char **argv)
{
 
  ros::init(argc, argv, "litener");
  ros::NodeHandle n;
  // 告诉master需要订阅chatter topic消息
  ros::Subscriber sub = n.subscribe("/voice", 1000, chatterCallback);
  ros::spin(); // 自循环
  return 0;
}