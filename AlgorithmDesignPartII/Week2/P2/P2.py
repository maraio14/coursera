__author__ = 'justme';
#from binary_heap import BinaryHeap, PriorityDiff
import string

import os

def NChooseK(N, K) :
    if K == 0 :
        return
    allComb = []
    comb = []
    stack = [x for x in xrange(0,K)]

    comb = stack[:]
    allComb.append(comb)

    lim_end = N

    idx_stack = K-1

    while stack[0] != N-K :

        if stack[idx_stack] == N  - K + idx_stack:
            idx_stack -= 1
            if stack[idx_stack] < N  - K + idx_stack :
                stack[idx_stack] += 1

                idx_upStack = idx_stack + 1
                while idx_upStack < K:
                    stack[idx_upStack] = stack[idx_upStack - 1] + 1
                    idx_upStack += 1
                idx_stack = K-1

                comb = stack[:]
                allComb.append(comb)
        else :
            stack[idx_stack] += 1
            idx_stack = K-1
            comb = stack[:]
            allComb.append(comb)

    return allComb

def choose_count(n, k):
    """
    A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
    """
    if 0 <= k <= n:
        ntok = 1
        ktok = 1
        for t in xrange(1, min(k, n - k) + 1):
            ntok *= n
            ktok *= t
            n -= 1
        return ntok // ktok
    else:
        return 0


def get_equidistant_nodes(nodes_map, distance, bits_count) :
    pairs = []
    pairs_map = {}
    list_mutations = NChooseK(bits_count,  distance)
    mutation_masks = []

    for mutation in list_mutations:
        mask = 0
        for pos in mutation:
            mask |=  (1 << pos)
        mutation_masks.append(mask)

    for node in nodes_map.iterkeys() :
        possible_nodes = []
        for mutation_mask in mutation_masks :
            permutation_node = node ^ mutation_mask
            possible_nodes.append( permutation_node )
        for possible_node in possible_nodes :
            if nodes_map.has_key(possible_node) :
                if (node < possible_nodes) :
                    pair_key = (node<<bits_count)|possible_node
                else :
                    pair_key = (possible_node<<bits_count)|node
                if not pairs_map.has_key(pair_key) :
                    pairs_map[pair_key] = None
                    pairs.append((node, possible_node))


    return pairs

def MaxSpacing(max_spacing) :

    fileName = "clustering_big.txt"
    #fileName = "T10000.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    line = f.readline().split()
    (nodes_count, bits_count) = (int(line[0]), int(line[1]))

    format_bin = "{0:0"+str(bits_count)+"b}"
    clusters = {}
    distance = 1

    if nodes_count != 0 :
        nodes_map = {}
        node_at_cluster = {}

        idx_cluster = 0
        is_node = True
        while is_node:

            line = string.joinfields(f.readline().split(), '')
            if len(line) == bits_count :
                line_int = int(line, 2)
                if not nodes_map.has_key(line_int) :
                    nodes_map[line_int] = None
                    #print format_bin.format(line_int)
                    clusters[idx_cluster] = {line_int:None}
                    node_at_cluster[line_int] = idx_cluster
                    idx_cluster += 1
            else:
                is_node = False

        assert(len(nodes_map) == len(clusters))
        # (cluster, {node, None})
        #clusters = {x : {x:None} for x in xrange(1, nodes_count+1)  }
        #node_at_cluster = {node: node for node in xrange(1, nodes_count+1)}

        while distance < max_spacing:
            list_equidistant_nodes = get_equidistant_nodes(nodes_map, distance, bits_count)
            for (node_from, node_to) in list_equidistant_nodes :
                #(node_from, node_to, distance) = heap_edges.extract()
                cluster_from = node_at_cluster[node_from]
                cluster_to = node_at_cluster[node_to]
                if (cluster_from != cluster_to) :
                    cluster_new = min(cluster_from, cluster_to)
                    cluster_to_del = max(cluster_from, cluster_to)

                    nodes_to_remove =  clusters.pop(cluster_to_del)
                    for node in nodes_to_remove :
                        clusters[cluster_new][node] = None
                        node_at_cluster[node] = cluster_new
            distance += 1
    print "Clusters found #" + str(len(clusters))

    return clusters

    f.close()







# #####################################################################
######################################################################
if __name__ == '__main__': MaxSpacing(3)

