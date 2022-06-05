#ifndef CLUSTER_H
#define CLUSTER_H

#include "main.hpp"
#include "config.hpp"
#include "request.hpp"
#include "response.hpp"
class Request;
class Response;

class Cluster {
public:
	//Cluster(std::string configfile = DEFAULT_CONFIG);
	void parse(std::string file);
	~Cluster();

	void setup(void);
	void run(void);
	int read_request(int fd);
	int handle_connection(int client_socket);
	static Cluster & getInstance(void);

public:
	Cluster(void);
	std::vector<Config> configs;
	std::vector<Server> servers;
	std::map<int, fd_set> read;
	std::map<int, fd_set> write;
	std::map<int, Request> requests;
	std::map<int, int> server_client;
	int setsize;
};



#endif /* CLUSTER_H */
