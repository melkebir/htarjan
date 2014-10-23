/*
 *  hd.h
 *
 *   Created on: 17-oct-2014
 *       Author: M. El-Kebir
 */

#ifndef HD_H
#define HD_H

#include <lemon/list_graph.h>
#include <lemon/adaptors.h>

#include <unordered_set>
#include <functional>
#include <ostream>
#include <cassert>

typedef lemon::ListDigraph Digraph;
DIGRAPH_TYPEDEFS(Digraph);

class HD
{
public:
  HD(const Digraph& g, const DoubleArcMap& w);
  
  virtual void run() = 0;
  
  void printTree(std::ostream& out) const;
  
  void printTreeArcList(std::ostream& out) const;
  
  template<class DGR>
  void printGraph(const DGR& g, const DoubleArcMap& w, std::ostream& out) const
  {
    out << "digraph G {" << std::endl;
    out << "layout=neato" << std::endl;
    out << "overlap=scale" << std::endl;
    
    for (typename DGR::NodeIt v(g); v != lemon::INVALID; ++v)
    {
      out << g.id(v) << std::endl;
    }
    
    for (typename DGR::ArcIt a(g); a != lemon::INVALID; ++a)
    {
      out << g.id(g.source(a)) << " -> " << g.id(g.target(a)) << " [label=\"" << w[a] << "\"]" << std::endl;
    }
    
    out << "}" << std::endl;
  }
  
  const Digraph& getT() const { return _T; }
  const Node toT(Node nodeInG) const { return _orgG2T[nodeInG]; }
  const Node toG(Node nodeInT) const { return _T2OrgG[nodeInT]; }
  double label(Node nodeInT) const { return _label[nodeInT]; }
  
protected:
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
  
  template<class T>
  struct NodeHasher
  {
  public:
    using Digraph = T;
    NodeHasher(const Digraph& g)
    : _g(g)
    {
    }
    
    size_t operator()(const typename Digraph::Node& v) const
    {
      return std::hash<int>()(_g.id(v));
    }
    
  private:
    const Digraph& _g;
  };
  
  typedef std::list<Arc> ArcList;
  typedef ArcList::const_iterator ArcListIt;
  typedef ArcList::iterator ArcListNonConstIt;
  
  typedef std::vector<Node> NodeVector;
  typedef NodeVector::const_iterator NodeVectorIt;
  typedef NodeVector::iterator NodeVectorNonConstIt;
  
  typedef std::unordered_set<Node, NodeHasher<Digraph>> NodeSet;
  typedef NodeSet::const_iterator NodeSetIt;
  typedef NodeSet::iterator NodeSetNonConstIt;
  
  typedef std::vector<NodeSet> NodeSetVector;
  typedef NodeSetVector::const_iterator NodeSetVectorIt;
  
  typedef Digraph::NodeMap<Node> NodeNodeMap;
  typedef lemon::SubDigraph<const Digraph> SubDigraph;
  typedef SubDigraph::NodeIt SubNodeIt;
  typedef SubDigraph::ArcIt SubArcIt;
  typedef SubDigraph::OutArcIt SubOutArcIt;
  
  const Digraph& _orgG;
  const DoubleArcMap& _orgW;
  Digraph _T;
  DoubleNodeMap _label;
  NodeNodeMap _orgG2T;
  NodeNodeMap _T2OrgG;
  Node _root;
};

#endif // HD_H
