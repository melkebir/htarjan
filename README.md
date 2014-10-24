Hierarchical decomposition
==========================

Given a strongly connected, directed graph *G = (V, E)* with edge weights *w : E -> R*, its hierarchical decomposition is constructively defined as follows. We iteratively consider the edges in increasing order by weights. At iteration *i* we compute the strongly connected components on the graph *G_i = (V, E_i)*. Initially at iteration i = 0, *E_i* is the empty set and we thus have |V| strongly connected components consisting of single nodes each. At subsequent iterations, the strongly connected components group together until at iteration *|E|* all nodes are part of one single strongly connected component.

A hierarchical decomposition can be represented as a tree *T* whose leaves correspond to the node set *V*, and whose internal nodes are labeled by an edge weight of *G*. Given a weight *c*, removing all internal nodes of *T* that are labeled by a larger weight than *c* results in a forest. The leaves of the trees of this forest correspond to the strongly connected components of the subgraph *G' = (V, E')* where *E'* consists of all edges of *E* that have weight at most *c*.

This package contains an implementation of Tarjan's algorithm that runs in *O(|E| log |V|)* time [1]. Note that we do not require the edge weights to be distinct.

Dependencies
------------

* LEMON graph library (http://lemon.cs.elte.hu/)
* Boost.NumPy (https://github.com/ndarray/Boost.NumPy)
* Boost.Python (http://www.boost.org/doc/libs/1_56_0/libs/python/doc)

Also the `numpy` Python package needs to be installed.

Compilation instructions
------------------------

First `LEMON` needs to be installed:

    wget http://lemon.cs.elte.hu/pub/sources/lemon-1.3.tar.gz
    tar xvzf lemon-1.3.tar.gz
    cd lemon-1.3
    cmake -DCMAKE_INSTALL_PREFIX=~/lemon
    make install

Next install `Boost.NumPy`:

    git clone https://github.com/ndarray/Boost.NumPy.git
    cd Boost.NumPy
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=~/Boost.NumPy -DLIBRARY_TYPE=STATIC -DBUILD_EXAMPLES=OFF -DBUILD_TEST=OFF ..
    make install
  
Finally, compile `htarjan`:

    git clone <HTTPS clone URL (see on the right side of this page)>
    cd htarjan
    mkdir build
    cd build
    cmake ..
    make

Run instructions
----------------

```
$ ipython
In [1]: import hierarchical_decomposition as hd

In [2]: import numpy as np

In [3]: hd.tarjan(hd.example_adjacency_matrix())
Out[3]: 
([1, None, 3, 1, 5, 3, 8, 8, 1, 11, 11, 5],
 [5, None, 2, None, 6, None, 4, 3, None, 0, 1, None],
 [None, 50.0, None, 45.0, None, 35.0, None, None, 16.0, None, None, 12.0])
```

[1] Tarjan, R. E. (1983). An improved algorithm for hierarchical clustering using strong components. Information Processing Letters, 17(1), 37–41.
