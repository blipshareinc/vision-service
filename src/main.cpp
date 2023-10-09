 //#include <fmt/core.h>
#include <iostream>

#include <oatpp/network/Server.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>

#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp-swagger/Controller.hpp>

#include "web/app_component.hpp"
#include "controller/controller.hpp"
#include "controller/robot_controller.hpp"

#include "processor/avprocessor.h"

#include <stdio.h>
auto start_stream() -> void
{
    std::cout<< "Stream url: " << VS1_STREAM_URL << std::endl;
    auto proc = vision_service::AVProcessor();
    if (!proc.start(VS1_STREAM_URL))
    {
        std::cout << "Failed to start the stream" << std::endl;
    }
    else
    {
        std::cout << "Successfully started the stream" << std::endl;
    }
}

auto run() -> void
{
    // register components
    vision_service::AppComponent components;

    // get the router
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    // create swagger document end point controller
    oatpp::web::server::api::Endpoints doc_end_points;

    // create controller and all of its endpoints to router
    auto controller = std::make_shared<vision_service::RESTApiController>();
    auto robot_controller = std::make_shared<robot::RobotController>();

    doc_end_points.append(router->addController(controller)->getEndpoints());
    doc_end_points.append(router->addController(robot_controller)->getEndpoints());

    router->addController(oatpp::swagger::Controller::createShared(doc_end_points));

    // get Http connection handler
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, conn_handler);

    // get TCP connection provider
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, conn_provider);

    // create server which takes provided TCP connections and passes them to Http connection handler
    auto server = oatpp::network::Server(conn_provider, conn_handler);

    // start processing the stream
    start_stream();

    // print server info
    OATPP_LOGI("VisionService", "Server running on port %s", conn_provider->getProperty("port").getData());

    // run the server
    server.run();
}

 /*
 * @brief Starting point of the application
 * 
 * @return int: 0 if successful. Else error code.
 */
int main(int argc, char* argv[])
{
    // initialize oatpp environment
    oatpp::base::Environment::init();

    // run the app
    run();

    // destrop oatpp environment
    oatpp::base::Environment::destroy();

    return 0;
}