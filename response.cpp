#include "response.hpp"
#include <errno.h>

Response::Response() : status_code(200), location_header("")
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

std::string Response::get_response_string(void)
{
	return (this->response_string);
}

void Response::set_response_string(std::string res)
{
	this->response_string = res;
}

void Response::set_default_error_page(int sc)
{
	this->set_status_code(sc);
	std::stringstream html;

	html << "<h1 style='color:red'>Error ";
	html << this->get_status_code();
	html << " :-(</h1>";
	html << "<p>this is the default error page!!!</p>";
	// Transfer it to a std::string to send
	this->set_response_string(html.str());

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
			if (check == "NOPERMISSIONS" || check == "DIR")
				this->set_default_error_page(403);
			else if (check == "NOTFOUND")
				this->set_default_error_page(404);
			else
				this->get_string_from_path(check);
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
			this->get_string_from_path(full_path_with_index);
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
		std::stringstream html;
		DIR *dir;
		struct dirent *ent;
		std::string path;
		path = this->getFullPathLocation();

		if ((dir = opendir (path.c_str())) != NULL)
		{
			/* print all the files and directories within directory */
			while ((ent = readdir (dir)) != NULL)
			{
				std::string file(std::string(ent->d_name));
				std::string uri = this->server_id.geturi();
				size_t pos;
	
				pos = uri.find("/");
				if (pos != std::string::npos && pos != 0)
				{
					if (uri[pos + 1] == '/')
						uri =  this->server_id.geturi().substr(pos + 1, uri.length());
				}
				else if (pos == 0 && uri[pos + 1] == 0)
					uri = "";
				file = "/" + file;
				html << "\t\t<p><a href=\"http://" + this->get_server_id().host + ":" <<\
        			this->get_server_id().port << uri +  file + "\">" +  file + "</a></p>\n";
			}
			closedir (dir);
			this->set_response_string(html.str());

		}
		else
		{
			/* could not open directory */
			perror ("");
			return EXIT_FAILURE;
		}

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
			if (it->second.host == "localhost")
				it->second.host = "127.0.0.1";
			if (it->second.host == it2->get_listen().host && it->second.port == it2->get_listen().port)
			{
				this->set_config_id(*it2);
				this->set_server_id(it->second);
				break;
			}
			else if (it->second.host == it2->get_servername() && it->second.port == it2->get_listen().port)
			{
				this->set_config_id(*it2);
				this->set_server_id(it->second);
				break;
			}

		}
	}
}

Request &Response::get_server_id(void)
{
	return (this->server_id);
}

void Response::set_server_id(Request & id)
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
std::string Response::get_location_header(void)
{
	return (this->location_header);
}

void Response::set_location_header(std::string l)
{
	this->location_header = l;
}

void Response::redirection(int status_code, std::string url)
{
	set_status_code(status_code);
	std::string new_url;
	size_t pos;
	pos = url.find(";");
	if (pos != std::string::npos)
		new_url = url.substr(0, pos);
	set_location_header(new_url);	
}

std::string Response::which_traitment(s_route r)
{
	if (r.redirect_status_code != 0)
	{
		if (r.redirect_url != "")
			return "Redirection";
	}
	return "Root";
}

void Response::find_Path(void)
{
	std::string location_full_path;
	std::string uri;
	std::string check;
	int flag = 0;
	int def = 0;
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
			if (this->which_traitment(i->second) == "Redirection")
				this->redirection(i->second.redirect_status_code, i->second.redirect_url);
			else
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
						this->get_string_from_path(check);
				
						//this->setFullPathLocation(check);
				}
				else
					this->find_error_page(405, conf.get_error_pages());
			}
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
				return ("NOPERMISSIONS");
			else
				return (path);	
		}
	}
	//error
	return ("NOTFOUND");
}

void Response::get_string_from_path(std::string path)
{
	std::ifstream grab_content;
	std::stringstream make_content;
	Config conf = get_config_id();
	size_t n = path.find_last_of(".");
	std::string ext = path.substr(n);
	std::map<std::string, s_route> routes = conf.get_routes();
	if (n != std::string::npos && routes.find(ext) != routes.end())
	{
		std::stringstream tmp;
		std::string hea;
		bool t = 0;
		cgi c(this->get_server_id(), routes[ext], path);
		grab_content.open(c.get_output());
		tmp << grab_content.rdbuf();
		while (std::getline(tmp, hea))
		{
			hea += "\n";
			if (t == 1)
				make_content << hea;
			else if (t == 0)
			{
				size_t pos = hea.find(":");
				if (pos != std::string::npos)
				{
					std::string name = hea.substr(0, pos);
					std::string value = hea.substr(pos + 2, hea.length() - (name.length() + 3));
					this->response_headers.insert(std::pair<std::string, std::string>(name, value));
				}
			}
			if (hea == "\r\n")
				t = 1;
		}
	}
	else {
		grab_content.open(path);
		make_content << grab_content.rdbuf();
	}

	std::string finished_content;
	finished_content = make_content.str();

	this->set_response_string(finished_content);
}

int Response::make_response(int client_socket, Request req)
{
	this->which_config(client_socket);
	if (req.check_all_keys() == false)
		this->find_error_page(400, this->get_config_id().get_error_pages());
	else
		this->find_Path();
	
	// Transfer the whole HTML to a std::string
	std::string location;
	location = this->getFullPathLocation();

	std::string finished_content;
	finished_content = this->get_response_string();
	// Create the HTTP Response
	std::stringstream make_response;
	make_response << "HTTP/1.1 " << this->get_status_code() << "\r\n";
	make_response << "Cache-Control: no-cache, private\r\n";
	if (this->response_headers.find("Content-type") != this->response_headers.end())
		make_response << "Content-Type: " << this->response_headers["Content-type"] << "\r\n";
	else
		make_response << "Content-Type: " << req.getRetCntType() << "\r\n";
	make_response << "Content-Length: " << finished_content.length() << "\r\n";
	if (this->get_location_header() != "")
		make_response << "Location: " << this->get_location_header() << "\r\n";
	make_response << "\r\n";
	make_response << finished_content;
	// Transfer it to a std::string to send
	std::string finished_response = make_response.str();
	// Send the HTTP Response
	
	int send_left;
	int send_rc;
	const char *message_ptr = finished_response.c_str();
	send_left = finished_response.length();

	while (send_left > 0)
	{
		send_rc = send(client_socket, message_ptr, send_left, 0);
		if (send_rc == -1)
			break;

		send_left -= send_rc;
		message_ptr += send_rc;
		usleep(200);
	}
	
	return (1);
}
