#include "recongfile.h"
#include "tts.h"
#include "tuling.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include<initializer_list> 
#include <stdlib.h>
bool findstr_and(std::string str_org,std::initializer_list<std::string> lst,std::string str_tts)
{
	
	for (auto i = lst.begin(); i != lst.end();i++)
	{
		if (str_org.find(*i) == str_org.npos)
		{
			return false;
		}
	}
    std::cout<<"o_o:  "<<"\033[34;4m"<<str_tts<<"\033[0m"<<std::endl;
    tts(str_tts.c_str());
	return true;
}

bool findstr_or(std::string str_org, std::initializer_list<std::string> lst,std::string str_tts)
{

	for (auto i = lst.begin(); i != lst.end(); i++)
	{
		if (str_org.find(*i) != str_org.npos)
		{
            tts(str_tts.c_str());
            std::cout<<"o_o:  "<<"\033[34;4m"<<str_tts<<"\033[0m"<<std::endl;
			return true;
		}
	}
	return false;
}

void pub_topic(std::string input)
{
    ros::NodeHandle nh;//创建一个句柄，并自动初始化  
  
    ros::Publisher pub = nh.advertise<std_msgs::String>("/voice",1000); //将std_msgs::String类型的消息发布给话题chatter，参数1000是消息队列的长度  
  
    ros::Rate rate(10);//指定自循环的频率  1/10  
    int count = 0;  
  
    while(count<10)  //当按下Ctrl+c命令时，返回false  
    {  
        std_msgs::String msg;  
        //std::stringstream ss;  
        //ss<< input;  
        msg.data =input;  
        std::cout<<"I heard:  "<<input<<std::endl;
        //ROS_INFO("%s",msg.data.c_str());  
        pub.publish(msg);  
        ros::spinOnce();  
        rate.sleep();  
        ++count;  
    }  
    return;
}

void msg_Callback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
  std::string str=msg->data.c_str();
  if (str=="arrived")
  {
      tts("我已经抵达目标点了");
  }
   if (str=="failed")
  {
      tts("对不起，我没有找到要去的地方在哪！");
  }
  return;
}


int main(int argc, char *argv[])
{
    const char* login_params= "appid = 5aa8ea00, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
    MSPLogout(); 
    MSPLogin(NULL, NULL, login_params);//第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://open.voicecloud.cn注册获取
    //const char* start = "您好，我是机器人小宝，很高兴认识您！请多多指教！";
    //tts(start);

    //ros的内容
    ros::init(argc, argv, "talkall");
    ros::NodeHandle n;
    // 告诉master需要订阅chatter topic消息
   
    while (1)
    {
        ros::Subscriber sub = n.subscribe("/voice_msg", 1000, msg_Callback);//有了while循环就不用再用spin()函数了
        system("clear");
        system("play /home/zjh/catkin_ws/src/talk_command/start.wav");
        std::string get_result=recong();
        if(get_result.empty()){continue;}
        if(get_result.size()<8){continue;}
        if(findstr_and(get_result,{"什么","名字"},"我是机器人小宝")) {continue;}
        if(findstr_and(get_result,{"今年","多大"},"我的年龄是个秘密哦！")) {continue;}
        if(findstr_and(get_result,{"喜欢","什么"},"我喜欢运动哦！")) {continue;}

        if(findstr_and(get_result,{"你好"},"好的，请您让一下")) 
        {
            pub_topic("move_forward");
             continue;
        }
        if(findstr_and(get_result,{"停"},"好的！")) 
         {
            pub_topic("move_stop");
             continue;
        }    
         if(findstr_and(get_result,{"厨房"},"好的！")) 
         {
            pub_topic("bedroom");
             continue;
        }      

        if(findstr_and(get_result,{"退出"},"好的，那么我就要退出了！")) {break;}
        tts(tuling(get_result).c_str());
    }
    MSPLogout(); //退出登录
    return 0;
}