__author__ = 'justme'


import sys

class Node :
    def __init__(self, node_idx):
        self.node_idx = node_idx
        self.vertices = []

class Vertex :
    def __init__(self, from_node, to_node, weight):
        self.to_node = to_node
        self.from_node = from_node
        self.weight = int(weight)

class Graph :
    def __init__(self):
        self.nodes = {}


def computeMST(graph, start_node_idx) :

    visited_node_idx = start_node_idx
    adjacentNodes = {}
    mstMap = {}
    minWeight = 0

    while(len(mstMap) < len(graph.nodes)) :

        visited_node = graph.nodes[visited_node_idx]
        mstMap[visited_node_idx] = minWeight
        for vertex in visited_node.vertices :
            if not mstMap.has_key(vertex.to_node.node_idx) :
                if adjacentNodes.has_key(vertex.to_node.node_idx) :
                    if adjacentNodes[vertex.to_node.node_idx] > vertex.weight :

                        adjacentNodes[vertex.to_node.node_idx] = vertex.weight
                else :
                    adjacentNodes[vertex.to_node.node_idx] = vertex.weight
        if(len(adjacentNodes) > 0) :
            visited_node_idx = min(adjacentNodes, key=adjacentNodes.get)
            minWeight = adjacentNodes[visited_node_idx]
            print (visited_node_idx, minWeight)

            del adjacentNodes[visited_node_idx]
        else :
            break


    return sum(mstMap.itervalues())

def test() :
    fileName = "edges.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    [nodes, edges] = f.readline().split()
    nodes = int(nodes)
    edges = int(edges)
    edge_idx = 0

    graph = Graph()

    start_node_idx = 0

    while edge_idx < edges :

        line = f.readline()
        [from_node_idx, to_node_idx, weight] = line.split()
        from_node_idx = int(from_node_idx)
        to_node_idx = int(to_node_idx)

        if edge_idx == 0 :
            start_node_idx = from_node_idx

        if from_node_idx not in graph.nodes :
            from_node = Node(from_node_idx)
            graph.nodes[from_node_idx] = from_node
        else :
            from_node = graph.nodes[from_node_idx]


        if to_node_idx not in graph.nodes :
            to_node = Node(to_node_idx)
            graph.nodes[to_node_idx] = to_node
        else :
            to_node = graph.nodes[to_node_idx]

        from_node.vertices.append(Vertex(from_node, to_node, weight))

        to_node.vertices.append(Vertex(to_node, from_node, weight))

        edge_idx += 1

    f.close()
    print computeMST(graph, start_node_idx)



if __name__ == '__main__': test()
