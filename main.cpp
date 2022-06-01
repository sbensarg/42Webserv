#include "main.hpp"
#include "cluster.hpp"


int main(int ac, char **av, char **envp)
{
  if (ac == 2)
    {
      std::string file(av[1]);

      try {
        // Cluster c(file);
        Cluster::getInstance().parse(file);

        // c.setup();
        // c.run();
        Cluster::getInstance().setup();
        Cluster::getInstance().run();
      } catch (char const *s) {
        std::cerr << RED << s << "\n";
        return 1;
      }

    }
  return (0);
}
