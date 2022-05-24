#ifndef SERVER_H
#define SERVER_H

#include "main.hpp"

struct	s_listen {
	std::string	host;
	int port;

  s_listen()
  {
    port = 0;
  }

  s_listen(std::string &h, int &p)
  {
    this->host = h;
    this->port = p;
  }

  s_listen(const s_listen &s)
  {
    this->host = s.host;
    this->port = s.port;
  }

  s_listen &operator=(const s_listen &s)
  {
    this->host = s.host;
    this->port = s.port;
    return (*this);
  }
//  for( i = 0 i < config.size())
//  {
// 	 port = config.getPo
// 	 host = Config
// 	 if(server == {port, host})
// 	 	returnn config[i]
//  }
  bool operator==(const s_listen &s) const
  {
    return (this->host == s.host && this->port == s.port);
  }
};


class Server {
public:
  Server(s_listen &l);
  void setup(void);
  int getFd(void);
  ~Server();

public:
  s_listen listen;
  int server_socket;


};



#endif /* SERVER_H */
