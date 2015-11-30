__author__ = 'justme'
import graph
import sys

class C2SATTarjan:
    def __init__(self, graph):
        self.graph = graph
        self.S = []
        self.dictS = {}
        #self.sccs = []
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
                if self.strongconnect(edge.to_node) :
                    node.low_link = min(node.low_link, edge.to_node.low_link)
                else:
                    return False
            elif self.search_stack(edge.to_node):
                node.low_link = min(node.low_link, edge.to_node.low_link)

        # If v is a root node, pop the stack and generate an SCC
        if node.low_link == node.visit_idx:
            map_scc = {}
            found_scc = True
            while found_scc:
                scc_node = self.pop_stack()
                if (-scc_node.node_idx) in map_scc:
                    return False
                else:
                    map_scc[scc_node.node_idx] = None

                if(scc_node.node_idx == node.node_idx) :
                    found_scc = False

            #self.sccs.append(map_scc)
        return True

    def check(self) :
        self.S.clear()
        self.current_visit_idx = 0
        for node_idx in self.graph.nodes :
            node = self.graph.nodes[node_idx]
            if node.visit_idx is None :
                if not self.strongconnect(node):
                    return 0
        return 1
                
def testSCCTarjan(filename):
    sccTarjan = C2SATTarjan(graph.readGraph(filename))
    return sccTarjan.check()


if __name__ == '__main__':

    out = sys.stdout
    print(str(testSCCTarjan("2sat1.txt")), end='', file=out)
    out.flush()
    print(str(testSCCTarjan("2sat2.txt")), end='', file=out)
    out.flush()
    print(str(testSCCTarjan("2sat3.txt")), end='', file=out)
    out.flush()
    print(str(testSCCTarjan("2sat4.txt")), end='', file=out)
    out.flush()
    print(str(testSCCTarjan("2sat5.txt")), end='', file=out)
    out.flush()
    print(str(testSCCTarjan("2sat6.txt")), end='', file=out)
    out.flush()



    # out = sys.stdout
    # print(str(testSCCTarjan("TC1SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC2SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC3NSAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC4SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC5SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC6SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC7SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC8SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC9NSAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC10SAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC11NSAT.txt")), end='', file=out)
    # out.flush()
    # print(str(testSCCTarjan("TC12SAT.txt")), end='', file=out)
    # out.flush()
    # out.close()
                
