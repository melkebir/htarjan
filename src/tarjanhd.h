#ifndef TARJANHD_H
#define TARJANHD_H

#include <lemon/adaptors.h>
#include <list>
#include <vector>
#include <unordered_set>
#include <ostream>
#include <functional>

#include "hd.h"

class TarjanHD
{
public:
  TarjanHD(const Digraph& g, const DoubleArcMap& w);
  
  void run();
  
  void printArcList(std::ostream& out) const
  {
    for (ArcIt a(_T); a != lemon::INVALID; ++a)
    {
      Node u = _T.source(a);
      Node v = _T.target(a);
      
      out << _label[u] << " -> " << _label[v] << std::endl;
    }
  }
  
  void print(std::ostream& out) const
  {
    out << "digraph G {" << std::endl;
    
    for (NodeIt v(_T); v != lemon::INVALID; ++v)
    {
      out << _T.id(v) << " [label=\"" << _label[v] << "\"]" << std::endl;
    }
    
    for (ArcIt a(_T); a != lemon::INVALID; ++a)
    {
      out << _T.id(_T.source(a)) << " -> " << _T.id(_T.target(a)) << std::endl;
    }
    
    out << "}" << std::endl;
  }
  
  template<class DGR>
  void print(const DGR& g, const DoubleArcMap& w, std::ostream& out) const
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
  
private:
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
  
  typedef std::vector<Arc> ArcVector;
  typedef std::vector<ArcVector> ArcMatrix;
  
  typedef std::list<Arc> ArcList;
  typedef ArcList::const_iterator ArcListIt;
  typedef ArcList::iterator ArcListNonConstIt;
  
  typedef std::list<Node> NodeList;
  typedef NodeList::const_iterator NodeListIt;
  typedef NodeList::iterator NodeListNonConstIt;
  
  typedef std::vector<Node> NodeVector;
  typedef NodeVector::const_iterator NodeVectorIt;
  typedef NodeVector::iterator NodeVectorNonConstIt;
  
  typedef std::vector<NodeList> NodeListVector;
  typedef NodeListVector::const_iterator NodeListVectorIt;
  
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
  
  typedef std::vector<bool> BoolVector;
  typedef std::vector<BoolVector> BoolMatrix;
  
  typedef typename Digraph::NodeMap<BoolNodeMap*> BoolNodeMatrixMap;
  
  const Digraph& _orgG;
  const DoubleArcMap& _orgW;
  
  Digraph _T;
  DoubleNodeMap _label;
  NodeNodeMap _orgG2T;
  NodeNodeMap _T2OrgG;
  Node _root;
  ArcList _sortedArcs;
  
  // In: G = (V, {e_1, e_2, ..., e_i, ... e_m})
  //
  // Pre-condition:
  // G_i = (V, {e_1, e_2, ..., e_i}) is acyclic,
  //       i.e. every vertex of G_i is in its own strongly connected component
  // i >= 0
  Node hd(Digraph& g,
          const DoubleArcMap& w,
          SubDigraph& subG,
          NodeNodeMap& mapToOrgG,
          NodeNodeMap& G2T,
          ArcList& sortedArcs,
          int i);
  
//  void contract(Digraph& g, SubDigraph& subG, Node u, Node v)
//  {
//    
//  }
  
  int condenseGraph(Digraph& g,
                    const DoubleArcMap& w,
                    SubDigraph& subG,
                    NodeNodeMap& mapToOrgG,
                    NodeNodeMap& G2T,
                    ArcList& sortedArcs,
                    const IntNodeMap& comp,
                    const NodeSetVector& components,
                    const NodeVector& roots,
                    int& j,
                    ArcList& newSortedArcs);
  
  int constructCondensedGraph(const Digraph& g,
                              const DoubleArcMap& w,
                              const NodeNodeMap& mapToOrgG,
                              const NodeNodeMap& G2T,
                              const ArcList& sortedArcs,
                              const IntNodeMap& comp,
                              const NodeSetVector& components,
                              const NodeVector& roots,
                              const int j,
                              Digraph& c,
                              DoubleArcMap& ww,
                              NodeNodeMap& mapCToOrgG,
                              NodeNodeMap& C2T,
                              ArcList& newSortedArcs);
  
  void fixArcList(const Digraph& g,
                  ArcList& arcs,
                  int& j)
  {
    for (ArcListNonConstIt arcIt = arcs.begin(); arcIt != arcs.end();)
    {
      if (g.valid(*arcIt))
      {
        ++arcIt;
      }
      else
      {
        arcIt = arcs.erase(arcIt);
        --j;
      }
    }
  }
  
  void fixNodeSet(const Digraph& g,
                  NodeSet& nodes)
  {
    for (NodeSetNonConstIt nodeIt = nodes.begin(); nodeIt != nodes.end();)
    {
      if (g.valid(*nodeIt))
      {
        ++nodeIt;
      }
      else
      {
        nodeIt = nodes.erase(nodeIt);
      }
    }
  }
  
  int get_i(const ArcList& sortedArcs,
            const DoubleArcMap& w,
            double w_i) const
  {
    int new_i = 0;

    for (ArcListIt arcIt = sortedArcs.begin();
         arcIt != sortedArcs.end() && w[*arcIt] <= w_i;
         ++arcIt, ++new_i);
    
    return new_i;
  }
  
  Arc getArcByRank(const ArcList& sortedArcs, int rank)
  {
    assert(1 <= rank && rank <= static_cast<int>(sortedArcs.size()));
    
    int i = 1;
    for (ArcListIt arcIt = sortedArcs.begin(); arcIt != sortedArcs.end(); ++arcIt, ++i)
    {
      if (i == rank)
      {
        return *arcIt;
      }
    }
    
    return lemon::INVALID;
  }
  
  void print(const ArcList& arcs, const DoubleArcMap& w, std::ostream& out) const
  {
    for (ArcListIt arcIt = arcs.begin(); arcIt != arcs.end(); ++arcIt)
    {
      out << w[*arcIt] << " ";
    }
    out << std::endl;
  }
};

#endif // TARJANHD_H
