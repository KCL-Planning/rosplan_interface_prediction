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
        node_handle->getParam("kb_node_name", kb_node_name);

        std::stringstream ss;

        ss << "/" << kb_node_name << "/domain/name";
        domain_name_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/domain/types";
        domain_type_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/domain/predicates";
        domain_predicate_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/domain/operators";
        domain_operator_list_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/domain/operator_details";
        domain_operator_details_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/domain/functions";
        domain_function_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/state/instances";
        state_instance_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/state/propositions";
        state_proposition_service = ss.str();
        ss.str("");

        ss << "/" << kb_node_name << "/state/functions";
        state_function_service = ss.str();
        ss.str("");
   

        // setting default file path
        ros::ServiceClient getDomainNameClient = node_handle->serviceClient<rosplan_knowledge_msgs::GetDomainNameService>(domain_name_service);
        rosplan_knowledge_msgs::GetDomainNameService nameSrv;
        default_file_path = "rosplan_knowledge_base";
        if (!getDomainNameClient.call(nameSrv)) {
            ROS_ERROR("KCL: (%s) Failed to call service %s; output file will take default name.", ros::this_node::getName().c_str(), domain_name_service.c_str());
        } else {
            ss << nameSrv.response.domain_name << "_prediction_table.csv";
            default_file_path = ss.str();
        }
        node_handle->getParam("output_file_path", default_file_path);
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

        ros::ServiceClient getTypesClient = node_handle->serviceClient<rosplan_knowledge_msgs::GetDomainTypeService>(domain_type_service);
        ros::ServiceClient getInstancesClient = node_handle->serviceClient<rosplan_knowledge_msgs::GetInstanceService>(state_instance_service);
        ros::ServiceClient getDomainPropsClient = node_handle->serviceClient<rosplan_knowledge_msgs::GetDomainAttributeService>(domain_predicate_service);
        ros::ServiceClient getPropsClient = node_handle->serviceClient<rosplan_knowledge_msgs::GetAttributeService>(state_proposition_service);

        // get types
        rosplan_knowledge_msgs::GetDomainTypeService typeSrv;
        if (!getTypesClient.call(typeSrv)) {
            ROS_ERROR("KCL: (%s) Failed to call service %s", ros::this_node::getName().c_str(), domain_type_service.c_str());
            return false;
        }

        // store instances of each type
        for(size_t t=0; t<typeSrv.response.types.size(); t++) {

            rosplan_knowledge_msgs::GetInstanceService instanceSrv;
            instanceSrv.request.type_name = typeSrv.response.types[t];

            if (!getInstancesClient.call(instanceSrv)) {
                ROS_ERROR("KCL: (%s) Failed to call service %s: %s",
                        ros::this_node::getName().c_str(),
                        state_instance_service.c_str(),
                        instanceSrv.request.type_name.c_str());
                return false;
            } else {
                type_object_map[typeSrv.response.types[t]];
                type_object_map[typeSrv.response.types[t]] = instanceSrv.response.instances;
                instance_list.insert(std::end(instance_list), std::begin(instanceSrv.response.instances), std::end(instanceSrv.response.instances));
            }
        }

        // fetch all predicates
        rosplan_knowledge_msgs::GetDomainAttributeService predSrv;
        if (!getDomainPropsClient.call(predSrv)) {
            ROS_ERROR("KCL: (%s) Failed to call service %s", ros::this_node::getName().c_str(), domain_predicate_service.c_str());
            return false;
        }

        for(size_t p=0; p<predSrv.response.items.size(); p++) {

            // at the moment we only care about binary predicates
            if(predSrv.response.items[p].typed_parameters.size()!=2) continue;

            predicate_list.push_back(predSrv.response.items[p].name);

            // fetch and store all true propositions of binary predicate
            rosplan_knowledge_msgs::GetAttributeService propSrv;
            propSrv.request.predicate_name = predSrv.response.items[p].name;
            if (!getPropsClient.call(propSrv)) {
                ROS_ERROR("KCL: (%s) Failed to call service %s", ros::this_node::getName().c_str(), state_proposition_service.c_str());
                return false;
            }

            for(size_t i=0; i<propSrv.response.attributes.size(); i++) {
                std::pair<std::string, std::string> pair = std::make_pair(propSrv.response.attributes[i].values[0].value, propSrv.response.attributes[i].values[1].value);
                binary_proposition_map[pair];
                binary_proposition_map[pair].push_back(predSrv.response.items[p].name);
            }
        }

        ROS_INFO("KCL: (%s) Writing to file: %s", ros::this_node::getName().c_str(), filePath.c_str());

        std::ofstream output_file;
        output_file.open(filePath);

        // print header line
        output_file << "'object1','object2'";
        std::vector<std::string>::iterator pit = predicate_list.begin();
        for(; pit != predicate_list.end(); pit++) {
            output_file << "," << (*pit);
        }
        output_file << std::endl;

        // loop through instance pairs
        std::vector<std::string>::iterator o1 = instance_list.begin();
        for(; o1 != instance_list.end(); o1++) {
            std::vector<std::string>::iterator o2 = instance_list.begin();
            for(; o2 != instance_list.end(); o2++) {

                // print object names
                output_file << "'" << (*o1) << "','" << (*o2) <<"'";

                // loop through predicates
                std::pair<std::string, std::string> pair = std::make_pair((*o1),(*o2));
                pit = predicate_list.begin();
                for(; pit != predicate_list.end(); pit++) {
                    // check if proposition is true and print result
                    std::vector<std::string>::iterator fit = std::find(binary_proposition_map[pair].begin(), binary_proposition_map[pair].end(), (*pit));
                    if(fit == binary_proposition_map[pair].end()) {
                        output_file << ",0";
                    } else {
                        output_file << ",1";
                    }
                }

                output_file << std::endl;
            }
        }

        output_file.close();
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
