#ifndef CGI_HPP
#define CGI_HPP

#include "main.hpp"
#include "request.hpp"
#include "config.hpp"

extern char **environ;

class cgi
{
public:
  // Response
  Request *reqs;
  s_route *config;
  const char *file;

  char **envs;
  char *args[3];
  int p[2];

  cgi(){};
  ~cgi();
  cgi(Request &reqs, s_route &conf, std::string &path);

  int fd_input;
  int fd_output;
  std::string outputfile;

  //methods
  void SetCgiEnv(void);
  void ExecuteCgi(void);
  void SetUpArgs_fds(void);
  int checkcgi(std::string path);
  std::string get_output() const;
};

#endif
