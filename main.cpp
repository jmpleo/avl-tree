#include "avl-tree.h"

#include <vector>
#include <random>

int main()
{
  std::vector<int> val(100);
  std::uniform_int_distribution<int> u(1,100);
  std::mt19937 gen(100);

  for (auto &v : val) {
      v = u(gen);
  }
  AVLTree<int> tree(val);
  tree.makeDOT("example.dot");
  return 0;
}
