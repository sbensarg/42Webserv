#include "request.hpp"

Request::Request(void) : total_size(0), data(""), cnt_size(0), header_length(0), body(""), flag(0)
{
	
}


Request::~Request()
{

}

void Request::append_data(std::string data, int size)
{ 
	int check = 0;
	int f = 0;
	if (total_size == 0)
	{
		this->data.append(data);
		f = this->data.find("\r\n\r\n");
		if (f != std::string::npos)
		{
			this->parse_request();
			this->total_size += size;
			header_length = f + 4;
			it = this->map.find("Content-Length");
			if (it != this->map.end())
				cnt_size = std::atoi(it->second.c_str());
			if (cnt_size > 0 && this->check_all_keys() == true)
			{
				if (this->getmethod() == "POST")
				{
					std::string wr = this->data.substr(header_length);
					pipe(pipes);
					write(pipes[1], wr.c_str(), wr.length());
					check = 1;
					std::cout << wr << "\n";
				}
			}
		}
		else if (total_size < cnt_size + header_length && check == 1)
		{
			std::cout << data << "\n";
			write(pipes[1], data.c_str(), size);
			total_size += size;
		}
		if (total_size == cnt_size + header_length && total_size > 0)
			request_read = true;
	}
}
		

bool Request::check_all_keys(void)
{
	if (this->check_method() == true 
			&& this->check_http_vesion() == true && this->flag == 0)
		return (true);
	return (false);
}


void Request::parse_request()
{
	size_t i;
	std::string url;
	std::string first_line;
	std::string first[3] = {"method", "uri", "ver"};

	i = 0;
	i = this->data.find_first_of("\r\n");
	if (i != std::string::npos)
	{
		first_line = this->data.substr(0,i);
		std::stringstream raw(first_line);
		std::string seg;
		int j = 0;
		while (std::getline(raw, seg, ' ') && j < 3)
		{
			if (j == 1)
				url = seg;
			else
				this->map.insert(std::pair<std::string, std::string>(first[j], seg));
			j++;
		}
		std::size_t del = url.find_first_of("?");
		if (del == std::string::npos)
			this->map.insert(std::pair<std::string, std::string>(first[1], url));
		else
		{
			this->map.insert(std::pair<std::string, std::string>(first[1], url.substr(0, del)));
			this->map.insert(std::pair<std::string, std::string>("GET_params", url.substr(del + 1)));
		}
	}
	std::string request_header;
	request_header = this->data.substr(i + 1);
	std::stringstream rh(request_header);
	std::string token;
	while (std::getline(rh, token, '\r') && token != "\n")
	{
		size_t pos = token.find(":");
		if (pos != std::string::npos)
		{
			std::string name = token.substr(1, pos - 1);
			std::string value = token.substr(pos + 2);
			this->map.insert(std::pair<std::string, std::string>(name, value));
		}
	}
	get_port();
	this->ret_cnt_Type();

	// it = this->map.find("method");
	// if (it != this->map.end())
	// {
	// 	if (it->second != "" && it->second == "POST")
	// 	{
	// 		std::size_t f = request_header.find("\r\n\r\n");
	// 		if (f != std::string::npos)
	// 		{
	// 			this->body = request_header.substr(f + 4);
	// 		}
	// 	}
	// }
	// std::cout << "body ==>" << this->body << "\n";
	// if (this->map.at("method") == "POST")
	// {
	// 	std::size_t f = request_header.find("\r\n\r\n");
	// 	if (f != std::string::npos)
	// 		{
	// 		this->body = request_header.substr(f + 4);
	// 		}
	// }
}

bool Request::check_http_vesion()
{
	it = this->map.find("ver");
	if (it != this->map.end())
	{
		if (it->second != "" && it->second == "HTTP/1.1")
			return (true);
	}
	return (false);
}

bool Request::check_method()
{
	it = this->map.find("method");
	if (it != this->map.end())
	{
		if (it->second != "" && (it->second == "POST" || it->second == "GET"
			|| it->second == "DELETE"))
		{
			this->set_method(it->second);
			return (true);
		}
			
	}
	return (false);
}

void Request::ret_cnt_Type()
{
	std::string extention;
	std::ifstream infile("mime.txt");
	std::string line;
	std::string name, value, val = "";
	size_t i, pos;
	value = this->geturi();
	i = value.find(".");
	if (i != std::string::npos)
		extention = value.substr(i + 1);
	if (infile.is_open())
	{
		while (std::getline(infile, line))
		{
			pos = line.find(' ');
			name = line.substr(0, pos);
			val = line.substr(pos + 1);
			if (std::strcmp(name.c_str(), extention.c_str()) == 0)
			{
				this->ret_cnt_type = val;
				break;
			}
		}
		infile.close();
	}
}
void Request::get_port()
{
	std::string value;
	
	size_t i = 0;
	
	it = this->map.find("Host");
	if (it != this->map.end() && it->second != "")
	{
		value = it->second;
		i = it->second.find(":");
		if (i != std::string::npos)
		{
			this->host = value.substr(0, i);
			std::string s(value, i + 1);
			this->port = std::atoi(s.c_str());
		}
		else
		{
			this->port = 80;
			this->host = value;
		}
	}
	else
		this->flag = 1;

}

std::string Request::getRetCntType(void)
{
	return this->ret_cnt_type;
}

std::string Request::geturi(void)
{
	std::string value = "";
	it = this->map.find("uri");
	if (it != this->map.end())
	{
		if (it->second != "")
			value = it->second;
	}
	return (value);
}
void Request::set_method(std::string method)
{
	this->method = method;
}

std::string Request::getmethod(void)
{
	return (this->method);
}

std::map<std::string, std::string> Request::getMap(void)
{
	return this->map;
}

void Request::affichage_request()
{
	//affichage de map
	std::map<std::string, std::string>::iterator it;
	std::string ext;
	for (it = map.begin(); it != map.end(); it++)
	{
		std::cout << "["<<  it->first << "] => [" << it->second << "]\n";
	}
	this->ret_cnt_Type();
	std::cout << "returned content type [" << this->ret_cnt_type << "]\n";

	if (this->check_method() == true)
		std::cout << "method allowed \n";
}
