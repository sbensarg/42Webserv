#include "cluster.hpp"

// Cluster::Cluster(void)
// {

// }
Cluster::Cluster(std::string configfile)
{
  //TODO: read config file
  std::ifstream configfd(configfile);
  std::string line;
  std::string block;
  std::size_t search;
  int bf;

  bf = 0;
  if (configfd.is_open())
  {
    while (!configfd.eof())
    {
      std::getline(configfd, line);
      if (line == "")
        continue ;
      block += line;
      block += "\n";
      search = line.find("{");
      if (search != std::string::npos)
        bf++;
      search = line.find("}");
      if (search != std::string::npos)
        bf--;
      if (bf == 0 && search != std::string::npos)
      {
        configs.push_back(Config(block));
        block = "";
      }
    }
    if (bf != 0)
      throw ("Config File: Syntax error");
    std::cerr << YELLOW << "Finised reading config file\n" << RESET;
  }
  else
  {
    throw ("ifstream : error");
  }
}

Cluster::~Cluster()
{

}

bool check_duplicated_servers(Config &f, Config &s)
{
  if (f.get_listen() == s.get_listen() && f.get_servername() == s.get_servername())
    throw ("Config File: Syntax error");
  return (0);
}

void Cluster::setup(void)
{
  std::vector<Config>::iterator it = configs.begin();
  std::vector<s_listen> listens;

  std::unique(configs.begin(), configs.end(), check_duplicated_servers);

  while (it != configs.end())
  {
    s_listen t;
    t = (*it).get_listen();

    listens.push_back(t);
    it++;
  }

  std::vector<s_listen>::iterator uni;
  uni = std::unique(listens.begin(), listens.end());
  listens.resize(std::distance(listens.begin(), uni));

  for(std::vector<s_listen>::iterator it2 = listens.begin(); it2 != listens.end(); it2++)
  {
    Server s(*it2);

    servers.push_back(s);
    (*(servers.rbegin())).setup();
    int fd = (*(servers.rbegin())).getFd();
    fd_set r;
    fd_set w;

    FD_ZERO(&r);
    FD_ZERO(&w);
    FD_SET(fd, &r);
    FD_SET(fd, &w);
    this->read.insert(std::pair<int, fd_set>(fd, r));
    this->write.insert(std::pair<int, fd_set>(fd, w));
  }
}

int accept_connection(int server_socket)
{
  int client_socket, addrlen;
  addrlen = sizeof(sockaddr_in);
  struct sockaddr_in client_addr;


  if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) < 0)
    throw ("accept: error");
  return (client_socket);
}



void Cluster::find_Path(int fd)
{
	Response s;
	std::string location_full_path;
	std::map<int, Request>::iterator it;
	std::string uri;
	int flag = 0;
	it = this->requests.find(fd);
	if (it != this->requests.end())
	{
		for(std::vector<Config>::iterator it2 = configs.begin(); it2 != configs.end(); it2++)
		{
			if (it->second.host == it2->get_listen().host && it->second.port == it2->get_listen().port)
			{
				uri = it->second.geturi(); 
				std::map<std::string, s_route>::iterator i = it2->get_routes().begin();
				int j = it2->get_routes().size() - 1;
				while (i != it2->get_routes().end())
				{
					std::string last_path = i->first;
					std::string first_path = i->second.root;
					std::cout << "last_path => " << last_path << "| uri =>" << uri << "\n";
					if (uri == last_path)
					{
						std::cout << "dkhaal " << "\n";
						location_full_path = i->second.root + i->first;
						s.setFullPathLocation(location_full_path);
						break;
					}
					i++;
					if (j == 0)
					{
						i = it2->get_routes().begin();
						j = it2->get_routes().size() - 1;
						size_t pos = 0;
						pos = uri.find_last_of("/");
						if (pos != std::string::npos && pos != 0)
						{
							uri = uri.substr(0, pos);
						}
						else if (pos == 0)
							uri = "/";
					}
					j--;			
				}
			}
				
		}
	}

	std::cout << "full path " << s.getFullPathLocation() << "\n";
}

int Cluster::read_request(int fd)
{
  std::map<int, Request>::iterator it;

	char buff[8192];
	int recb;
	std::string str;
	fcntl(fd, F_SETFL, O_NONBLOCK);
	memset(buff, 0, 8192);
	if((recb = recv(fd, buff, 8192, 0)) <= 0)
	{
		this->requests.erase(fd);
		close(fd);
		throw("recv: error");
	}
	str = buff;
	it = this->requests.find(fd);
	if (it != this->requests.end())
	{
		it->second.append_data(str, recb);
		if (it->second.check_recv_all_data() == 1)
		{
			it->second.parse_request();
			it->second.affichage_request();
			this->find_Path(fd);

			return (1);
		}
  }
  return (0);
}

int	Cluster::handle_connection(int client_socket)
{
	int size_request = 0;
	std::map<int, Request>::iterator it;
	Response response;
	it = this->requests.find(client_socket);
	if (it != this->requests.end())
	{
		response.make_response(client_socket, it->second);
		close(client_socket);
		this->requests.erase(client_socket);
		return (1);
	}
	return (0);
}

void Cluster::run(void)
{
  //TODO: server main loop
  fd_set tmp_read;
  fd_set tmp_write;

  setsize = FD_SETSIZE;
  struct timeval tv = {0, 100};
  
  while (1)
  {
    for(std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
      int socket_fd = (*it).getFd();
      tmp_read = this->read[socket_fd];
      tmp_write = this->write[socket_fd];
      if (select(setsize, &tmp_read, &tmp_write, NULL, &tv) < 0)
        throw ("select: error");
      for(int i = 0; i <= setsize; i++)
      {
        if (FD_ISSET(i, &tmp_read))
        {
          if (i == socket_fd)
          {
			std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
            int client = accept_connection(socket_fd);
            FD_SET(client, &this->read[socket_fd]);

			this->requests.insert(std::pair<int, Request>(client, Request()));                         
          }
          else
          {
             	if (this->read_request(i) == 1)
				{
					FD_CLR(i, &this->read[socket_fd]);
					FD_SET(i, &this->write[socket_fd]);
				}
          }
        }
        if (FD_ISSET(i, &tmp_write))
        {
			if (this->handle_connection(i) == 1)
				FD_CLR(i, &this->write[socket_fd]);
        }
      }
    }
  }
}
