#include "ros/ros.h"

#include "std_msgs/String.h"
#include "std_srvs/Empty.h"

#ifndef KCL_interface_prediction
#define KCL_interface_prediction

/**
 * This file contains an interface between the knowledge base and prediction services.
 */
namespace KCL_rosplan {

	class RPPredictionInterface
	{
	private:

		ros::NodeHandle* node_handle;

		/* params */
        std::string default_file_path;
        std::string kb_node_name;

	public:

		RPPredictionInterface(ros::NodeHandle& nh);
		virtual ~RPPredictionInterface();

		bool writeToFile(std::string filePath);
		bool writeToFileServerDefault(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res);
	};

} // close namespace

#endif
