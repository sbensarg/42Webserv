#pragma once
#include "request.hpp"
 #include "cluster.hpp"

// class Cluster;
// class Request;

class Response 
{
private:
	std::string location;
	int status_code;
public:
	Response(void);
	~Response();

	//find path from uri
	void find_Path(int fd);
	//check allowed method
	int check_allowed_method(std::vector<std::string> list, std::string method);
	// display index
	int display_index(std::vector<std::string> list);
	// check autoindex
	int check_autoindex(bool autoindex);
	//find location errors
	void find_location_error(std::string location);
	void setFullPathLocation(std::string l);
	std::string &getFullPathLocation(void);
	std::string checkLocation(std::string path);
	void setStatusCode(int sc, std::map<int, std::string> err_pages);
	int & getStatusCode(void);
	int make_response(int client_socket, Request req);
};

