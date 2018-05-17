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

    return nx.is_isomorphic(dot_graph1, dot_graph2)


#

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(
            'Usage: {} [dotfile1] [dotfile2]\n'.format(sys.argv[0]),
            file=sys.stderr)
        exit(1)

    if not os.path.exists(sys.argv[1]):
        raise ValueError("Input file does not exist")

    if not os.path.exists(sys.argv[2]):
        raise ValueError("Input file does not exist")

    if check_graph_iso(sys.argv[1], sys.argv[2]):
        print('OK')
    else:
        print('Error')
