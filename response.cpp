#include "response.hpp"

Response::Response()
{
}

Response::~Response()
{
}

std::string &Response::getFullPathLocation(void)
{
	return (this->location);
}

void Response::setFullPathLocation(std::string l)
{
	this->location = l;
}

int Response::make_response(int client_socket, Request req)
{
	// Transfer the whole HTML to a std::string
	std::string filename;
	filename = req.getMap().at("uri");
	//std::cout << filename << "\n";
	std::ifstream grab_content("test2" + filename);
	std::stringstream make_content;
	make_content << grab_content.rdbuf();
	std::string finished_content;
	finished_content = make_content.str();

	// Create the HTTP Response
	std::stringstream make_response;
	make_response << "HTTP/1.1 200 OK\r\n";
	make_response << "Cache-Control: no-cache, private\r\n";
	make_response << "Content-Type: " << req.getRetCntType() << "\r\n";
	make_response << "Content-Length: " << finished_content.length() << "\r\n";
	make_response << "\r\n";
	make_response << finished_content;
	// Transfer it to a std::string to send
	std::string finished_response = make_response.str();

	// Send the HTTP Response
	if (send(client_socket, finished_response.c_str(), finished_response.length(), 0) < 0)
	{
          perror("Send failed");
          exit(EXIT_FAILURE);
    }
	
	
	return (1);
}