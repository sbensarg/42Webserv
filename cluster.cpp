#include "cluster.hpp"

#define BUFF_SIZE 65536

Cluster::Cluster(void)
{

}
void Cluster::parse(std::string configfile)
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

Cluster & Cluster::getInstance(void)
{
  static Cluster c;
  return c;
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

int Cluster::read_request(int fd)
{
  std::map<int, Request>::iterator it;

  char buff[BUFF_SIZE] = {0};
  int recb;

  it = this->requests.find(fd);
  if (it != this->requests.end())
    {
      fcntl(fd, F_SETFL, O_NONBLOCK);
      if((recb = recv(fd, buff , BUFF_SIZE - 1, 0)) <= 0)
        return(0);
      it->second.append_data(fd, buff, recb);
      if(it->second.request_read == true || it->second.check_all_keys() == false)
        {
          return (1);
        }
    }
  return (2);
}

int	Cluster::handle_connection(int client_socket)
{
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
          for(int i = 0; i < setsize; i++)
            {
              if (FD_ISSET(i, &tmp_read))
                {
                  if (i == socket_fd)
                    {
                      std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
                      int client = accept_connection(socket_fd);
                      FD_SET(client, &this->read[socket_fd]);
                      this->server_client.insert(std::pair<int, int>(socket_fd, client));
                      this->requests.insert(std::pair<int, Request>(client, Request()));
                    }
                  else
                    {
                      int ret = this->read_request(i);
                      if (ret == 1)
                        {
                          FD_CLR(i, &this->read[socket_fd]);
                          FD_CLR(i, &tmp_read);
                          FD_SET(i, &this->write[socket_fd]);
                        }
                      else if (ret == 0)
                        {
                          this->requests.erase(i);
                          close(i);
                          FD_CLR(i, &this->read[socket_fd]);
                          FD_CLR(i, &tmp_read);
                        }
                    }
                }
              if (FD_ISSET(i, &tmp_write))
                {
                  if (this->handle_connection(i) == 1)
                    {
                      this->server_client.erase(socket_fd);
                      FD_CLR(i, &this->write[socket_fd]);
                      FD_CLR(i, &tmp_write);

                    }
                }
            }
        }
    }
}
