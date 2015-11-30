__author__ = 'justme'

class Node :
    def __init__(self, node_idx):
        self.node_idx = node_idx
        self.edges = []

class Edge :
    def __init__(self, from_node, to_node, weight):
        assert( isinstance(from_node, Node) )
        assert( isinstance(to_node, Node) )
        self.to_node = to_node
        self.from_node = from_node
        self.weight = weight

class Graph :
    def __init__(self, nodes_count, edges_count):
        self.nodes = {}
        self.nodes_count = nodes_count
        self.edges_count = edges_count

def readGraph(fileName, is_directed = False) :

    f = open(fileName, 'rt')
    f.seek(0)
    [nodes, edges] = f.readline().split()
    nodes_count = int(nodes)
    edges_count = int(edges)
    edge_idx = 0

    graph = Graph(nodes_count,edges_count)

    while edge_idx < edges_count :

        line = f.readline()
        [from_node_idx, to_node_idx, weight] = line.split()
        from_node_idx = int(from_node_idx) - 1
        to_node_idx = int(to_node_idx) -1
        weight = float(weight)

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

        from_node.edges.append(Edge(from_node, to_node, weight))
        if(not is_directed) :
            to_node.edges.append(Edge(to_node, from_node, weight))

        edge_idx += 1

    f.close()

    return graph
