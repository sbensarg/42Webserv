#pragma once
#include "request.hpp"
class Request;
class Response
{
private:
	std::string location;
public:
	Response(void);
	~Response();
	void setFullPathLocation(std::string l);
	std::string &getFullPathLocation(void);
	int make_response(int client_socket, Request req);
};

