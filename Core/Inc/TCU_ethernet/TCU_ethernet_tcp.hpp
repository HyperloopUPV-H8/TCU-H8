#pragma once
#include "TCU_ethernet/TCU_ethernet_common.hpp"

namespace ethernet{

#define TCP_CLIENT_PORT 50401
#define TCP_SERVER_PORT 50500

ServerSocket *server_connection;

void start_server_socket(){
	server_connection = new ServerSocket((string)BACKEND_IP,TCP_SERVER_PORT);
}

}
