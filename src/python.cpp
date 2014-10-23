/*
 *  python.cpp
 *
 *   Created on: 23-oct-2014
 *       Author: M. El-Kebir
 */

#include <boost/numpy.hpp>
#include <boost/scoped_array.hpp>
#include <iostream>

#include "hd.h"
#include "naivehd.h"
#include "tarjanhd.h"
#include <lemon/connectivity.h>

namespace p = boost::python;
namespace np = boost::numpy;

void read_graph(const double* array, int n, int row_stride, int col_stride,
                Digraph& g, DoubleArcMap& w)
{
  // we have a full graph
  g.reserveNode(n);
  g.reserveArc(n*(n-1));
  
  std::vector<Node> node(n, lemon::INVALID);

  // let's add the nodes
  for (int i = 0; i < n; ++i)
  {
    node[i] = g.addNode();
  }
  
  // let's add arcs and set their weights
  const double * row_iter = array;
  for (int i = 0; i < n; ++i, row_iter += row_stride)
  {
    const double * col_iter = row_iter;
    for (int j = 0; j < n; ++j, col_iter += col_stride)
    {
      if (i != j && *col_iter != 0)
      {
        Arc a = g.addArc(node[i], node[j]);
        w[a] = *col_iter;
      }
    }
  }
  
  if (!lemon::stronglyConnected(g))
  {
    PyErr_SetString(PyExc_TypeError, "Graph is not strongly connected");
    p::throw_error_already_set();
  }
}

void check_input(const np::ndarray& array)
{
  if (array.get_dtype() != np::dtype::get_builtin<double>())
  {
    PyErr_SetString(PyExc_TypeError, "Incorrect array data type");
    p::throw_error_already_set();
  }
  
  if (array.get_nd() != 2)
  {
    PyErr_SetString(PyExc_TypeError, "Incorrect number of dimensions");
    p::throw_error_already_set();
  }
  
  if (array.shape(0) != array.shape(1))
  {
    PyErr_SetString(PyExc_TypeError, "Input matrix is not squared");
    p::throw_error_already_set();
  }
}

p::tuple make_results(const Digraph& g, const HD& hd)
{
  // Initialize the Python runtime.
  Py_Initialize();
  
  const Digraph& T = hd.getT();
  IntNodeMap id(T, -1);
  
  p::list mapping;
  int i = 0;
  for (NodeIt vv(T); vv != lemon::INVALID; ++vv, ++i)
  {
    id[vv] = i;
    if (hd.toG(vv) != lemon::INVALID)
    {
      mapping.append(g.id(hd.toG(vv)));
    }
    else
    {
      mapping.append(p::object());
    }
  }
  
  p::list adj;
  for (NodeIt vv(T); vv != lemon::INVALID; ++vv, ++i)
  {
    Arc a = OutArcIt(T, vv);
    if (a != lemon::INVALID)
    {
      adj.append(id[T.target(a)]);
    }
    else
    {
      adj.append(p::object());
    }
  }
  
  p::list label;
  for (NodeIt vv(T); vv != lemon::INVALID; ++vv, ++i)
  {
    if (hd.toG(vv) == lemon::INVALID)
    {
      label.append(hd.label(vv));
    }
    else
    {
      label.append(p::object());
    }
  }
  
  return p::make_tuple(adj, mapping, label);
}

p::tuple wrap_tarjan(const np::ndarray& array)
{
  check_input(array);
  
  Digraph g;
  DoubleArcMap w(g);
  read_graph(reinterpret_cast<const double*>(array.get_data()), array.shape(0),
             array.strides(0) / sizeof(double), array.strides(1) / sizeof(double),
             g, w);
  
  TarjanHD thd(g, w);
  thd.run();
  
  return make_results(g, thd);
}

p::tuple wrap_naive(const np::ndarray& array)
{
  check_input(array);
  
  Digraph g;
  DoubleArcMap w(g);
  read_graph(reinterpret_cast<const double*>(array.get_data()), array.shape(0),
             array.strides(0) / sizeof(double), array.strides(1) / sizeof(double),
             g, w);
  
  NaiveHD nhd(g, w);
  nhd.run();

  return make_results(g, nhd);
}

np::ndarray wrap_example()
{
  const int n = 7;
  
  Py_Initialize();
  p::tuple shape = p::make_tuple(7, 7);
  np::dtype dtype = np::dtype::get_builtin<double>();
  
  double A[7][7] =
  {
    {  0, 10,  0,  0,  0,  0, 15, },
    { 12,  0, 30,  0,  0,  0,  0, },
    {  0,  0,  0,  0,  0,  0, 45, },
    {  0,  0,  6,  0, 16,  0, 14, },
    {  0,  0,  0, 13,  0,  8,  0, },
    { 26,  0,  0,  0,  0,  0, 20, },
    {  0, 35, 22,  0, 50,  0,  0, },
  };
  
  np::ndarray a = np::zeros(shape, dtype);
  
  double * row_iter = reinterpret_cast<double*>(a.get_data());
  size_t row_stride = a.strides(0) / sizeof(double);
  size_t col_stride = a.strides(1) / sizeof(double);
  for (int i = 0; i < n; ++i, row_iter += row_stride)
  {
    double * col_iter = row_iter;
    for (int j = 0; j < n; ++j, col_iter += col_stride)
    {
      *col_iter = A[i][j];
    }
  }
  
  return a;
}

BOOST_PYTHON_MODULE(hierarchical_decomposition)
{
  np::initialize();  // have to put this in any module that uses Boost.NumPy
  p::def("tarjan", wrap_tarjan);
  p::def("naive", wrap_naive);
  p::def("example_adjacency_matrix", wrap_example);
}

/*
import hierarchical_decomposition as hd
import numpy as np
a=np.array([[1,2,3],[4,500,6]],dtype=float)
b=np.array([[1,2],[4,500]],dtype=float)
hd.tarjan(b)
*/

//int main(int argc, char **argv)
//{
//    // This line makes our module available to the embedded Python intepreter.
//    PyImport_AppendInittab("example", &initexample); 
//
//    // Initialize the Python runtime.
//    Py_Initialize();
//
//    PyRun_SimpleString(
//        "import example\n"
//        "import numpy\n"
//        "z1 = numpy.zeros((5,6), dtype=float)\n"
//        "z2 = numpy.zeros((4,3), dtype=float)\n"
//        "example.fill1(z1)\n"
//        "example.fill2(z2)\n"
//        "print z1\n"
//        "print z2\n"
//    );
//    Py_Finalize();
//}
