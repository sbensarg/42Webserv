#include "config.hpp"

void trim(std::string &line)
{
  line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
}

Config::Config(void)
{
  this->body_size = 0;
}

Config::Config(std::string &b)
{
// TODO: Read server settings from block
  std::string line;
  std::istringstream block;

  block.str(b);
  std::getline(block, line);
  if (line.rfind("server : ", 0) != 0)
    throw ("Config File: Syntax error");
  this->server_name = line.substr(9);
  std::getline(block, line);
  if (line != "{")
    throw ("Config File: Syntax error");
  std::getline(block, line);
  trim(line);
  if (line.rfind("host=", 0) != 0)
    throw ("Config File: Syntax error");
  std::string h = line.substr(5, line.find(";") - 5);
  if (h == "localhost")
    h = "127.0.0.1";
  std::getline(block, line);
  trim(line);
  if (line.rfind("port=", 0) != 0)
    throw ("Config File: Syntax error");
  int p;
  std::istringstream(line.substr(5, line.find(";") - 5)) >> p;
  this->listen = s_listen(h, p);
  while (std::getline(block, line))
    {
      trim(line);
      if (line == "}")
        break ;
      if (line.rfind("error_page_", 0) == 0)
        {
          std::pair<int, std::string> ret;
          std::istringstream(line.substr(11, 3)) >> ret.first;
          if (*(line.c_str() + 14) != '=' || line.find(";") == std::string::npos)
            throw ("Config File: Syntax error");
          ret.second = line.substr(15, line.find(";") - 15);
          this->error_pages.insert(ret);

          //TODO: check if path valid
        }
      else if (line.rfind("location:", 0) == 0)
          this->routes.insert(this->get_route(line, block));
      else if (line.rfind("max_body_size=", 0) == 0)
        std::istringstream(line.substr(14, line.find(";") - 14)) >> this->body_size;
      else
        throw ("Config File: Syntax error");
    }
  if (this->routes.find("/") == this->routes.end())
    throw ("Config File: location: / missing");

}

s_listen &Config::get_listen(void)
{
  return (this->listen);
}

void insertvec(std::vector<std::string> &vec, std::string str)
{
  size_t pos = 0;
  std::string token;
  while ((pos = str.find(",")) != std::string::npos)
    {
      token = str.substr(0, pos);
      str.erase(0, pos + 1);
      vec.push_back(token);
    }
  vec.push_back(str);
}


std::string &Config::get_servername(void)
{
  return (this->server_name);
}

std::map<std::string, s_route> &Config::get_routes(void)
{
	return (this->routes);
}
std::map<int, std::string> & Config::get_error_pages(void)
{
	return (this->error_pages);
}

unsigned long long & Config::get_body_size(void)
{
	return (this->body_size);
}

std::pair<std::string, s_route> Config::get_route(std::string &b, std::istringstream &block)
{
  std::pair<std::string, s_route> ret;

  ret.first = b.substr(9);
  if (ret.first.length() == 0)
    throw ("Config File: Syntax error");
  std::getline(block, b);
  trim(b);
  if (b != "{")
    throw ("Config File: Syntax error");
  while (std::getline(block, b))
    {
      trim(b);
      if (b == "}")
        break ;
      if (b.find(";") == std::string::npos)
        throw ("Config File: Syntax error");
      if (b.rfind("root=") == 0)
        ret.second.root = b.substr(5, b.find(";") - 5);
      else if (b.rfind("autoindex=") == 0)
        {
          if (b.substr(10, b.find(";") - 10) == "on")
            ret.second.autoindex = 1;
          else if (b.substr(10, b.find(";") - 10) != "off")
            throw ("Config File: Syntax error");
        }
      else if (b.rfind("path=") == 0)
        {
          ret.second.cgi_path = b.substr(5, b.find(";") - 5);
        }
      else if (b.rfind("allowed_methods=") == 0)
        {
          ret.second.allowed_methods.clear();
          ::insertvec(ret.second.allowed_methods, b.substr(16, b.find(";") - 16));
        }
      else if (b.rfind("index_order=") == 0)
        {
          ::insertvec(ret.second.index, b.substr(12, b.find(";") - 12));
        }
      else if (b.rfind("redirect_code=") == 0)
        {
          std::istringstream(b.substr(14, b.find(";"))) >> ret.second.redirect_status_code;
        }
      else if (b.rfind("redirect_url=") == 0)
        {
          ret.second.redirect_url = b.substr(13, b.find(";"));
        }
      else
        throw ("Config File: Syntax error");
  }
  return ret;
}


Config::Config(const Config &c)
{
  this->server_name = c.server_name;
  this->error_pages = c.error_pages;
  this->routes = c.routes;
  this->body_size = c.body_size;
  this->listen = c.listen;
}

Config::~Config()
{

}

