/*
 *  naivehd.h
 *
 *   Created on: 14-oct-2014
 *       Author: M. El-Kebir
 */

#ifndef NAIVEHD_H
#define NAIVEHD_H

#include <lemon/adaptors.h>
#include <vector>
#include <set>
#include <ostream>

#include "hd.h"

class NaiveHD : public HD
{
public:
  NaiveHD(const Digraph& g, const DoubleArcMap& w);
  
  void run();
  
private:
  typedef std::vector<Arc> ArcVector;
  typedef ArcVector::const_iterator ArcVectorIt;
};

#endif // NAIVEHD_H
