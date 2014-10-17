#ifndef NAIVEHD_H
#define NAIVEHD_H

#include <lemon/adaptors.h>
#include <vector>
#include <set>
#include <ostream>

#include "hd.h"

class NaiveHD
{
public:
  NaiveHD(const Digraph& g, const DoubleArcMap& w);
  
  void run();
  
  void print(std::ostream& out);
  
  void printArcList(std::ostream& out) const
  {
    typename Digraph::NodeMap<BoolNodeMap*> A(_T, NULL);
    for (NodeIt v(_T); v != lemon::INVALID; ++v)
    {
      A[v] = new BoolNodeMap(_T, false);
    }
    
    for (ArcIt a(_T); a != lemon::INVALID; ++a)
    {
      Node u = _T.source(a);
      Node v = _T.target(a);
      
      if (!(*A[u])[v])
      {
        out << _label[u] << " -> " << _label[v] << std::endl;
        A[u]->set(v, true);
      }
    }
    
    for (NodeIt v(_T); v != lemon::INVALID; ++v)
    {
      delete A[v];
    }
  }
  
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

#endif // NAIVEHD_H
