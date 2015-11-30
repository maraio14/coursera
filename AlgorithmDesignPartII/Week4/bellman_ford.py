__author__ = 'justme'
import graph

def bellman_ford(graph_, start_node) :
    #init phase
    weight = {}

    for node in graph_.nodes.values() :
        if node.node_idx == start_node.node_idx :
            weight[node.node_idx] = float(0)
        else :
            weight[node.node_idx] = float("inf")

    idx_max_path_len = 0
    while idx_max_path_len < len(graph_.nodes) - 1 :
        for node in graph_.nodes.values() :
            for edge in node.edges :
                if weight[edge.from_node.node_idx] + edge.weight < weight[edge.to_node.node_idx] :
                    weight[edge.to_node.node_idx] = weight[edge.from_node.node_idx] + edge.weight
        idx_max_path_len += 1

    found_negative_cycle = False
    for node in graph_.nodes.values() :
        for edge in node.edges :
            if weight[edge.from_node.node_idx] + edge.weight < weight[edge.to_node.node_idx] :
                found_negative_cycle = True
                break
        if found_negative_cycle :
            break

    if found_negative_cycle :
        return None
    else :
        return weight

def testBellmanFord() :
    graph_ = graph.readGraph("g3.txt", True)
    print(bellman_ford(graph_, graph_.nodes[len(graph_.nodes) - 1]))

if __name__ == "__main__": testBellmanFord()