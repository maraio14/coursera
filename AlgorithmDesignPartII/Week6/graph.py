__author__ = 'justme'

class Node :
    def __init__(self, node_idx):
        self.node_idx = node_idx
        self.edges = {}
        self.visit_idx = None #numbers the nodes consecutively in the order in which they are discovered
        self.low_link = None #the smallest index of any node reachable from this node, including itself


class Edge :
    def __init__(self, to_node):
        assert( isinstance(to_node, Node) )
        self.to_node = to_node

class Graph :
    def __init__(self, nodes_count):
        self.nodes = {}
        self.nodes_count = nodes_count


def readGraph(fileName) :

    f = open(fileName, 'rt')
    f.seek(0)
    nodes = f.readline()
    nodes_count = int(nodes)

    graph = Graph(nodes_count)

    is_clause = True

    while is_clause :

        line = f.readline().split()
        if len(line) < 2 :
            is_clause = False
        else :
            
            from_node_idx = (int(line[0]))
            if from_node_idx not in graph.nodes :
                from_node = Node(from_node_idx)
                graph.nodes[from_node_idx] = from_node
            else:
                from_node = graph.nodes[from_node_idx]

            from_node_idx_neg = (-from_node_idx)
            if from_node_idx_neg not in graph.nodes :
                from_node_neg = Node(from_node_idx_neg)
                graph.nodes[from_node_idx_neg] = from_node_neg
            else:
                from_node_neg = graph.nodes[from_node_idx_neg]
                
            to_node_idx = (int(line[1]))
            if to_node_idx not in graph.nodes :
                to_node = Node(to_node_idx)
                graph.nodes[to_node_idx] = to_node
            else :
                to_node = graph.nodes[to_node_idx]

            to_node_idx_neg = (-to_node_idx)
            if to_node_idx_neg not in graph.nodes :
                to_node_neg = Node(to_node_idx_neg)
                graph.nodes[to_node_idx_neg] = to_node_neg
            else :
                to_node_neg = graph.nodes[to_node_idx_neg]

            from_node_neg.edges[to_node.node_idx] = Edge(to_node)
            to_node_neg.edges[from_node.node_idx] = Edge(from_node)
    f.close()

    return graph


class SCCTarjan:
    def __init__(self, graph):
        self.graph = graph
        self.S = []
        self.dictS = {}
        self.sccs = []
        self.current_visit_idx = -1

    def search_stack(self, node):
        if node.node_idx in self.dictS:
            return True
        return False

    def append_stack(self, node):
        self.S.append(node)
        self.dictS[node.node_idx] = None

    def pop_stack(self):
        node = self.S.pop()
        del self.dictS[node.node_idx]
        return node

    def strongconnect(self, node) :
        node.visit_idx = self.current_visit_idx
        node.low_link = self.current_visit_idx
        self.append_stack(node)
        self.current_visit_idx += 1

        for idx, edge in node.edges.items():
            if edge.to_node.visit_idx is None:
                self.strongconnect(edge.to_node)
                node.low_link = min(node.low_link, edge.to_node.low_link)
            elif self.search_stack(edge.to_node):
                node.low_link = min(node.low_link, edge.to_node.low_link)

        # If v is a root node, pop the stack and generate an SCC
        if node.low_link == node.visit_idx:
            list_scc = []
            found_scc = True
            while found_scc:
                scc_node = self.pop_stack()
                list_scc.append(scc_node)
                if(scc_node.node_idx == node.node_idx) :
                    found_scc = False

            self.sccs.append(list_scc)

    def SCCTarjan(self) :
        self.S.clear()
        self.current_visit_idx = 0
        for node_idx in self.graph.nodes :
            node = self.graph.nodes[node_idx]
            if node.visit_idx is None :
                self.strongconnect(node)

def testLoadGraph(filename):
    sccTarjan = SCCTarjan(readGraph(filename))
    sccTarjan.SCCTarjan()
    for scc in (sccTarjan.sccs):
        for node in scc:
            print(str(node.node_idx) + "|" , end='')
        print()
        #print (len(scc))

if __name__ == '__main__':
    testLoadGraph("TC11NSAT.txt")
