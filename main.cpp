#include "main.hpp"
#include "cluster.hpp"

int main(int ac, char **av)
{
  if (ac == 2)
    {
      std::string file(av[1]);
      try {
        Cluster c(file);
        c.setup();
        c.run();
      } catch (char const *s) {
        std::cerr << RED << s << "\n";
        return 1;
      }

    }
  return (0);
}
