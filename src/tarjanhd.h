/*
 *  tarjanhd.h
 *
 *   Created on: 17-oct-2014
 *       Author: M. El-Kebir
 */

#ifndef TARJANHD_H
#define TARJANHD_H

#include <lemon/adaptors.h>
#include <list>
#include <vector>
#include <unordered_set>
#include <ostream>
#include <functional>

#include "hd.h"

class TarjanHD : public HD
{
public:
  TarjanHD(const Digraph& g, const DoubleArcMap& w);
  
  void run();
  
private:
  // In: G = (V, {e_1, e_2, ..., e_i, ... e_m})
  //
  // Pre-condition:
  // G_i = (V, {e_1, e_2, ..., e_i}) is acyclic,
  //       i.e. every vertex of G_i is in its own strongly connected component
  // i >= 0
  Node hd(const Digraph& g,
          const DoubleArcMap& w,
          SubDigraph& subG,
          NodeNodeMap& mapToOrgG,
          NodeNodeMap& G2T,
          const ArcList& sortedArcs,
          int i);
  
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
  
  void fixTree(Node root)
  {
    bool change = true;
    while (change)
    {
      NodeSet sameWeightChildren(10, NodeHasher<Digraph>(_T));
      change = false;
      for (InArcIt a(_T, root); a != lemon::INVALID; ++a)
      {
        Node child = _T.source(a);
        if (_T2OrgG[child] == lemon::INVALID && _label[root] == _label[child])
          sameWeightChildren.insert(child);
      }
      
      for (NodeSetIt nodeIt = sameWeightChildren.begin();
           nodeIt != sameWeightChildren.end(); ++nodeIt)
      {
        _T.contract(root, *nodeIt);
        change = true;
      }
    }
    
    for (InArcIt a(_T, root); a != lemon::INVALID; ++a)
    {
      fixTree(_T.source(a));
    }
  }
};

#endif // TARJANHD_H
