#pragma once
#include "request.hpp"

class Response
{
private:

public:
	Response(void);
	~Response();
	int make_response(int client_socket, Request req);
};

