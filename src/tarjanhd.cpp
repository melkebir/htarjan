#include "tarjanhd.h"
#include <lemon/connectivity.h>
#include <limits>

TarjanHD::TarjanHD(const Digraph& g, const DoubleArcMap& w)
  : _orgG(g)
  , _orgW(w)
  , _T()
  , _label(_T, -1)
  , _orgG2T(_orgG, lemon::INVALID)
  , _T2OrgG(_T, lemon::INVALID)
  , _root(lemon::INVALID)
  , _sortedArcs()
{
}

void TarjanHD::run()
{
  _T.clear();
  
  // let's make a copy of the graph
  Digraph g;
  DoubleArcMap w(g);
  NodeNodeMap mapToOrgG(g, lemon::INVALID);
  
  lemon::digraphCopy(_orgG, g)
    .arcMap(_orgW, w)
    .nodeCrossRef(mapToOrgG)
    .run();
  
  for (ArcIt a(g); a != lemon::INVALID; ++a)
  {
    _sortedArcs.push_back(a);
  }

  // sort by weight
  _sortedArcs.sort(Comparison(w));

  BoolArcMap arcFilter(g, true);
  BoolNodeMap nodeFilter(g, true);
  NodeNodeMap G2T(g, lemon::INVALID);
 
  SubDigraph subG(g, nodeFilter, arcFilter);
  
  _root = hd(g, w, subG, mapToOrgG, G2T, _sortedArcs, 0);
//  fixArcList(g, _sortedArcs);
}

