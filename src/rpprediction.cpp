#include "rosplan_interface_prediction/rpprediction.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <string>
#include <streambuf>

namespace KCL_rosplan {

    /*-------------*/
    /* constructor */
    /*-------------*/

    RPPredictionInterface::RPPredictionInterface(ros::NodeHandle& nh)
    {
        node_handle = &nh;

        // connecting to KB
        kb_node_name = "rosplan_knowledge_base";
        node_handle->getParam("knowledge_base", kb_node_name);
    }
    
    RPPredictionInterface::~RPPredictionInterface()
    {

    }

    /*----------------------*/
    /* Prediction interface */
    /*----------------------*/

    /**
     * read from KB and write csv to file
     */
    bool RPPredictionInterface::writeToFileServerDefault(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res) {
        // defaults
        return writeToFile(default_file_path);
    }

    bool RPPredictionInterface::writeToFile(std::string filePath) {
        ROS_INFO("KCL: (%s) Writing to file: %s", ros::this_node::getName().c_str(), filePath.c_str());
        return true;
    }

} // close namespace

    /*-------------*/
    /* Main method */
    /*-------------*/

    int main(int argc, char **argv) {

        srand (static_cast <unsigned> (time(0)));

        ros::init(argc,argv,"rosplan_interface_prediction");
        ros::NodeHandle nh("~");

        KCL_rosplan::RPPredictionInterface rpp(nh);
        
        // start the planning services
        ros::ServiceServer service1 = nh.advertiseService("prediction_interface_server", &KCL_rosplan::RPPredictionInterface::writeToFileServerDefault, &rpp);

        ROS_INFO("KCL: (%s) Ready to receive", ros::this_node::getName().c_str());
        ros::spin();

        return 0;
    }
