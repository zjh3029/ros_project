#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


using namespace std;
using namespace boost::asio;


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
        vel_char[5] = 0xf4;//速度,低八位
        vel_char[6] = 0x10;//高八位
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



int main(int argc, char** argv)
{       
        
   // set_base("/dev/ttyUSB0",0,0,-30);
  //  usleep(1000000);
    set_base("/dev/ttyUSB0",100,0,0);
    
    return 0;
}