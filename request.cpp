#include "request.hpp"

Request::Request(void) : total_size(0), data(""), cnt_size(0), header_length(0), body("")
{
	
}


Request::~Request()
{

}

void Request::append_data(std::string data, int size)
{ 
	int f = 0;
	this->total_size += size;
	this->data.append(data);
	f = this->data.find("\r\n\r\n");
	if (f != std::string::npos)
	{
		std::string cnt = "Content-Length: ";
		int pos = this->data.find(cnt);
		if (pos != std::string::npos)
		{
			int len = cnt.length() + pos;
			const char *s = this->data.c_str();
			cnt_size = std::atoi(s + len);
		}
		header_length = f + 4;
	}
	std::cout << "data { " << this->data << "}\n";
		
}

int Request::check_recv_all_data()
{
	int size_request = 0;
	size_request = this->cnt_size + this->header_length;
	if (this->total_size == size_request && this->total_size > 0)
	{	
		std::cout << "\n\nDone. Received a total of: " << this->total_size << "bytes\n\n";
		return (1);
	}
	return(0);
} 

void Request::parse_request()
{
	size_t i;
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
			this->map.insert(std::pair<std::string, std::string>(first[j++], seg));
		}
	}
	std::string request_header;
	request_header = this->data.substr(i + 1);
	std::stringstream rh(request_header);
	std::string token;
	while (std::getline(rh, token, '\r') && token != "\n")
	{
		size_t pos = token.find(":");
		std::string name = token.substr(1, pos - 1);
		std::string value = token.substr(pos + 2);
		this->map.insert(std::pair<std::string, std::string>(name, value));
	}
	get_port();
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
	if (this->map.at("ver") == "HTTP/1.1")
		return (true);
	return (false);
}

bool Request::check_method()
{
	if (this->map.at("method") == "POST" || this->map.at("method") == "GET"
	|| this->map.at("method") == "DELETE" )
	return (true);
	return (false);
}

void Request::ret_cnt_Type()
{
	std::string extention;
	std::ifstream infile("request/mime.txt");
	std::string line;
	std::string name, value, val = "";
	size_t i, pos;
	value = map.at("uri");
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
	value = this->map.at("Host");
	i = value.find(":");
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

std::string Request::getRetCntType(void)
{
	return this->ret_cnt_type;
}

std::string Request::geturi(void)
{
	std::string value;
	value = this->map.at("uri");
	return (value);
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
	//this->get_port();
	//std::cout << "port " << this->port << " host " << this->host << "\n"; 
	std::cout << "POST BODY ==> {" << this->body << "}\n";
}
