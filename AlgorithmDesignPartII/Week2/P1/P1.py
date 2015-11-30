__author__ = 'justme';
from binary_heap import BinaryHeap, PriorityDiff

import os

def MaxSpacing(clusters_count) :

    fileName = "clustering1.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    nodes_count = int(f.readline())

    heap_edges = BinaryHeap(PriorityDiff())

    if nodes_count != 0 :
        edge_idx = 0
        is_edge = True
        while is_edge:
            line = f.readline().split()

            if len(line) == 3 :
                #print line
                (node_from, node_to, weight) = (int(line[0]), int(line[1]), int(line[2]))
                heap_edges.insert(((node_from, node_to, weight)))
                edge_idx += 1
            else:
                is_edge = False

        # (cluster, {node, None})
        clusters = {x : {x:None} for x in xrange(1, nodes_count+1)}
        node_at_cluster = {node: node for node in xrange(1, nodes_count+1)}

        while len(clusters) > clusters_count and not heap_edges.is_empty():
            (node_from, node_to, weight) = heap_edges.extract()
            cluster_from = node_at_cluster[node_from]
            cluster_to = node_at_cluster[node_to]
            if (cluster_from != cluster_to) :
                cluster_new = min(cluster_from, cluster_to)
                cluster_to_del = max(cluster_from, cluster_to)

                nodes_to_remove =  clusters.pop(cluster_to_del)
                for node in nodes_to_remove :
                    clusters[cluster_new][node] = None
                    node_at_cluster[node] = cluster_new

        foundMaxSpacing = False
        maxSpacing = 0
        while not foundMaxSpacing :
            (node_from, node_to, maxSpacing) = heap_edges.extract()
            if node_at_cluster[node_from] != node_at_cluster[node_to] :
                foundMaxSpacing = True
        if(foundMaxSpacing) :
            print "MaxSpacing: " + str(maxSpacing)
        else :
            print "Nothing"

    f.close()



# #####################################################################
######################################################################
if __name__ == '__main__': MaxSpacing(4)

