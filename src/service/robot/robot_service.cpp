#include <iostream>
#include "robot_service.h"

namespace robot
{
    namespace service
    {
        oatpp::Object<RobotDTO> RobotService::getRobotStatus()
        {   auto robot = oatpp::Object<RobotDTO>::createShared();
            robot->robot_id   = 1;
            robot->robot_name = "Donkey";
            //robot->is_online  = 0;

            return robot;
        }
    }
}