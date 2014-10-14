#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <lemon/list_graph.h>
#include "naivehd.h"

typedef lemon::ListDigraph Digraph;
TEMPLATE_DIGRAPH_TYPEDEFS(Digraph);

bool readGraph(Digraph& g, DoubleArcMap& weight, std::istream& in)
{
  int n = -1, m = -1;
  in >> n;
  in >> m;
  
  if (n < 0)
  {
    std::cerr << "Invalid number of nodes: " << n << std::endl;
    return false;
  }
  if (m < 0)
  {
    std::cerr << "Invalid number of arcs: " << m << std::endl;
    return false;
  }
  
  g.clear();
  g.reserveNode(n);
  g.reserveArc(m);
  
  std::string line;
  
  std::vector<Node> id;
  for (int i = 0; i < n; i++)
  {
    id.push_back(g.addNode());
  }
  
  std::getline(in, line);
  for (int i = 0; i < m; i++)
  {
    std::getline(in, line);
    std::stringstream ss(line);

    int a = -1, b = -1;
    double w = -1;
    
    ss >> a >> b >> w;
    if (!(0 <= a && a < n))
    {
      std::cerr << "Invalid first node '" << a << "' specified at line " << i << std::endl;
      return false;
    }
    if (!(0 <= b && b < n))
    {
      std::cerr << "Invalid second node '" << b << "'specified at line " << i << std::endl;
      return false;
    }
    
    Arc arc = g.addArc(id[a], id[b]);
    weight[arc] = w;
  }
  
  return true;
}

int main(int argc, char** argv)
{
  Digraph g;
  DoubleArcMap w(g);
  
//  std::ifstream inFile(argv[1]);
  
  if (!readGraph(g, w, std::cin))
  {
    return 1;
  }
  
  NaiveHD nhd(g, w);
  nhd.run();
  nhd.print(std::cout);
  
  return 0;
}