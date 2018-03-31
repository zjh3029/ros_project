#include <ros/ros.h>
#include "std_msgs/String.h"
#include "tts.h"

bool findstr_and(std::string str_org,std::vector<std::string> lst)
{
	for (std::vector<std::string>::iterator i = lst.begin(); i != lst.end();i++)
	{
		if (str_org.find(*i) == str_org.npos)
		{
			return false;
		}
	}
	return true;
}

bool findstr_or(std::string str_org,std::vector<std::string> lst)
{
	for (std::vector<std::string>::iterator i = lst.begin(); i != lst.end(); i++)
	{
		if (str_org.find(*i) != str_org.npos)
		{
			return true;
		}
	}
	return false;
}

 /**
 *接受/voice/xf_tts_topic话题的字符串的回调函数
 */
void TTSCallback(const std_msgs::String::ConstPtr &msg)
{
    std::string out_result=msg->data.c_str();
    std::string   str[2]={"hello", "ni"};
    std::vector<std::string> strArray(str,str+2);
}

int main(int argc, char* argv[])
{
    const char* login_params         = "appid = 5aa8ea00, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
    MSPLogin(NULL, NULL, login_params);//第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://open.voicecloud.cn注册获取

    const char* start = "宝宝启动成功了,你知道我在哪里吗？";
    tts(start);

    ros::init(argc, argv,"robotts");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("/voice/recong_topic",3,TTSCallback);

    ros::spin();
    MSPLogout(); //退出登录

    return 0;
}