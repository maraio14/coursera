
import sys

__author__ = 'justme';


import os

def KnapsackRec( items, weight_idx, item_idx , map_solutions) :

    key = (weight_idx << sys.getsizeof(weight_idx)) | item_idx

    if(map_solutions.has_key(key)) :
        return map_solutions[key]

    (value, weight) = items[item_idx]

    prev_item_val = 0;
    if(item_idx >  0) :
        prev_item_val = KnapsackRec( items, weight_idx, item_idx - 1, map_solutions)


    if (weight_idx < weight) :
        return prev_item_val
    else :
        this_item_taken = KnapsackRec( items, weight_idx- weight, item_idx - 1, map_solutions) + value
        map_solutions[key] = max(prev_item_val, this_item_taken)
        return map_solutions[key]


def Knapsack( ) :
    fileName = "knapsack_big.txt"
    #fileName = "knapsack1.txt"
    #fileName = "test1.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    line = f.readline().split()

    (knapsack_size, number_of_items) = (int(line[0]), int(line[1]))

    oldlimit = sys.getrecursionlimit()

    sys.setrecursionlimit(max(knapsack_size, number_of_items))

    if number_of_items != 0 :
        item_idx = 0
        is_item= True
        items = []
        while is_item:
            line = f.readline().split()

            if len(line) == 2 :
                #print line
                (value, weight) = (int(line[0]), int(line[1]))
                items.append((value, weight))
                item_idx += 1
            else:
                is_item = False

        assert(item_idx == number_of_items)

    map_solutions = {}
    print ("Knapsack max. value = " + str(KnapsackRec(items,  knapsack_size - 1, number_of_items-1, map_solutions )))

    sys.setrecursionlimit(oldlimit)
    f.close()



# #####################################################################
######################################################################
if __name__ == '__main__': Knapsack()

