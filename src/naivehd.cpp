/*
 *  naivehd.cpp
 *
 *   Created on: 14-oct-2014
 *       Author: M. El-Kebir
 */

#include "naivehd.h"
#include <lemon/connectivity.h>

NaiveHD::NaiveHD(const Digraph& g, const DoubleArcMap& w)
  : HD(g, w)
{
}

void NaiveHD::run()
{
//  printGraph(_orgG, _orgW, std::cout);
  
  // initialize _T
  NodeNodeMap org2RootInT(_orgG, lemon::INVALID);
  _T.clear();
  for (NodeIt v(_orgG); v != lemon::INVALID; ++v)
  {
    Node vv = _T.addNode();
    _orgG2T[v] = vv;
    _T2OrgG[vv] = v;
    _label[vv] = -1;
    org2RootInT[v] = vv;
  }
  
  NodeHasher<Digraph> hasher(_orgG);

  IntNodeMap comp(_orgG, -1);
  BoolArcMap arcFilter(_orgG, false);
  BoolNodeMap nodeFilter(_orgG, true);
  SubDigraph subG(_orgG, nodeFilter, arcFilter);
  
  // sort by weight
  ArcVector arcs;
  for (ArcIt a(_orgG); a != lemon::INVALID; ++a)
  {
    arcs.push_back(a);
  }

  std::sort(arcs.begin(), arcs.end(), Comparison(_orgW));

  int itCount = 0;
  double prev_weight = std::numeric_limits<double>::max();
  Node root = lemon::INVALID;
  for (ArcVectorIt it = arcs.begin(); it != arcs.end(); ++it, ++itCount)
  {
    Arc arc = *it;
    subG.enable(arc);
    
    int n = lemon::stronglyConnectedComponents(subG, comp);
    NodeSetVector components(n, NodeSet(42, hasher));
    
    for (SubNodeIt v(subG); v != lemon::INVALID; ++v)
    {
      components[comp[v]].insert(v);
    }
    
    for (NodeSetVectorIt compIt = components.begin(); compIt != components.end(); ++compIt)
    {
      const NodeSet& component = *compIt;
      
      bool different = false;
      for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end();)
      {
        NodeSetIt nextNodeIt = nodeIt;
        ++nextNodeIt;
        
        if (nextNodeIt != component.end() && org2RootInT[*nodeIt] != org2RootInT[*nextNodeIt])
        {
          different = true;
          break;
        }
        
        nodeIt = nextNodeIt;
      }
      
      if (different && _orgW[arc] != prev_weight)
      {
        prev_weight = _orgW[arc];
        root = _T.addNode();
        _T2OrgG[root] = lemon::INVALID;
        _label[root] = _orgW[arc];
        for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end(); ++nodeIt)
        {
          _T.addArc(org2RootInT[*nodeIt], root);
          org2RootInT[*nodeIt] = root;
        }
      }
      else if (different)
      {
        for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end(); ++nodeIt)
        {
          _T.addArc(org2RootInT[*nodeIt], root);
          org2RootInT[*nodeIt] = root;
        }
      }
    }
  }
  
  // let's kill the multiple arcs in _T
  int n = lemon::countNodes(_T);
  std::vector< std::vector<bool> > adjT(n, std::vector<bool>(n, false));
  for (ArcIt a(_T); a != lemon::INVALID;)
  {
    ArcIt nextIt = a;
    ++nextIt;

    Node u = _T.source(a);
    Node v = _T.target(a);
    
    if (adjT[_T.id(u)][_T.id(v)])
    {
      _T.erase(a);
    }
    else
    {
      adjT[_T.id(u)][_T.id(v)] = true;
    }

    a = nextIt;
  }

#ifdef DEBUG
  Node root = _G2T[NodeIt(_orgG)];
  for (NodeIt v(_orgG); v != lemon::INVALID; ++v)
  {
    assert(root == _G2T[v]);
  }
#endif
}

