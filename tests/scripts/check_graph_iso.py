#!/usr/bin/env python
"""A command line utility that checks if two digraphs in GraphViz DOT format
are isomorphic.

The utility accepts the names of the two files containing GraphViz DOT digraphs
and outputs the string "OK" if they are isomorphic. Optionally, it can also
check for matching values for node attributes between the graphs.
"""

from __future__ import print_function

import sys
import os

from itertools import tee

from argparse import ArgumentParser

import networkx as nx

from networkx.drawing.nx_agraph import read_dot


def find_nodes_with_attributes(g, **attr):
    """Find all the nodes in a graph with the specified attributes and their
    corresponding values.

    Args:
        g (graph): Graph object.
        attr: Arbitrary keyword arguments to use for finding matching graph
        nodes (same attribute names and corresponding values).

    Returns:
        list: A list of all the nodes found with matching attributes and
        their corresponding values.
    """

    found = []
    nodes = g.nodes(data=True)

    for k, v in attr.iteritems():
        for n in nodes:
            if n[1][k] == v:
                found.append(n[0])

    return found


def are_isomorphic_with_attribute(g1, g2, node_attribute):
    """Checks if 2 graphs are isomorphic by matching nodes with specific
    attributes and their corresponding values.

    Args:
        g1 (graph): Graph object.
        g2 (graph): Graph object.
        node_attribute (string): Name of the graph node attribute that is used
        to check for isomorphism.

    Returns:
        bool: True when graphs are isomorphic, false otherwise.
    """

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
    """Checks if two GraphViz DOT digraphs are isomorphic.

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


def pairwise(iterable):
    """Creates an iterator of pairwise elements from an iterables.

    If an iterable is a list as [a, b, c], it will return the following tuples
    in order: (a, b) (b, c).

    Args:
        dotfile1 (iterable): An iterable.

    Returns:
        iterator: An iterator.
    """
    a, b = tee(iterable)
    next(b)
    return zip(a, b)


#

if __name__ == '__main__':
    parser = ArgumentParser(
        description='Check graphs in GraphViz DOT format for isomorphism')
    parser.add_argument(
        '-f',
        '--files',
        dest='dotfiles',
        nargs='*',
        required=True,
        help='GraphViz DOT files')
    parser.add_argument(
        '-a',
        '--attributes',
        dest='node_attributes',
        default=[],
        nargs='+',
        help='Graph node attributes')
    parser.add_argument(
        '-q',
        '--quiet',
        dest='quiet',
        default=False,
        action='store_true',
        help='silence output')

    args = vars(parser.parse_args())

    #

    if args['quiet']:
        sys.stdout = None

    invalid_files = [f for f in args['dotfiles'] if not os.path.isfile(f)]

    if invalid_files:
        raise ValueError('Input files: {} do not exist'.format(invalid_files))

    if len(args['dotfiles']) < 2:
        raise ValueError('Less than 2 files were provided')

    status = True
    for dotfile, dotfile_next in pairwise(args['dotfiles']):
        status = check_graph_iso(
            dotfile, dotfile_next, node_attr=args['node_attributes'])

        if not status:
            break

    print('OK' if status else 'Error')

    sys.exit(0)
