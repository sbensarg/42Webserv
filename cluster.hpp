#ifndef CLUSTER_H
#define CLUSTER_H

#include "main.hpp"
#include "config.hpp"
#include "request.hpp"
#include "response.hpp"
class Request;
#define DEFAULT_CONFIG "configs/default.conf"

class Cluster {
public:
  //Cluster(void);
  Cluster(std::string configfile = DEFAULT_CONFIG);
  ~Cluster();

  void setup(void);
  void run(void);
  int read_request(int fd);
  int handle_connection(int client_socket);
  //find path from uri
  void find_Path(int fd);
private:
  std::vector<Config> configs;
  std::vector<Server> servers;
  std::map<int, fd_set> read;
  std::map<int, fd_set> write;
  std::map<int, Request> requests;
  int setsize;
};



#endif /* CLUSTER_H */
