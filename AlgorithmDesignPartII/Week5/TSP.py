__author__ = 'justme'

import graph


class Key :
    def __init__(self, items):
        self.items = items
    def __eq__(self, other):
        return self.items == other.items
    def __ne__(self, other):
        return not self.__eq__(other)
    def __hash__(self):
        return hash(repr(self))
    def __repr__(self):
        return str(self.items)


def NChooseK(N, K) :
    if K == 0 :
        return
    allComb = []
    comb = []
    stack = [x for x in range(0, K)]

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
                #yield comb
        else :
            stack[idx_stack] += 1
            idx_stack = K-1
            comb = stack[:]
            #yield comb
            allComb.append(comb)

    return allComb

def choose_count(n, k):
    """
    A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
    """
    if 0 <= k <= n:
        ntok = 1
        ktok = 1
        for t in range(1, min(k, n - k) + 1):
            ntok *= n
            ktok *= t
            n -= 1
        return ntok // ktok
    else:
        return 0


def tsp(filename):
    tspgraph = graph.readTspGraph(filename, is_directed = False)

    #base case
    A = {}
    first_item = None
    for S in (NChooseK(tspgraph.nodes_count, 1)) :
        key = Key(S)
        A[key] = [float("inf") ]* tspgraph.nodes_count
        if(first_item is None) :
            first_item = key
            A[key][key.items[0]] = 0


    for m in range(2, tspgraph.nodes_count + 1) : #m=subproblem size
        print ("m="+str(m))
        Anext = {}

        nChooseK = NChooseK(tspgraph.nodes_count-1, m - 1)
        
        print("Processing nChooseK options # " + str(len(nChooseK)))
        for S in nChooseK : #for each set containing start element
          
            #skip the first element
            for idx in range(len(S)):
                S[idx] += 1

            new_key = Key( first_item.items + S)
            Anext[new_key] = [float("inf") ]* tspgraph.nodes_count

            #S.insert(0, first_item.items[0])

            for j in range(1,len(new_key.items)): #for each j in S, j != 1
                before = []

                old_key = Key(new_key.items[:j] + new_key.items[j+1:])
                old_value = A[old_key]

                min_dist_to_idx = float("inf")
                for k in range(len(new_key.items)):
                    if (k != j) :
                        min_dist_to_idx = min(min_dist_to_idx, old_value[new_key.items[k]] + graph.TSPGraph.dist(tspgraph.nodes[new_key.items[j]], tspgraph.nodes[new_key.items[k]]))
                Anext[new_key][new_key.items[j]] = min_dist_to_idx
        A.clear()
        A = Anext

    assert(len(A.items()) == 1)
    min_dist_to_first = float("inf")
    for key in A.keys() :
        values = A[key]
        for idx_node in range(1, len(values)) :
            min_dist_to_first = min(min_dist_to_first, values[idx_node] + graph.TSPGraph.dist(tspgraph.nodes[idx_node], tspgraph.nodes[0]) )
        break


    return min_dist_to_first



if __name__ == "__main__":
    # print("TC2.txt -> " + str(tsp("TC2.txt")))
    # print("TC3.txt -> " + str(tsp("TC3.txt")))
    # print("TC4.txt -> " + str(tsp("TC4.txt")))
    # print("TC5.txt -> " + str(tsp("TC5.txt")))
    # print("TC6.txt -> " + str(tsp("TC6.txt")))
    # print("TC8.txt -> " + str(tsp("TC8.txt")))
    #print("TC9.txt -> " + str(tsp("TC9.txt")))
    print("tsp.txt -> " + str(tsp("tsp.txt")))
    # TC2.txt -> 4.0
    # TC3.txt -> 10.47213595499958
    # TC4.txt -> 6.179858814879626
    # TC5.txt -> 6.265332236315541
    # TC6.txt -> 124.96585981781078
    # TC8.txt -> 16898.13352231527
    # TC9.txt -> 26714.87715804676
