#include <iostream>
#include <fstream>
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Image.h>
#include <experimental/filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace std;
namespace stdfs = std::experimental::filesystem ;

int main(int argc, char **argv) {
	ros::init(argc, argv, "listener");
	ros::NodeHandle n;
	rosbag::Bag bag;
	bag.open("test.bag", rosbag::bagmode::Write);

// GPS
	ifstream fin ("/home/thesidjway/datasets/RobotCar/2014-06-26-08-53-56/gps/gps.csv");
	string gpsStr;
	long num_iterations = 0;
	while(getline(fin, gpsStr))  {
		if(num_iterations == 0) {
			num_iterations = 1;
			continue;
		}
		int substring_number = 0;
		cout << gpsStr << endl;
		string substring;
		stringstream ss(gpsStr);
		sensor_msgs::NavSatFix gps_message;
		while(ss.good()){
			string substring;
			getline(ss, substring, ',' );
			substring_number++;
			switch(substring_number) {
				case 1:
				{
					cout << "timestamp: " << setprecision(10) << stod(substring) << endl;
					string gpssecs = substring.substr(0,10);
					string gpsnsecs = substring.substr(10, 16);
					cout << "secs: " << gpssecs << endl;
					cout << "nsecs: " << gpsnsecs << endl;
					gps_message.header.stamp = ros::Time(stod(gpssecs), stod(gpsnsecs)*1000);
					gps_message.header.seq = num_iterations;
					gps_message.status.service = 1;
					break;
				}
				case 2:
					cout << "num_satellites: " << setprecision(10) << stod(substring) << endl;
					break                 ;
				case 3:
					cout << "latitude: " << setprecision(10) << stod(substring) << endl;
					gps_message.latitude = stod(substring);
					break;
				case 4:
					cout << "longitude: " << setprecision(10) << stod(substring) << endl;
					gps_message.longitude = stod(substring);
					break;
				case 5:
					cout << "altitude: " << setprecision(10) << stod(substring) << endl;
					gps_message.altitude = stod(substring);
					break;
				case 6:
					cout << "latitude_sigma: " << setprecision(10) << stod(substring) << endl;
					gps_message.position_covariance[0] = stod(substring)*stod(substring);
					break;
				case 7:
					cout << "longitude_sigma: " << setprecision(10) <<  stod(substring) << endl;
					gps_message.position_covariance[4] = stod(substring)*stod(substring);
					break;
				case 8:
					cout << "altitude_sigma: " <<  setprecision(10) <<  stod(substring) << endl;
					gps_message.position_covariance[8] = stod(substring)*stod(substring);
					break;
				default:
					break;
			}
		}
		num_iterations++;
		cout << num_iterations << endl;
		bag.write("fix", gps_message.header.stamp, gps_message);
	}
	
// Images     
	string imgStr;
        int count = 0;
	ifstream fin2 ("/home/thesidjway/datasets/RobotCar/2014-06-26-08-53-56/stereo.timestamps");
	while(getline(fin2, imgStr))  {
		cout << "Count: " << count;
		count++;
		stringstream ss;
		double timestamp = stod(imgStr.substr(0, imgStr.length() - 2));
		ss << setprecision(20) << "/home/thesidjway/datasets/RobotCar/2014-06-26-08-53-56/stereo/left/" << timestamp <<".png";
		cout << ss.str() << endl;
		cv::Mat image = cv::imread(ss.str(), CV_8UC1);
		string timestamp_read = imgStr.substr(0, 16);
		string imgsecs = timestamp_read.substr(0, 10);
		string imgnsecs = timestamp_read.substr(10,16);
		cout << "secs: " << stod(imgsecs) << endl;
		cout << "nsecs: " << stod(imgnsecs) << endl;
		sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "mono8", image).toImageMsg();
		bag.write("left", ros::Time(stod(imgsecs), stod(imgnsecs)*1000), msg);
	}
	bag.close();
}
