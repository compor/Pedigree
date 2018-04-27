#!/usr/bin/env python

#
# input graph from graphviz file and generate another graphviz file with
# strongly connected components displayed as clusters
#

from __future__ import print_function

import sys
import os

import networkx as nx

from networkx.drawing.nx_agraph import read_dot
from networkx.drawing.nx_agraph import to_agraph


def scc_print(infile, outfile):
    if os.path.exists(outfile):
        raise ValueError("Output file already exists")

    dot_graph = read_dot(infile)
    scc_graph = to_agraph(dot_graph)

    scc_gen = nx.strongly_connected_components(dot_graph)

    for i, scc in enumerate(scc_gen):
        if len(scc) > 1:
            subgraph = scc_graph.subgraph(list(scc), 'cluster_scc_' + str(i))
            subgraph.graph_attr.update(label='')

    scc_graph.write(outfile)


#

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(
            'Usage: {} [infile] [outfile]\n'.format(sys.argv[0]),
            file=sys.stderr)
        exit(1)

    scc_print(sys.argv[1], sys.argv[2])
