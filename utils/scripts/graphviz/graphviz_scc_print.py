#!/usr/bin/env python
""" A command line utility that outputs the strongly connected components
(SCC's) subgraphs of a digraph as GraphViz DOT clusters.

The utility accepts the name of a file containing a GraphViz DOT digraph and
outputs a file with the given name of a digraph in the same format with all the
SCC's containing more than 1 nodes grouped as subgraph clusters.
"""

from __future__ import print_function

import sys
import os

import networkx as nx

from networkx.drawing.nx_agraph import read_dot
from networkx.drawing.nx_agraph import to_agraph


def create_scc_subgraphs(infile, outfile):
    """ Create the strongly connected components (SCC's) as subgraphs of a
    GraphViz DOT digraph.

    It finds the SCC's of a GrahViz digraph and groups them as subgraph
    clusters in this format.

    Args:
        infile (string): The input file name expected to contain a GraphViz DOT
        digraph.

        outfile (string): The output file name to be created containing a
        GraphViz DOT digraph.
    """

    dot_graph = read_dot(infile)
    scc_graph = to_agraph(dot_graph)

    scc_gen = nx.strongly_connected_components(dot_graph)

    for i, scc in enumerate(scc_gen):
        if len(scc) > 1:
            subgraph = scc_graph.subgraph(list(scc), 'cluster_scc_' + str(i))
            subgraph.graph_attr.update(label='')

    # clean up attribute label copied over by graph
    scc_graph.edge_attr.update(label='')
    scc_graph.node_attr.update(label='')
    [e.attr.update(label='') for e in scc_graph.edges()]

    scc_graph.write(outfile)


#

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(
            'Usage: {} [infile] [outfile]\n'.format(sys.argv[0]),
            file=sys.stderr)
        exit(1)

    if not os.path.exists(sys.argv[1]):
        raise ValueError('Input file: {} does not exist'.format(sys.argv[1]))

    create_scc_subgraphs(sys.argv[1], sys.argv[2])
