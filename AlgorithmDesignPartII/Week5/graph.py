__author__ = 'justme'

from math import sqrt, pow

class Node :
    def __init__(self, node_idx, xcoord, ycoord):
        self.node_idx = node_idx
        self.edges = []
        self.xcoord = xcoord
        self.ycoord = ycoord

class Edge :
    def __init__(self, from_node, to_node, weight):
        assert( isinstance(from_node, Node) )
        assert( isinstance(to_node, Node) )
        self.to_node = to_node
        self.from_node = from_node
        self.weight = weight

class TSPGraph :
    def __init__(self, nodes_count):
        self.nodes = {}
        self.nodes_count = nodes_count

    @staticmethod
    def dist(from_node, to_node) :
        assert(isinstance(from_node, Node))
        assert(isinstance(to_node, Node))
        return sqrt(pow(to_node.xcoord - from_node.xcoord, 2) + pow(to_node.ycoord - from_node.ycoord, 2))


def readTspGraph(fileName, is_directed = False) :

    f = open(fileName, 'rt')
    f.seek(0)
    nodes = f.readline()
    nodes_count = int(nodes)
    nodes_idx = 0

    graph = TSPGraph(nodes_count)

    while nodes_idx < nodes_count :

        line = f.readline()
        [xcoord, ycoord] = line.split()
        xcoord = float(xcoord)
        ycoord = float(ycoord)

        graph.nodes[nodes_idx] = Node(nodes_idx, xcoord, ycoord)

        nodes_idx += 1

    f.close()

    #print (TSPGraph.dist(graph.nodes[0], graph.nodes[1]))

    return graph
