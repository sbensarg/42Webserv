#pragma once
#include "request.hpp"
class Request;
class Response
{
private:
	std::string location;
	int status_code;
public:
	Response(void);
	~Response();

	void setFullPathLocation(std::string l);
	std::string &getFullPathLocation(void);
	void setStatusCode(int sc);
	int & getStatusCode(void);
	int make_response(int client_socket, Request req);
};

