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
import re
import random

from argparse import ArgumentParser

import networkx as nx

from networkx.drawing.nx_agraph import read_dot
from networkx.drawing.nx_agraph import to_agraph


def find_postdom_frontiers(graph, start):
    """ Create the strongly connected components (SCC's) as subgraphs of a
    GraphViz DOT digraph.

    It finds the SCC's of a GrahViz digraph and groups them as subgraph
    clusters in this format.

    Args:
        infile (string): The input file name expected to contain a GraphViz DOT
        digraph.

        outfile (string): The output file name to be created containing a
        GraphViz DOT digraph.

        mode (string): A string denoting the way that the multinode SCCs will
        be visually marked on the output graph. Defaults to the emptry string.
        Current values supported: ['subgraph', 'colour']
    """

    return sorted((u, sorted(df))
                  for u, df in nx.dominance_frontiers(graph, start).items())


#

if __name__ == '__main__':
    parser = ArgumentParser(
        description=
        'Find postdominance frontier for graphs specified GraphViz DOT format')
    parser.add_argument(
        '-f',
        '--files',
        dest='dotfiles',
        nargs='*',
        required=True,
        help='GraphViz DOT files')
    parser.add_argument(
        '-q',
        '--quiet',
        dest='quiet',
        default=False,
        action='store_true',
        help='silence output')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        '-s',
        '--start',
        dest='start_node',
        help='The start node of dominance computation')
    group.add_argument(
        '-l',
        '--start-label',
        dest='start_node_label',
        help='The start node of dominance computation')

    args = parser.parse_args()

    #

    if args.quiet:
        sys.stdout = None

    invalid_files = [f for f in args.dotfiles if not os.path.isfile(f)]

    if invalid_files:
        raise ValueError('Input files: {} do not exist'.format(invalid_files))

    start_node = args.start_node

    for f in args.dotfiles:
        dot_graph = read_dot(f)
        reverse_dot_graph = dot_graph.reverse()

        if args.start_node_label:
            for n in reverse_dot_graph.nodes():
                if reverse_dot_graph.node[n]['label'].find(
                        args.start_node_label):
                    start_node = n

        frontiers = find_postdom_frontiers(reverse_dot_graph, start_node)
        print(frontiers)

    sys.exit(0)
