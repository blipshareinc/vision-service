#pragma once

#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>
#include <oatpp/core/macro/component.hpp>

#include <oatpp-swagger/Model.hpp>
#include <oatpp-swagger/Resources.hpp>

#include <fmt/core.h>

namespace vision_service
{

/**
 * @brief Creates and holds the various application components (like swagger, database) etc
 * 
 */
class AppComponent
{
    /**
     * @brief Create connection provider
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, conn_provider)([] {
        return oatpp::network::tcp::server::ConnectionProvider::createShared(
            { SERVER_NAME, SERVER_PORT, oatpp::network::Address::IP_4 }
        );
    }());

    /**
     * @brief Create router component
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, http_router)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());

    /**
     * @brief Create connection handler
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, conn_handler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
        return oatpp::web::server::HttpConnectionHandler::createShared(router);
    }());

    /**
     * @brief Create object mapper
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, obj_mapper)([]{
        return oatpp::parser::json::mapping::ObjectMapper::createShared();
    }());

    /**
     * @brief Create Swagger API docs info
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>, swagger_doc_info)([] {
        oatpp::swagger::DocumentInfo::Builder builder;

        builder
            .setTitle("Vision Service Available Endpoints")
            .setDescription("APIs with swagger docs")
            .setVersion("1.0")
            .setContactName("BLIPShare")
            .setContactUrl("https://www.blipshare.com")
            .setLicenseName("Apache License, Version 2.0")
            .setLicenseUrl("http://www.apache.org/licenses/LICENSE-2.0")
            .addServer(fmt::format("{}:{}", SERVER_ADDRESS, SERVER_PORT), "server");

        return builder.build();
    }());

    /**
     * @brief Swagger Resources
     * 
     */
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>, swagger_resources)([] {
        // set the oatpp-swagger resources path
        return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
    }());
};

}   // namespace vision_service