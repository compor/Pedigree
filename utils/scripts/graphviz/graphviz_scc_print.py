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

from argparse import ArgumentParser

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


def get_filename_parts(filename, default_suffix=''):
    """ Parses a string representing a filename and returns as a 2-element
    string tuple with the filename stem and its suffix (the shortest string
    after a dot from the end of the string). If there's no suffix found a
    default is used instead.

    Args:
        filename(string): The input file name expected to contain a GraphViz DOT
        digraph.
        default_suffix(string, optional): The suffix to use if one cannot be
        found from the filename. Defaults to an empty string.

    Returns (string, string): A 2 element tuple of the filename stem and its
    suffix.
    """
    m = re.match(r'(.*)(\..*$)', filename)
    if not m:
        return (filename, default_suffix)

    return m.group(1, 2)


#

if __name__ == '__main__':
    parser = ArgumentParser(
        description='Mark SCCs in graphs specified GraphViz DOT format')
    parser.add_argument(
        '-f',
        '--files',
        dest='dotfiles',
        nargs='*',
        required=True,
        help='GraphViz DOT files')
    parser.add_argument(
        '-s',
        '--suffix',
        dest='suffix',
        default='scc',
        help='output file supplementary suffix')
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

    for f in args['dotfiles']:
        cur_prefix, cur_suffix = get_filename_parts(f)
        new_filename = cur_prefix + '.' + args['suffix'] + cur_suffix
        print('generating file: {}'.format(new_filename))
        create_scc_subgraphs(f, new_filename)

    sys.exit(0)
