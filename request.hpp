#ifndef REQUEST_H
#define REQUEST_H

#include "main.hpp"

class Request
{
public:
	std::string body;
	std::string data;
	std::string ret_cnt_type;
	int total_size;
	int header_length;
	int cnt_size;
	int port;
	std::string host;
	std::map<std::string, std::string> map;
	std::map<std::string, std::string>::iterator it;
	int flag;
	std::string method;
	int fd;
	int pipes[2];
	std::string bodyfilename;
	int bodyfd;
	bool request_read;
	int check;
public:
	Request(void);
	Request(std::string &data, int size);
	~Request();
	void append_data(int fd, char * data, int size);
	int check_recv_all_data(void);
	void parse_request(void);
	//check methods
	bool check_http_vesion(void);
	bool check_method(void);
	void ret_cnt_Type(std::string uri);

	//getters
	std::string getRetCntType(void);
	std::string geturi(void);
	std::string getmethod(void);
	void find_method(void);
	void get_port();
	// check all keys
	bool check_all_keys(void);
	//setters
	void set_method(std::string method);
	std::map<std::string, std::string> getMap(void);
	//affichage request
	void affichage_request();
};


#endif /* REQUEST_H */