Node TarjanHD::hd(Digraph& g,
                  const DoubleArcMap& w,
                  SubDigraph& subG,
                  NodeNodeMap& mapToOrgG,
                  NodeNodeMap& G2T,
                  ArcList& sortedArcs,
                  int i)
{
  assert(i >= 0);
  
  int m = static_cast<int>(sortedArcs.size());
//  int mm = lemon::countArcs(subG);
//  std::cout << m << "\t" << mm << std::endl;
  assert(m == lemon::countArcs(subG));
//  assert(m == mm);
  
  int r = m - i;
  if (r == 1)
  {
    // add to _T a subtree rooted at node r,
    // labeled with w(e_m) and having n children labeled with
    // the vertices of subG
    Node root = _T.addNode();
    _label[root] = w[sortedArcs.back()];
    _T2OrgG[root] = lemon::INVALID;
    
    for (SubNodeIt v(subG); v != lemon::INVALID; ++v)
    {
      Node vv = G2T[v];
      if (vv == lemon::INVALID)
      {
        vv = _T.addNode();
        _label[vv] = -1;
        if (mapToOrgG[v] != lemon::INVALID)
        {
          _orgG2T[mapToOrgG[v]] = vv;
          _T2OrgG[vv] = mapToOrgG[v];
        }
      }
      _T.addArc(vv, root);
    }
    
    return root;
  }
  else
  {
    int j = (i + m) % 2 == 0 ? (i + m) / 2 : (i + m) / 2 + 1;
    // remove arcs j+1 .. m
    ArcListIt arcEndIt, arcIt = sortedArcs.begin();
    for (int k = 1; k <= m; ++k)
    {
      assert(subG.status(*arcIt));
      if (k == j + 1)
      {
        arcEndIt = arcIt;
      }
      if (k > j)
      {
        subG.disable(*arcIt);
      }
      ++arcIt;
    }
    
//    print(subG, w, std::cout);
    
    // compute SCCs
    IntNodeMap comp(g, -1);
    int numSCC = lemon::stronglyConnectedComponents(subG, comp);
    if (numSCC == 1)
    {
      ArcList newSortedArcs(sortedArcs.cbegin(), arcEndIt);
      
      // _subG is strongly connected
      Node root = hd(g, w, subG, mapToOrgG, G2T, newSortedArcs, i);
      fixArcList(g, sortedArcs, i);
      return root;
    }
    else
    {
      // determine strongly connected components
      NodeHasher<Digraph> hasher(g);
      NodeSetVector components(numSCC, NodeSet(0, hasher));
      for (SubNodeIt v(subG); v != lemon::INVALID; ++v)
      {
        components[comp[v]].insert(v);
      }
      
      NodeVector roots(numSCC, lemon::INVALID);
      double w_i = i > 0 ? w[getArcByRank(sortedArcs, i)] : -std::numeric_limits<double>::max();
      for (int k = 0; k < numSCC; ++k)
      {
        NodeSet& component = components[k];
        if (component.size() > 1)
        {
          // construct new sorted arc list for component: O(m) time
          ArcList newSortedArcs;
          for (ArcListIt arcIt = sortedArcs.begin(); arcIt != arcEndIt; ++arcIt)
          {
            Node u = g.source(*arcIt);
            Node v = g.target(*arcIt);

            bool u_in_comp = component.find(u) != component.end();
            bool v_in_comp = component.find(v) != component.end();
            
            if (u_in_comp && v_in_comp)
            {
              newSortedArcs.push_back(*arcIt);
            }
          }
          
//          std::cout << "Component " << k << ": ";
//          print(newSortedArcs, w, std::cout);
          
          // remove nodes not in component from the graph
          for (NodeIt v(g); v != lemon::INVALID; ++v)
          {
            subG.status(v, component.find(v) != component.end());
          }
          
          // find new_i, i.e. largest k such that w(e'_k) <= w(e_i)
          // if i == 0 or i > 0 but no such k exists => new_i := 0
          int new_i = get_i(newSortedArcs, w, w_i);
          
//          print(subG, w, std::cout);
          
          // recurse on strongly connected component
          roots[k] = hd(g, w, subG, mapToOrgG, G2T, newSortedArcs, new_i);
          fixArcList(g, sortedArcs, j);
          fixNodeSet(g, component);
        }
      }
      
      // enable all disabled nodes again
      for (int k = 0; k < numSCC; ++k)
      {
        const NodeSet& component = components[k];
        for (NodeSetIt nodeIt = component.begin(); nodeIt != component.end(); ++nodeIt)
        {
          subG.enable(*nodeIt);
        }
      }
      
      // enable all disabled arcs again
      for (ArcListIt arcIt = arcEndIt; arcIt != sortedArcs.end(); ++arcIt)
      {
        assert(!subG.status(*arcIt));
        subG.enable(*arcIt);
      }
      std::cout << "Arcs in subG: " << lemon::countArcs(subG) << std::endl;
      assert(lemon::countArcs(subG) == sortedArcs.size());
      
      // construct the condensed graph:
      // each strongly connected component is collapsed into a single node, and
      // from the resulting sets of multiple arcs retain only those with minimum weight
//      Digraph c;
//      DoubleArcMap ww(c);
//      NodeNodeMap mapCToOrgG(c);
//      NodeNodeMap C2T(c);
//      ArcList newSortedArcs;
//
//      int new_i = constructCondensedGraph(g, w, mapToOrgG, G2T, sortedArcs,
//                                          comp, components, roots, j,
//                                          c, ww, mapCToOrgG, C2T, newSortedArcs);
//      
//      BoolArcMap newArcFilter(c, true);
//      BoolNodeMap newNodeFilter(c, true);
//      SubDigraph subC(c, newNodeFilter, newArcFilter);
//      
//      Node root = hd(c, ww, subC, mapCToOrgG, C2T, newSortedArcs, new_i);

      ArcList newSortedArcs;
      int new_i = condenseGraph(g, w, subG, mapToOrgG, G2T, sortedArcs, comp,
                                components, roots, j, newSortedArcs);
//      print(subG, w, std::cout);
//      int mmm = newSortedArcs.size();
//      std::cout << mmm << std::endl;
      Node root = hd(g, w, subG, mapToOrgG, G2T, newSortedArcs, new_i);
      fixArcList(g, sortedArcs, j);
      
//      print(std::cout);
      
      return root;
    }
  }
  
  return lemon::INVALID;
}

