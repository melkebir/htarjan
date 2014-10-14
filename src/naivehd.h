#ifndef HTARJAN_H
#define HTARJAN_H

#include <lemon/list_graph.h>
#include <lemon/adaptors.h>
#include <vector>
#include <set>
#include <ostream>

class NaiveHD
{
public:
  typedef lemon::ListDigraph Digraph;

  TEMPLATE_DIGRAPH_TYPEDEFS(Digraph);
  
  NaiveHD(const Digraph& g, const DoubleArcMap& w);
  
  void run();
  
  void print(std::ostream& out);
  
private:
  typedef std::vector<Arc> ArcVector;
  typedef ArcVector::const_iterator ArcVectorIt;
  typedef lemon::FilterArcs<const Digraph> SubDigraph;
  typedef SubDigraph::NodeIt SubNodeIt;
  typedef Digraph::NodeMap<Node> NodeNodeMap;
  typedef std::set<Node> NodeSet;
  typedef NodeSet::const_iterator NodeSetIt;
  typedef std::vector<NodeSet> NodeSetVector;
  typedef NodeSetVector::const_iterator NodeSetVectorIt;
  
  struct Comparison
  {
  public:
    Comparison(const DoubleArcMap& w)
      : _w(w)
    {
    }
    
    bool operator() (const Arc& a, const Arc& b)
    {
      return _w[a] < _w[b];
    }
    
  private:
    const DoubleArcMap& _w;
  };
  
  const Digraph& _g;
  const DoubleArcMap& _w;
  ArcVector _arcs;
  BoolArcMap _arcFilter;
  SubDigraph _subG;
  Digraph _T;
  NodeNodeMap _g2T;
  IntNodeMap _comp;
  DoubleNodeMap _label;
};

#endif // HTARJAN_H
