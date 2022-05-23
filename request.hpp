#ifndef REQUEST_H
#define REQUEST_H

#include "main.hpp"

class Request
{
private:
	std::string body;
	std::string data;
	std::string ret_cnt_type;
	int total_size;
	int header_length;
	int cnt_size;
	std::map<std::string, std::string> map;
public:
	Request(void);
	Request(std::string &data, int size);
	~Request();
	void append_data(std::string data, int size);
	int check_recv_all_data(void);
	void parse_request(void);
	//check methods
	bool check_http_vesion(void);
	bool check_method(void);
	void ret_cnt_Type(void);
	//getters
	std::string getRetCntType(void);
	std::map<std::string, std::string> getMap(void);
	//affichage request
	void affichage_request();
};


#endif /* REQUEST_H */
