#ifndef CONFIG_H
#define CONFIG_H

#include "server.hpp"
#include "main.hpp"

struct s_route
{
  std::string redirect_url;
  int redirect_status_code;
  std::string root;
  std::string cgi_path;
  std::vector<std::string> index;
  std::vector<std::string> allowed_methods;
  bool autoindex;

  s_route(): autoindex(0)
  {
    allowed_methods.push_back("POST");
    allowed_methods.push_back("GET");
    allowed_methods.push_back("DELETE");
  }
  s_route(const s_route &c)
  {
    this->allowed_methods = c.allowed_methods;
    this->redirect_url = c.redirect_url;
    this->redirect_status_code = c.redirect_status_code;
    this->root = c.root;
    this->autoindex = c.autoindex;
    this->index = c.index;
    this->cgi_path = c.cgi_path;
  }
};



class Config
{
public:
  Config(std::string &b);
  Config(const Config &c);
  Config(void);
  ~Config();
  s_listen &get_listen(void);
  std::string &get_servername(void);
  std::map<std::string, s_route> &get_routes(void);
  std::map<int, std::string> & get_error_pages(void);
   unsigned long long & get_body_size(void);

private:
  std::pair<std::string, s_route> get_route(std::string &b, std::istringstream &block);
private:
  Server *serv;
  s_listen listen;
  std::string server_name;
  std::map<int, std::string> error_pages;
  unsigned long long body_size;
  std::map<std::string, s_route> routes;

};



#endif /* CONFIG_H */
