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
		int substr_number = 0;
		cout << gpsStr << endl;
		string substr;
		stringstream ss(gpsStr);
		sensor_msgs::NavSatFix gps_message;
		while(ss.good()){
			string substr;
			getline(ss, substr, ',' );
			substr_number++;
			switch(substr_number) {
;					cout << "timestamp: " << setprecision(10) << stod(substr) << endl;
					gps_message.header.stamp = ros::Time(stod(substr));
					gps_message.header.seq = num_iterations;
					gps_message.status.service = 1;
					break;
				case 2:
					cout << "num_satellites: " << setprecision(10) << stod(substr) << endl;
					break;
				case 3:
					cout << "latitude: " << setprecision(10) << stod(substr) << endl;
					gps_message.latitude = stod(substr);
					break;
				case 4:
					cout << "longitude: " << setprecision(10) << stod(substr) << endl;
					gps_message.longitude = stod(substr);
					break;
				case 5:
					cout << "altitude: " << setprecision(10) << stod(substr) << endl;
					gps_message.altitude = stod(substr);
					break;
				case 6:
					cout << "latitude_sigma: " << setprecision(10) << stod(substr) << endl;
					gps_message.position_covariance[0] = stod(substr)*stod(substr);
					break;
				case 7:
					cout << "longitude_sigma: " << setprecision(10) <<  stod(substr) << endl;
					gps_message.position_covariance[4] = stod(substr)*stod(substr);
					break;
				case 8:
					cout << "altitude_sigma: " <<  setprecision(10) <<  stod(substr) << endl;
					gps_message.position_covariance[8] = stod(substr)*stod(substr);
					break;
				default:
					break;
			}
		}
		num_iterations++;
		cout << num_iterations << endl;
		//bag.write("fix", gps_message.header.stamp, gps_message);
	}
	
// Images
	std::vector<std::string> filenames;
	std::experimental::filesystem::path path("/home/thesidjway/datasets/RobotCar/2014-06-26-08-53-56/stereo/centre");
	const stdfs::directory_iterator end{};
	for( stdfs::directory_iterator iter{path} ; iter != end ; ++iter ) {
		if(stdfs::is_regular_file(*iter)) {
			cv::Mat image = cv::imread(iter->path().string(), CV_8UC1);
			sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "mono8", image).toImageMsg();
			cout << iter->path().filename() << endl;
			//bag.write("centre", ros::Time((iter->path().filename().path().string())), msg);
		}
	}
	bag.close();
}