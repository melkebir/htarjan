/*
 *  main.cpp
 *
 *   Created on: 14-oct-2014
 *       Author: M. El-Kebir
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <lemon/arg_parser.h>
#include <lemon/connectivity.h>

#include "hd.h"
#include "naivehd.h"
#include "tarjanhd.h"

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
  
  if (lemon::stronglyConnected(g))
  {
    std::cerr << "Graph is not strongly connected" << std::endl;
    return false;
  }
  
  return true;
}

int main(int argc, char** argv)
{
  Digraph g;
  DoubleArcMap w(g);
  
  lemon::ArgParser ap(argc, argv);
  ap.boolOption("n", "Use naive method");
  ap.run();
  
  if (ap.files().size() == 0)
  {
    if (!readGraph(g, w, std::cin))
    {
      return 1;
    }
  }
  else
  {
    std::ifstream inFile(ap.files()[0].c_str());
    if (!readGraph(g, w, inFile))
    {
      return 1;
    }
  }

  if (ap.given("n"))
  {
    NaiveHD nhd(g, w);
    nhd.run();
    nhd.printTreeArcList(std::cout);
    nhd.printTree(std::cerr);
  }
  else
  {
    TarjanHD thd(g, w);
    thd.run();
    thd.printTreeArcList(std::cout);
    thd.printTree(std::cerr);
  }
  
  return 0;
}