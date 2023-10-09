#pragma once

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/core/macro/component.hpp"

#include "web/dto/robot_dto.hpp"

namespace robot
{
    namespace service
    {
        class RobotService
        {
        public:
            oatpp::Object<RobotDTO> getRobotStatus();

        private:
            typedef oatpp::web::protocol::http::Status Status;
        };
    }
}