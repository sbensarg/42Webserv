#include "response.hpp"

Response::Response() : status_code(0)
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

void Response::find_location_error(std::string location)
{
	// todo : FIND LOCATION OF ERRPR PAGE
}

void Response::setStatusCode(int sc, std::map<int, std::string> err_pages)
{
	//this->status_code = sc;
	std::map<int, std::string>::iterator it = err_pages.begin();

	while ( it != err_pages.end())
	{
		if (it->first == sc)
		{
			this->find_location_error(it->second);
			return ;
		}
		it++;
	}
}

int & Response::getStatusCode(void)
{
	return this->status_code;
}

int Response::check_allowed_method(std::vector<std::string> list, std::string method)
{
	std::vector<std::string>::iterator it = list.begin();
	
	while(it != list.end())
	{
		if (*it == method)
			return (1);
		it++;
	}
	return (0);
}

std::string ret_new_location(std::string path, int flag)
{
	size_t pos = 0;
	std::string newpath;
	pos = path.find_last_of("/");
	if (pos != std::string::npos && pos != 0)
	{
		newpath = path.substr(0, pos);
	}
	else if (pos == 0 && flag == 0)
		newpath = "/";
	return (newpath);
}

int Response::display_index(std::vector<std::string> list)
{
	std::vector<std::string>::iterator it = list.begin();
	std::string full_path_with_index;
	std::string ret;
	while(it != list.end())
	{
		full_path_with_index = this->getFullPathLocation() + "/" + *it;
		std::cout << "full_path_with_index " << full_path_with_index << "\n";
		ret = this->checkLocation(full_path_with_index);
		if (ret == full_path_with_index)
		{
			this->setFullPathLocation(full_path_with_index);
			return (1);
		}
		else if (ret == "NOPERMISSIONS")
			return (2);
		else
			it++;
	}
	return (0);
}

int Response::check_autoindex(bool autoindex)
{
	if (autoindex == 1)
	{
		std::cout << "AUTOINDEX ON";
		return (1);
	}
	return (0);
}

void Response::find_Path(int fd)
{
	std::string location_full_path;
	std::map<int, Request>::iterator it;
	std::string uri;
	std::string check;
	int flag = 0;
	int ret_index = 0;
	it = Cluster::getInstance().requests.find(fd);
	if (it != Cluster::getInstance().requests.end())
	{
		for(std::vector<Config>::iterator it2 = Cluster::getInstance().configs.begin(); it2 != Cluster::getInstance().configs.end(); it2++)
		{
			if (it->second.host == it2->get_listen().host && it->second.port == it2->get_listen().port)
			{
				uri = it->second.geturi(); 
				std::map<std::string, s_route>::iterator i = it2->get_routes().begin();
				int j = it2->get_routes().size() - 1;
				while (i != it2->get_routes().end())
				{
					
					std::string last_path = i->first;
					std::string first_path = i->second.root;
					if (uri == last_path)
					{
						flag = this->check_allowed_method(i->second.allowed_methods, it->second.getmethod());
						if (flag == 1)
						{
							last_path = ret_new_location(last_path, 1);
							location_full_path = first_path + last_path + it->second.geturi();
							this->setFullPathLocation(location_full_path);
							check = this->checkLocation(this->getFullPathLocation());
							if (check == "DIR")
							{
								if (i->second.index.size() != 0)
								{
									ret_index = this->display_index(i->second.index);
									if (ret_index == 0)
									{
										if (this->check_autoindex(i->second.autoindex) == 0)
											this->setStatusCode(404, it2->get_error_pages());
									}
									else if (ret_index == 2)
										this->setStatusCode(403, it2->get_error_pages());
								}
								else 
									this->check_autoindex(i->second.autoindex);
							}
							else if (check == "NOPERMISSIONS")
								this->setStatusCode(403, it2->get_error_pages());
							else if (check == "NOTFOUND")
								this->setStatusCode(404, it2->get_error_pages());
							else
								this->setFullPathLocation(check);
						}
						else
						{
							std::cout << "method not allowed\n";
							this->setStatusCode(405, it2->get_error_pages());
							
						}
						break;
					}
					i++;
					if (j == 0)
					{
						i = it2->get_routes().begin();
						j = it2->get_routes().size();
						uri = ret_new_location(uri, 0);
					}
					j--;		
				}
			}	
		}
	}
}

std::string Response::checkLocation(std::string path)
{
	struct stat s;
	if( stat(path.c_str(), &s) == 0)
	{
		 if ( S_ISDIR(s.st_mode))
		 {
			// it's a directory
			return("DIR");
		} 
		else if (S_ISREG(s.st_mode))
		{
			// it's a file
			if(!(s.st_mode & S_IRWXU))
			{
				return ("NOPERMISSIONS");
			}
			else
				return (path);	
		}
	}
	//error
	return ("NOTFOUND");

}

int Response::make_response(int client_socket, Request req)
{
	this->find_Path(client_socket);
	// Transfer the whole HTML to a std::string
	//std::string filename;
	//filename = req.getMap().at("uri");
	
	std::string location;
	location = this->getFullPathLocation();
	std::cout << "filename " << location << "\n";
	std::ifstream grab_content(location);
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