#!/usr/bin/env python
""" A command line utility that outputs the post-dominance frontier starting at
the selected node of a digraph input as GraphViz DOT.

The utility accepts the name of a file containing a GraphViz DOT digraph and a
start node and outputs a dict containing the post-dominance frontiers of each
node reachable from start as lists.
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
    """ Returns the post-dominance frontiers of all nodes in a networkx.DiGraph

    Args:
        graph (networkx.DiGraph or networkx.MultiDiGraph): The input digraph.

        start (string): The output file name to be created containing a
        GraphViz DOT digraph.


    Returns:
       df : dict keyed by nodes
        A dict containing the dominance frontiers of each node reachable from
        `start` as lists.

    Raises:
        Same as networkx.algorithms.dominance.dominance_frontiers()
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
