cmake_minimum_required(VERSION 2.8.3)
project(rosplan_interface_prediction)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  std_srvs
  rosplan_knowledge_msgs
  rosplan_dependencies
)

find_package(FLEX REQUIRED)

find_package(Boost REQUIRED COMPONENTS
  filesystem
)

###################################
## catkin specific configuration ##
###################################

## Declare things to be passed to dependent projects
catkin_package(
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp rospy std_msgs std_srvs rosplan_knowledge_msgs
  DEPENDS
)

###########
## Build ##
###########

## include_directories(include)
include_directories(include)
include_directories(${catkin_INCLUDE_DIRS})

## Declare cpp executables
add_executable(rpprediction src/rpprediction.cpp)
add_dependencies(rpprediction ${catkin_EXPORTED_TARGETS})

## Specify libraries against which to link a library or executable target
target_link_libraries(rpprediction ${catkin_LIBRARIES})

#############
## Install ##
#############

install(TARGETS rpprediction
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
