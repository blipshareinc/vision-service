#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include "web/dto/status_dto.hpp"
#include "service/robot/robot_service.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace robot
{
    class RobotController : public oatpp::web::server::api::ApiController
    {
    public:
        RobotController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper))
            : oatpp::web::server::api::ApiController(object_mapper)
        {
        }

        ENDPOINT_INFO(getRobotStatus)
        {
            info->summary = "Get Robot Status";
            info->addResponse<Object<robot::RobotDTO>>(Status::CODE_200, "application/json");
            info->addResponse<Object<vision_service::StatusDTO>>(Status::CODE_400, "application/json");
            info->addResponse<Object<vision_service::StatusDTO>>(Status::CODE_500, "application/json");
        }
        ENDPOINT("GET",
                 "/robot/get_status/",
                 getRobotStatus)
        {
            return createDtoResponse(Status::CODE_200, robot_service_.getRobotStatus());
        }
    private:
        service::RobotService robot_service_;
    };
}
#include OATPP_CODEGEN_END(ApiController)