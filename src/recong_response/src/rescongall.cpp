#include "recongfile.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32.h"


int main(int argc,char **argv)
{
    //登录科大讯飞语音平台
    const char* login_params = "appid = 5aa8ea00, work_dir = .";
	MSPLogout(); 
	if (MSP_SUCCESS != MSPLogin(NULL, NULL, login_params))
	{
		std::cout << "========================" << std::endl;
		std::cout << "Login failed!" << std::endl;
		std::cout << "========================" << std::endl;
	}

    //ros的初始化部分
    ros::init(argc, argv, "rescongall");
    ros::NodeHandle nd;
    ros::Publisher pub=nd.advertise<std_msgs::String>("/voice/recong_topic",5);
    ros::Rate loop_rate(10);

    //识别的结果
    while (ros::ok())
    {
        std::string get_result=recong();
        if(get_result.empty())
        {
           continue;
        }
        std_msgs::String msg;
        msg.data=get_result;
        pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
    MSPLogout();
    return 0;
}