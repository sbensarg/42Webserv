#include "server.hpp"

Server::Server(s_listen &l) : listen(l), server_socket(0)
{

}


void Server::setup(void)
{
  std::cout << GREEN << "Seting up server listening on port " << this->listen.host << ":" << this->listen.port << "\n" << RESET;
  struct sockaddr_in address;

  this->server_socket = socket(AF_INET, SOCK_STREAM, 0);
  //std::cout << "fd " << this->server_socket << "\n";
  if (this->server_socket < 0)
    {
      throw ("Socket : error");
    }
  fcntl(this->server_socket, F_SETFL, O_NONBLOCK);
  int optval = 1;
  if ((setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))) == -1)
    {
		throw ("In set socket options : error");
    }

  memset(&address, 0, sizeof(sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = ::inet_addr(this->listen.host.c_str());
  address.sin_port = htons(this->listen.port);

  if (bind(this->server_socket, (struct sockaddr *)&address, sizeof(sockaddr_in)) == -1)
    {
      throw ("Bind : error");
    }
  if (::listen(this->server_socket, 1000) == -1)
    {
      throw ("listen : error");
    }
}

int Server::getFd(void)
{
  return (this->server_socket);
}

Server::~Server()
{
//	std::cout << "destructor \n"; 
 // if (this->server_socket > 0)
   // ::close(this->server_socket);
}
