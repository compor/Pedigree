#!/usr/bin/env python
""" A command line utility that checks if two digraphs in GraphViz DOT format
are isomorphic.

The utility accepts the names of the two files containing GraphViz DOT digraphs
and outputs the string "OK" if they are isomorphic.
"""

from __future__ import print_function

import sys
import os

from argparse import ArgumentParser

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


def are_isomorphic_with_attribute(g1, g2, node_attribute):
    are_iso = True

    while are_iso and len(g1):
        first_node = list(g1.nodes(data=True))[0]

        found = find_nodes_with_attributes(
            g2, **{node_attribute: first_node[1][node_attribute]})

        if found:
            g2.remove_nodes_from(found)
            g1.remove_node(first_node[0])
            are_iso = nx.is_isomorphic(g1, g2)
        else:
            are_iso = False

    return are_iso


def check_graph_iso(dotfile1, dotfile2, node_attr=[]):
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

    for na in node_attr:
        if are_iso:
            are_iso = are_isomorphic_with_attribute(
                g1=dot_graph1, g2=dot_graph2, node_attribute=na)
        else:
            break

    return are_iso


#

if __name__ == '__main__':
    parser = ArgumentParser(
        description='Check graphs in GraphViz DOT format for isomorphism')
    parser.add_argument(
        '-f',
        dest='dotfiles',
        nargs='*',
        required=True,
        help='GraphViz DOT files')
    parser.add_argument(
        '-a',
        dest='node_attributes',
        default=[],
        nargs='+',
        help='Graph node attributes')

    args = vars(parser.parse_args())

    #

    if check_graph_iso(
            dotfile1=args['dotfiles'][0],
            dotfile2=args['dotfiles'][1],
            node_attr=args['node_attributes']):
        print('OK')
    else:
        print('Error')
