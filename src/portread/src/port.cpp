#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>


using namespace std;
using namespace boost::asio;

#define M_PI 3.14159265358979323846

 double X_speed;
 double Y_speed;
 double Z_speed;
 

//调用示例：set_base("/dev/ttyUSB0",1020,0,0);
void set_base(string open_port,short int vel,short int angle,short int angular)//速度，角度，角速度
{
        //底盘的协议
        unsigned char vel_char[12];
	unsigned char sum=0;
        
        vel_char[0] = 0x5a;
        vel_char[1] = 0x5a;//ID
        vel_char[2] = 0x03;
        vel_char[3] = 0x08;
        vel_char[4] = 0x01;
        vel_char[5] = vel&0xff;//速度,低八位
        vel_char[6] = vel>>8;//高八位
        vel_char[7] = angle&0xff;//角度
        vel_char[8] = angle>>8;
        vel_char[9] = angular&0xff;//角速度
        vel_char[10] =angular>>8;

	for (size_t i = 2; i < 11; i++)//不加开头
	{
	        sum += vel_char[i];
	}
	vel_char[11] = sum;

        //串口的初始化及设置
        io_service iosev;
	serial_port sp(iosev, open_port);
	sp.set_option(serial_port::baud_rate(115200));
	sp.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp.set_option(serial_port::parity(serial_port::parity::none));
	sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp.set_option(serial_port::character_size(8));
	
	write(sp, buffer(vel_char, 12));//写入数据
	iosev.run();
}


void cmd_velCallback(const geometry_msgs::Twist &twist_aux)
{
    geometry_msgs::Twist twist = twist_aux;
    
    X_speed = twist_aux.linear.x;//前进速度
    if (X_speed>0.7){X_speed=0.7;}
    
    Y_speed = twist_aux.linear.y;//未用到
    Z_speed = twist_aux.angular.z;//旋转速度，即时角速度
    if ((Z_speed*230/M_PI)>100)
    {
         Z_speed=twist_aux.angular.z;   
    }
    
        cout<<"X_speed:"<<X_speed<<endl;

    set_base("/dev/ttyUSB0",X_speed*1000,0,Z_speed*240/M_PI);//控制底盘运动
}



int main(int argc, char** argv)
{       
        //定义并初始化
        ros::init(argc, argv, "port");
        ros::NodeHandle nd;
        ros::Publisher odom_pub=nd.advertise<nav_msgs::Odometry>("odom",50);
        tf::TransformBroadcaster odom_broadcaster;
        ros::Subscriber cmd_vel_sub = nd.subscribe("cmd_vel", 5, cmd_velCallback);

        double x = 0.0;
        double y = 0.0;
        double th = 0.0;

        double vx = 0.0;
        double vy = 0.0;
        double vth = 0.0;

        ros::Time current_time=ros::Time::now();
        ros::Time last_time = ros::Time::now();

        ros::Rate r(10);

        while (nd.ok())
        {
                ros::spinOnce();
                current_time=ros::Time::now();
                double dt = (current_time - last_time).toSec();//时间差

                vx=X_speed;//更新的速度值
                vy=Y_speed;
                vth=Z_speed;

                double delta_x = (vx * cos(th) - vy * sin(th)) * dt;
                double delta_y = (vx * sin(th) + vy * cos(th)) * dt;
                double delta_th = vth * dt;

                x += delta_x;
                y += delta_y;
                th += delta_th;

                geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
                geometry_msgs::TransformStamped odom_trans;
                odom_trans.header.stamp = current_time;
                odom_trans.header.frame_id = "odom";
                odom_trans.child_frame_id = "base_link";//注意可能是base_footprint

                odom_trans.transform.translation.x = x;
                odom_trans.transform.translation.y = y;
                odom_trans.transform.translation.z = 0.0;
                odom_trans.transform.rotation = odom_quat;

                odom_broadcaster.sendTransform(odom_trans);

                nav_msgs::Odometry odom;
                odom.header.stamp = current_time;
                odom.header.frame_id = "odom";

                odom.pose.pose.position.x = x;
                odom.pose.pose.position.y = y;
                odom.pose.pose.position.z = 0.0;
                odom.pose.pose.orientation = odom_quat;

                odom.child_frame_id = "base_link";

                odom.twist.twist.linear.x = vx;
                odom.twist.twist.linear.y = vy;
                odom.twist.twist.angular.z = vth;
                odom_pub.publish(odom);
                last_time = current_time;

                r.sleep();
        }
	return 0;
}