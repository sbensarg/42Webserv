#include "cgi.hpp"

cgi::cgi(Request &req, s_route &conf, std::string &path)
{
  this->config = &conf;
  this->reqs = &req;
  this->outputfile = "/tmp/resp_body";
  this->fd_input = 0;
  this->file = path.c_str();
  this->SetUpArgs_fds();
  this->ExecuteCgi();
}

int cgi::checkcgi(std::string path)
{
  struct stat s;
  if( stat(path.c_str(), &s) == 0)
    {
      if ( S_ISDIR(s.st_mode))
        {
          return(0);
        }
      else if (S_ISREG(s.st_mode))
        {
          // it's a file
          if(!(s.st_mode & S_IXUSR))
            return (1);
          else
            return (0);
        }
    }
  //error
  return (0);
}


void cgi::SetCgiEnv()
{
  setenv("GATEWAY_INTERFACE", "CGI/1.1",1);
  setenv("SERVER_PROTOCOL", "HTTP/1.1",1);
  setenv("REDIRECT_STATUS", "200",      1);
  setenv("SCRIPT_FILENAME", this->file , 1);
  setenv("REQUEST_METHOD",  this->reqs->method.c_str(), 1);
  std::cout << "request method >>>>> " << this->reqs->method;
  if (this->reqs->map.find("Content-Type") != this->reqs->map.end())
    setenv("CONTENT_TYPE", this->reqs->map["Content-Type"].c_str(), 1);
  else
    setenv("CONTENT_TYPE", this->reqs->getRetCntType().c_str(), 1);
  if (this->reqs->map.find("Cookie") != this->reqs->map.end())
    setenv("HTTP_COOKIE", this->reqs->map["Cookie"].c_str(), 1);
  if (this->reqs->method == "GET")
    {
      //std::cout << "get is in ==> " << "QUERY_STRING" << "\n\n\n";
      setenv("QUERY_STRING", this->reqs->map["GET_params"].c_str(), 1);
      setenv("CONTENT_LENGTH", "", 1);
    }
  else if (this->reqs->method == "POST")
    {
      this->fd_input = this->reqs->pipes[0];
      setenv("QUERY_STRING", "", 1);
      setenv("CONTENT_LENGTH", this->reqs->map["Content-Length"].c_str(), 1);
    }
 }

void cgi::SetUpArgs_fds()
{
  this->args[2] = NULL;
  if (checkcgi(this->config->cgi_path) == 0)
      this->args[0] = (char *)this->config->cgi_path.c_str();
  else
    throw 500;
   this->args[1] = (char *)this->file;
   this->fd_output = open((this->outputfile).c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0666);
}


void cgi::ExecuteCgi()
{
  int pid = fork();
  if (pid == 0)
    {
      this->SetCgiEnv();
      dup2(this->fd_input, 0);
      dup2(this->fd_output, 1);
      dup2(this->fd_output, 2);
      execve(this->args[0], (char **)this->args, (char **)environ);
      exit(1);
    }
  else
    {
      waitpid(pid, NULL, 0);
      //close(this->fd_input);
      close(this->fd_output);
    }

}

std::string cgi::get_output() const
{
  return (this->outputfile);
}

cgi::~cgi()
{
  //remove("body_content");
}
