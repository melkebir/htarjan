/*
 *  hd.cpp
 *
 *   Created on: 23-oct-2014
 *       Author: M. El-Kebir
 */

#include "hd.h"

HD::HD(const Digraph& g, const DoubleArcMap& w)
  : _orgG(g)
  , _orgW(w)
  , _T()
  , _label(_T, -1)
  , _orgG2T(_orgG, lemon::INVALID)
  , _T2OrgG(_T, lemon::INVALID)
  , _root(lemon::INVALID)
{
}

void HD::printTreeArcList(std::ostream& out) const
{
  for (ArcIt a(_T); a != lemon::INVALID; ++a)
  {
    Node u = _T.source(a);
    Node v = _T.target(a);
    
    out << _label[u] << " -> " << _label[v] << std::endl;
  }
}

void HD::printTree(std::ostream& out) const
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
