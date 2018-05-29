#!/usr/bin/env python
""" A command line utility that checks if two digraphs in GraphViz DOT format
are isomorphic.

The utility accepts the names of the two files containing GraphViz DOT digraphs
and outputs the string "OK" if they are isomorphic.
"""

from __future__ import print_function

import sys
import os

import networkx as nx

from networkx.drawing.nx_agraph import read_dot


def find_nodes_with_attributes(g, **attr):
    found = []
    nodes = g.nodes(data=True)

    for k, v in attr.iteritems():
        for n in nodes:
            if n[1][k] == v:
                found.append(n[0])

    return found


def check_graph_iso(dotfile1, dotfile2):
    """ Checks if two GraphViz DOT digraphs are isomorphic.

    Args:
        dotfile1 (string): Input file name expected to contain a GraphViz DOT
        digraph.
        dotfile2 (string): Input file name expected to contain a GraphViz DOT
        digraph.

    Returns:
        bool: True when graphs are isomorphic, false otherwise.
    """

    dot_graph1 = read_dot(dotfile1)
    dot_graph2 = read_dot(dotfile2)

    are_iso = nx.is_isomorphic(dot_graph1, dot_graph2)

    while are_iso and len(dot_graph1):
        first_node = list(dot_graph1.nodes(data=True))[0]
        found = find_nodes_with_attributes(
            dot_graph2, dg_uid=first_node[1]['dg_uid'])

        if found:
            dot_graph2.remove_nodes_from(found)
            dot_graph1.remove_node(first_node[0])
            are_iso = nx.is_isomorphic(dot_graph1, dot_graph2)
        else:
            are_iso = False

    return are_iso


#

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(
            'Usage: {} [dotfile1] [dotfile2]\n'.format(sys.argv[0]),
            file=sys.stderr)
        exit(1)

    if not os.path.exists(sys.argv[1]):
        raise ValueError('Input file: {} does not exist'.format(sys.argv[1]))

    if not os.path.exists(sys.argv[2]):
        raise ValueError('Input file: {} does not exist'.format(sys.argv[2]))

    if check_graph_iso(dotfile1=sys.argv[1], dotfile2=sys.argv[2]):
        print('OK')
    else:
        print('Error')
