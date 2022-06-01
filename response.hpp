#pragma once
#include "request.hpp"
#include "cluster.hpp"
#include "cgi.hpp"

// class Cluster;
// class Request;

class Response 
{
private:
	std::string location;
	int status_code;
	Request server_id;
	Config config_id;
	std::string location_header;
	std::string response_string;
	std::map<std::string, std::string> response_headers;
public:
	Response(void);
	~Response();
	void get_string_from_path(std::string path);
	std::string get_response_string(void);
	void set_response_string(std::string res);
	// define which type of location (redirection, cgi or root)
	std::string which_traitment(s_route r);
	// redirection
	void redirection(int status_code, std::string url);
	std::string get_location_header(void);
	void set_location_header(std::string l);
	//find which server the request should be routed to
	void which_config(int fd);
	//get server_id
	Request &get_server_id(void);
	void set_server_id(Request id);
	//get config_id
	Config get_config_id(void);
	void set_config_id(Config id);
	//find path from uri
	void find_Path();
	//set default error page
	void set_default_error_page(int sc);
	// set status code
	int get_status_code(void);
	void set_status_code(int s);
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
	void find_error_page(int sc, std::map<int, std::string> err_pages);
	int make_response(int client_socket, Request req);
};

