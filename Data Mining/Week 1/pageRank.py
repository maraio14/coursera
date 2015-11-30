
import sys


def googlePageRank(ls_nodes, nodes_count, beta, init_sum, epsilon, stop_iter=sys.maxsize):
    #init_sum = 3
    lsdict_pageRank = [{} for idx in range(0, nodes_count)]
    #the next one uses the same memory for each element
    #lsdict_pageRank  = [{}]*nodes_count
    for link in ls_nodes:
        dict_outLinks = lsdict_pageRank[link[0]]
        dict_outLinks[link[1]] = None

    for dict_outLinks in lsdict_pageRank :
        if(len(dict_outLinks) > 0): #otherwise is a dead end
            rij = 1./(len(dict_outLinks))
            for to_node in dict_outLinks.keys():
                dict_outLinks[to_node] = rij

    dict_inLinks= {}
    for idx in range(0, nodes_count) :
        dict_outLinks = lsdict_pageRank[idx]
        for key in dict_outLinks.keys():
            if key not in dict_inLinks:
                dict_inLinks[key] = []
            dict_inLinks[key].append(idx)

    print(lsdict_pageRank)
    print(dict_inLinks)
    is_epsilon = False
    R = [init_sum/nodes_count for idx in range(0, nodes_count)]
    Rnew = [0 for idx in range(0, nodes_count)]

    iter = 0
    while not is_epsilon:
        #print("R=" + str(R))
        for idx in range(0, nodes_count):
            sum_innodes = 0
            if idx not in dict_inLinks :
                Rnew[idx] = (1-beta)/nodes_count
            else :
                for in_node in dict_inLinks[idx] :
                    sum_innodes += R[in_node]*lsdict_pageRank[in_node][idx]
                Rnew[idx] = beta * sum_innodes + (1-beta)/nodes_count

        #print("Rnew no leaking=" + str(Rnew))
        S = sum(Rnew)

        sum_epsilon = 0
        leaking_amount = (init_sum-S)/nodes_count
        for idx in range(0, nodes_count):
            Rnew[idx] = Rnew[idx] + (leaking_amount)
            sum_epsilon += abs(Rnew[idx]-R[idx])

        if(sum_epsilon <= epsilon) :
            is_epsilon = True

        R = Rnew.copy()
        iter += 1

        if(iter <= stop_iter):
            print(lsdict_pageRank)
            print("iter#" + str(iter) +  " R=" + str(R) + " epsilon : " + str(sum_epsilon))

    return R

# print(googlePageRank([(0,1), (1,0), (0,0), (1,2), (2,2)], 3, .8, 1, .02))
# print(googlePageRank([(0,1), (1,0), (0,0), (1,2)], 3, .8, 1, .02))
#
# print ("No dead ends")
# print(googlePageRank([(0,1), (1,0), (0,0), (1,2), (2,2),(2,1),(2,0)], 3, .8, 1, 0.002))
#
# print("Dead ends")
# print(googlePageRank([(0,1), (1,0), (0,0), (1,2)], 3, .8,  1, .002))
#
#
# print ("No dead ends")
# print(googlePageRank([(0,1), (1,0), (0,0), (2,2),(2,1),(2,0)], 3, .8, 1, 0.002))
#
# print("Dead ends")
# print(googlePageRank([(0,1), (1,0), (0,0)], 3, .8, 1, .002))

# R =googlePageRank([(0,1),(0, 2), (1,2), (2,2) ], 3, .7, 3, .001)
# print(R)
# print ("R[0] + R[1] = " + str(R[0] + R[1]))
# print ("R[0] + R[2] = " + str(R[0] + R[2]))
# print ("R[1] + R[2] = " + str(R[1] + R[2]))

R =googlePageRank([(0,1),(0, 2), (1,2), (2,0) ], 3,  .85, 3, .001, 4)
print(R)
print(str(.85*R[2] - R[1] - .575*R[0]))
print(.95*R[1] - .475*R[0] - .05*R[2])
print(R[1] - .475*R[0] - .05*R[2])
print(R[0] - .9*R[2] - .05*R[1])

R =googlePageRank([(0,1),(0, 2), (1,2), (2,0) ], 3,  1., 3, .001, 4)
print(R)



R =googlePageRank([(0,1),(1, 2), (2,3), (3,0), (3,1) , (3,2) , (3,3) ], 4,  0, 1, .001)
print(R)