#include "ros/ros.h"

#include <iostream>
#include <fstream>

#include "std_msgs/String.h"
#include "std_srvs/Empty.h"
#include "diagnostic_msgs/KeyValue.h"
#include "rosplan_knowledge_msgs/GetDomainNameService.h"
#include "rosplan_knowledge_msgs/GetDomainAttributeService.h"
#include "rosplan_knowledge_msgs/GetDomainTypeService.h"
#include "rosplan_knowledge_msgs/GetInstanceService.h"
#include "rosplan_knowledge_msgs/GetAttributeService.h"

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

		std::string domain_name_service;
		std::string domain_type_service;
		std::string domain_predicate_service;
		std::string domain_function_service;
		std::string domain_operator_details_service;
		std::string domain_operator_list_service;

		std::string state_instance_service;
		std::string state_proposition_service;
		std::string state_function_service;

        /* problem details */
        std::map<std::string, std::vector<std::string>> type_object_map;
        std::vector<std::string> predicate_list;
        std::vector<std::string> instance_list;
        std::map<std::pair<std::string,std::string>,std::vector<std::string>> binary_proposition_map;

	public:

		RPPredictionInterface(ros::NodeHandle& nh);
		virtual ~RPPredictionInterface();

		bool writeToFile(std::string filePath);
		bool writeToFileServerDefault(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res);
	};

} // close namespace

#endif
