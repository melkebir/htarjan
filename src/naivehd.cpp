#include "naivehd.h"
#include <lemon/connectivity.h>

NaiveHD::NaiveHD(const Digraph& g, const DoubleArcMap& w)
  : _g(g)
  , _w(w)
  , _arcs()
  , _arcFilter(g, false)
  , _subG(g, _arcFilter)
  , _T()
  , _g2T(g, lemon::INVALID)
  , _comp(_g, -1)
  , _label(_T)
{
  for (ArcIt a(_g); a != lemon::INVALID; ++a)
  {
    _arcs.push_back(a);
  }
}

void NaiveHD::run()
{
  // initialize _T
  _T.clear();
  for (NodeIt v(_g); v != lemon::INVALID; ++v)
  {
    Node vv = _T.addNode();
    _g2T[v] = vv;
    _label[vv] = 0;
  }
  
  // sort by weight
  std::sort(_arcs.begin(), _arcs.end(), Comparison(_w));

  int itCount = 0;
  for (ArcVectorIt it = _arcs.begin(); it != _arcs.end(); ++it, ++itCount)
  {
    std::cerr << itCount << "/" << _arcs.size() << "\r" << std::flush;
//    std::cout << "Considering arc with weight: " << _w[*it] << std::endl;
    
    Arc arc = *it;
    _subG.enable(arc);
//    std::cout << "Nodes: " << lemon::countNodes(_subG) << std::endl;
//    std::cout << "Arcs: " << lemon::countArcs(_subG) << std::endl;
    
    int n = lemon::stronglyConnectedComponents(_subG, _comp);
    NodeSetVector components(n, NodeSet());
    
    for (SubNodeIt v(_subG); v != lemon::INVALID; ++v)
    {
      components[_comp[v]].insert(v);
    }
    
    for (NodeSetVectorIt compIt = components.begin(); compIt != components.end(); ++compIt)
    {
      const NodeSet& component = *compIt;
      
      bool different = false;
      for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end();)
      {
        NodeSetIt nextNodeIt = nodeIt;
        ++nextNodeIt;
        
        if (nextNodeIt != component.end() && _g2T[*nodeIt] != _g2T[*nextNodeIt])
        {
          different = true;
          break;
        }
        
        nodeIt = nextNodeIt;
      }
      
      if (different)
      {
        Node root = _T.addNode();
        _label[root] = _w[arc];
        for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end(); ++nodeIt)
        {
//          if (different)
          {
            _T.addArc(_g2T[*nodeIt], root);
            // abusing different to only add one arc at the start
//            different = false;
          }
          _g2T[*nodeIt] = root;
        }
      }
    }
  }

#ifdef DEBUG
  Node root = _g2T[NodeIt(_g)];
  for (NodeIt v(_g); v != lemon::INVALID; ++v)
  {
    assert(root == _g2T[v]);
  }
#endif
}

void NaiveHD::print(std::ostream& out)
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
