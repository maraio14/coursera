__author__ = 'justme'

import graph
from pqdict import PQDict

def dijkstra(graph_, start_node) :

    #initialization phase
    min_prio_queue = PQDict()
    dist = {}

    for node in graph_.nodes.values() :
        if node.node_idx == start_node.node_idx :
             min_prio_queue[node] = float(0)
        else:
            min_prio_queue[node] = float("inf")
        dist[node.node_idx] = min_prio_queue[node]

    while (len(min_prio_queue) > 0) :
        min_node = min_prio_queue.top()
        min_node_dist = min_prio_queue[min_node]
        min_prio_queue.pop()

        for edge in  min_node.edges :
            neighbour_node = edge.to_node
            tentative_dist = min_node_dist + edge.weight
            if(tentative_dist < dist[neighbour_node.node_idx]):
                dist[neighbour_node.node_idx] = tentative_dist
                min_prio_queue[neighbour_node] = tentative_dist

    return dist

def testDijkstra() :
    graph_ = graph.readGraph("dijkstra1.txt", True)
    print(dijkstra(graph_, graph_.nodes[0]))

if __name__ == "__main__": testDijkstra()