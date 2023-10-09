#pragma once

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

namespace robot
{
    class RobotDTO : public oatpp::DTO
    {
        DTO_INIT(RobotDTO, DTO)

        DTO_FIELD(UInt32, robot_id,    "robot_id");
        DTO_FIELD(String, robot_name,  "robot_name");
       // DTO_FIELD(UInt32, is_online,   "is_online");        
    };
}

#include OATPP_CODEGEN_END(DTO)