int TarjanHD::constructCondensedGraph(const Digraph& g,
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
                                      ArcList& newSortedArcs)
{
  // construct the condensed graph:
  // each strongly connected component is collapsed into a single node, and
  // from the resulting sets of multiple arcs retain only those with minimum weight
  lemon::DynArcLookUp<Digraph> lookUpArc(c);
  
  // first construct the nodes
  const size_t numSCC = components.size();
  NodeVector nodesOfC(numSCC, lemon::INVALID);
  for (size_t k = 0; k < numSCC; ++k)
  {
    Node v = nodesOfC[k] = c.addNode();
    
    if (components[k].size() == 1)
    {
      mapCToOrgG[v] = mapToOrgG[*components[k].begin()];
      C2T[v] = G2T[*components[k].begin()];
    }
    else
    {
      mapCToOrgG[v] = lemon::INVALID;
      C2T[v] = roots[k];
    }
  }
  
  // next construct the arcs: O(m) time
  for (ArcListIt arcIt = sortedArcs.begin(); arcIt != sortedArcs.end(); ++arcIt)
  {
    Arc a = *arcIt;
    Node u = g.source(a);
    Node v = g.target(a);
    
    Node uu = nodesOfC[comp[u]];
    Node vv = nodesOfC[comp[v]];
    if (uu != vv)
    {
      Arc aa = lookUpArc(uu, vv);
      if (aa == lemon::INVALID)
      {
        aa = c.addArc(uu, vv);
        newSortedArcs.push_back(aa);
        ww[aa] = w[a];
      }
#ifdef DEBUG
      else
      {
        assert(w[a] > ww[aa]);
      }
#endif
    }
  }
  
  return get_i(newSortedArcs, ww, w[getArcByRank(sortedArcs, j)]);
}

int TarjanHD::condenseGraph(Digraph& g,
                            const DoubleArcMap& w,
                            SubDigraph& subG,
                            NodeNodeMap& mapToOrgG,
                            NodeNodeMap& G2T,
                            ArcList& sortedArcs,
                            const IntNodeMap& comp,
                            const NodeSetVector& components,
                            const NodeVector& roots,
                            int& j,
                            ArcList& newSortedArcs)
{
//  int m = static_cast<int>(sortedArcs.size());
//  int mm = lemon::countArcs(subG);
//  if (m != mm)
//  {
//    print(sortedArcs, w, std::cout);
//    print(subG, w, std::cout);
//  }
  assert(sortedArcs.size() == lemon::countArcs(subG));
  
  // contract nodes belonging to the same component
  const size_t numSCC = components.size();
  for (size_t k = 0; k < numSCC; ++k)
  {
    const NodeSet& component = components[k];   
    if (component.size() > 1)
    {
      Node v  = *component.begin();
      G2T[v] = roots[k];
      mapToOrgG[v] = lemon::INVALID;
      
      NodeSetIt nodeIt = component.begin();
      while (++nodeIt != component.end())
      {
        assert(g.valid(v));
        assert(g.valid(*nodeIt));
        g.contract(v, *nodeIt, false);
      }
    }
  }
  
  // remove multiple arcs
  BoolMatrix arcPresent(numSCC, BoolVector(numSCC, false));
  
//  m = sortedArcs.size();
  for (ArcListNonConstIt arcIt = sortedArcs.begin(); arcIt != sortedArcs.end();)
  {
    Arc a = *arcIt;
    Node u = g.source(a);
    Node v = g.target(a);
       
    if (u != v && subG.status(u) && subG.status(v) && !arcPresent[comp[u]][comp[v]])
    {
//      std::cout << g.id(u) << " -> "
//                << g.id(v) << ", "
//                << (subG.status(a) ? "enabled " : "disabled")
//                << std::endl;
      newSortedArcs.push_back(a);
      
//      // arcs were disabled because of SCC on G_j
//      subG.enable(a);
      ++arcIt;
//      std::cout << "." << std::endl;
    }
    else if (subG.status(u) && subG.status(v))
    {
//      std::cout << ".." << std::endl;
      arcIt = sortedArcs.erase(arcIt);
      g.erase(a);
      --j;
    }
    else
    {
//      std::cout << "..." << std::endl;
      ++arcIt;
    }
  }
  
//  mm = newSortedArcs.size();
//  int mmm = lemon::countArcs(subG);
  assert(newSortedArcs.size() == lemon::countArcs(subG));

  // TODO: i can be determined on the fly
  return get_i(newSortedArcs, w, w[getArcByRank(sortedArcs, j)]);
}
