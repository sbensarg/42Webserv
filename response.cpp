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
void Response::set_default_error_page(int sc)
{
	this->set_status_code(sc);
	// Create the HTTP for default error page
	std::string path = "/Users/sbensarg/Desktop/www/error_pages/.default_error_page.html";
	std::ofstream html(path);
	if(!html)
    {
        std::cerr<<"Cannot open the default file."<<std::endl;
        return ;
    }
	html << "<h1 style='color:red'>Error ";
	html << this->get_status_code();
	html << " :-(</h1>";
	html << "<p>this is the default error page!!!</p>";
	html.close();
	// Transfer it to a std::string to send
	this->setFullPathLocation(path);

}
void Response::find_location_error(std::string location)
{
	// todo : FIND LOCATION OF ERRPR PAGE
	Request id = this->get_server_id();
	Config conf = this->get_config_id();
	std::string location_full_path;
	std::string check;
	std::map<std::string, s_route>::iterator i = conf.get_routes().begin();
	while (i != conf.get_routes().end())
	{
		std::string last_path = i->first;
		std::string first_path = i->second.root;
		if (location == last_path)
		{
			location_full_path = first_path + location;
			this->setFullPathLocation(location_full_path);
			check = this->checkLocation(this->getFullPathLocation());
			if (check == "NOPERMISSIONS")
				this->set_default_error_page(403);
			else if (check == "NOTFOUND")
				this->set_default_error_page(404);
			else
				this->setFullPathLocation(check);
			return ;
		}			
		i++;
	}
	this->set_default_error_page(404);
}

void Response::find_error_page(int sc, std::map<int, std::string> err_pages)
{
	this->set_status_code(sc);
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
	set_default_error_page(this->get_status_code());
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

int Response::get_status_code(void)
{
	return (this->status_code);
}

void Response::set_status_code(int s)
{
	this->status_code = s;
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
		std::string path_autoindex = "/Users/sbensarg/Desktop/www/.autoindex.html";
		std::ofstream html(path_autoindex);
		if(!html)
		{
			std::cerr<<"Cannot open the default file."<<std::endl;
			return 1;
		}

		DIR *dir;
		struct dirent *ent;
		std::string path;
		path = this->getFullPathLocation();
		std::cout << "path " << path << "\n";

		if ((dir = opendir (path.c_str())) != NULL)
		{
			/* print all the files and directories within directory */
			while ((ent = readdir (dir)) != NULL)
			{
				std::string file(std::string(ent->d_name));
				 if (file[0] != '/' || !(this->server_id.geturi().find_last_of("/")))
        			file = "/" + file;
				html << "\t\t<p><a href=\"http://" + this->server_id.host + ":" <<\
        			this->server_id.port << this->server_id.geturi() +  file + "\">" +  file + "</a></p>\n";
				std::cout << "files => " << ent->d_name << "\n";
			}
			closedir (dir);
			this->setFullPathLocation(path_autoindex);
		}
		else
		{
			/* could not open directory */
			perror ("");
			return EXIT_FAILURE;
		}
		//std::cout << "AUTOINDEX ON";
		return (1);
	}
	return (0);
}

void Response::which_config(int fd)
{
	std::map<int, Request>::iterator it;

	it = Cluster::getInstance().requests.find(fd);
	if (it != Cluster::getInstance().requests.end())
	{
		for(std::vector<Config>::iterator it2 = Cluster::getInstance().configs.begin(); it2 != Cluster::getInstance().configs.end(); it2++)
		{
			if (it->second.host == it2->get_listen().host && it->second.port == it2->get_listen().port)
			{
				this->set_config_id(*it2);
				this->set_server_id(it->second);
				break;
			}
		}
	}
}

Request Response::get_server_id(void)
{
	return (this->server_id);
}

void Response::set_server_id(Request id)
{
	this->server_id = id;
}

Config Response::get_config_id(void)
{
	return (this->config_id);
}

void Response::set_config_id(Config id)
{
	this->config_id = id;
}

void Response::find_Path(void)
{
	std::string location_full_path;
	std::string uri;
	std::string check;
	int flag = 0;
	int ret_index = 0;
	Request id = this->get_server_id();
	Config conf = this->get_config_id();

	uri = id.geturi(); 
	std::map<std::string, s_route>::iterator i = conf.get_routes().begin();
	int j = conf.get_routes().size() - 1;
	while (i != conf.get_routes().end())
	{
		std::string last_path = i->first;
		std::string first_path = i->second.root;
		if (uri == last_path)
		{

			flag = this->check_allowed_method(i->second.allowed_methods, id.getmethod());
			if (flag == 1)
			{
				last_path = ret_new_location(last_path, 1);
				location_full_path = first_path + last_path + id.geturi();
				std::cout << "location_full_path " << location_full_path << "\n"; 
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
								this->find_error_page(404, conf.get_error_pages());
						}
						else if (ret_index == 2)
							this->find_error_page(403, conf.get_error_pages());
					}
					else
					{
						if (this->check_autoindex(i->second.autoindex) == 0)
							this->find_error_page(404, conf.get_error_pages());
					}
				}
				else if (check == "NOPERMISSIONS")
					this->find_error_page(403, conf.get_error_pages());
				else if (check == "NOTFOUND")
					this->find_error_page(404, conf.get_error_pages());
				else
					this->setFullPathLocation(check);
			}
			else
				this->find_error_page(405, conf.get_error_pages());
			break;
		}
		i++;
		if (j == 0)
		{
			i = conf.get_routes().begin();
			j = conf.get_routes().size();
			uri = ret_new_location(uri, 0);
		}
		j--;		
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

	this->which_config(client_socket);
	this->find_Path();
	// Transfer the whole HTML to a std::string
	std::string location;
	location = this->getFullPathLocation();
	std::cout << "location" << location << "\n"; 
	std::ifstream grab_content(location);
	std::stringstream make_content;
	make_content << grab_content.rdbuf();
	std::string finished_content;
	finished_content = make_content.str();

	// Create the HTTP Response
	std::stringstream make_response;
	make_response << "HTTP/1.1 " << this->get_status_code() << "\r\n";
	make_response << "Cache-Control: no-cache, private\r\n";
	make_response << "Content-Type: " << req.getRetCntType() << "\r\n";
	make_response << "Content-Length: " << finished_content.length() << "\r\n";
	make_response << "\r\n";
	make_response << finished_content;
	// Transfer it to a std::string to send
	std::string finished_response = make_response.str();

	// Send the HTTP Response
	if (send(client_socket, finished_response.c_str(), finished_response.length(), 0) <= 0)
	{
          perror("Send failed");
          exit(EXIT_FAILURE);
    }
	
	
	return (1);
}