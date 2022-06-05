#include "main.hpp"
#include "cluster.hpp"

#define DEFAULT_CONFIG "configs/default.conf"

int main(int ac, char **av)
{
  std::string file;
  if (ac != 1 && ac != 2)
    return (1);
  if (ac == 1)
    file = DEFAULT_CONFIG;
  else if (ac == 2)
    file = av[1];
  try {
    Cluster::getInstance().parse(file);

    Cluster::getInstance().setup();
    Cluster::getInstance().run();
  } catch (char const *s) {
    std::cerr << RED << s << "\n";
    return 1;
  }

  return (0);
}
