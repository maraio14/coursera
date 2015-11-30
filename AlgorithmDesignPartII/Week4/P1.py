__author__ = 'justme'

import graph
import bellman_ford
import dijkstra

def augment_graph(graph_) :
    start_node = graph.Node(len(graph_.nodes) )

    for node in graph_.nodes.values() :
        start_node.edges.append(graph.Edge(start_node ,node, 0))
    graph_.nodes[start_node.node_idx] = start_node

    graph_.nodes_count += 1
    graph_.nodes_count += len(start_node.edges)
    return start_node

def johnson (graph_) :
    start_node = augment_graph(graph_)
    bf_shortest_paths = bellman_ford.bellman_ford(graph_, start_node)

    if not bf_shortest_paths is None :
        for node in graph_.nodes.values() :
            for edge in node.edges :
                edge.weight += (bf_shortest_paths[edge.from_node.node_idx] - bf_shortest_paths[edge.to_node.node_idx])


        sh_shortest_paths = {int:{}} #[[float("inf") for idx_to in range(graph_.nodes_count - 1)] for idx_from in range(graph_.nodes_count - 1)]
        for node in graph_.nodes.values() :
            if(node.node_idx != start_node.node_idx) :
                weight_from_aug = bf_shortest_paths[node.node_idx]
                dij_sh_paths = dijkstra.dijkstra(graph_, node)
                del dij_sh_paths[start_node.node_idx]
                for dij_idx in dij_sh_paths.keys() :
                    dij_sh_paths[dij_idx]  -=  (weight_from_aug - bf_shortest_paths[dij_idx])
                sh_shortest_paths[node.node_idx] = dij_sh_paths
        return sh_shortest_paths
    else:
        return None

def computeShShortestPaths(filename):
    sh_shortest_paths = johnson(graph.readGraph(filename, True))
    if (sh_shortest_paths) :
        min_dist = float("inf")
        for map_line in sh_shortest_paths.values() :
            for elem in map_line.values() :
                if elem < min_dist :
                    min_dist = elem
        return min_dist
    else :
        return None

def sh_shortert_path(filenames) :
    shortest_paths=[]
    for filename in filenames :
        shortest_paths.append(computeShShortestPaths(filename))
    print(shortest_paths)

if __name__ == "__main__":
    #sh_shortert_path(["test1.txt", "Question4.txt"])
    sh_shortert_path(["g1.txt", "g2.txt", "g3.txt"])
