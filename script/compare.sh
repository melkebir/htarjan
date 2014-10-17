#!/bin/bash
./makeFullGraph.py $1 > graph.txt
$2 -n graph.txt | sort > 1.txt
$2 graph.txt | sort > 2.txt
diff 1.txt 2.txt
