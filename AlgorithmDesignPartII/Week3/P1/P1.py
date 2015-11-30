__author__ = 'justme';


import os

def Knapsack() :

    fileName = "knapsack1.txt"
    #fileName = "test1.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    line = f.readline().split()

    (knapsack_size, number_of_items) = (int(line[0]), int(line[1]))
    values_mtx = [[0 for item_idx in xrange(number_of_items)] for weight_idx in xrange(knapsack_size)]

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

        for item_idx in xrange(number_of_items) :
            for weight_idx in xrange(knapsack_size) :
                prev_item_val = 0;
                if(item_idx > 0) :
                    prev_item_val = values_mtx[weight_idx][item_idx - 1]

                (value, weight) = items[item_idx]
                if (weight_idx < weight) :
                    values_mtx[weight_idx][item_idx] = prev_item_val
                else :
                    this_item_taken = values_mtx[weight_idx- weight ][item_idx - 1] + value
                    values_mtx[weight_idx][item_idx] = max(prev_item_val, this_item_taken)

    print "Knapsack max. value = " + str(values_mtx[knapsack_size-1][number_of_items-1])

    f.close()



# #####################################################################
######################################################################
if __name__ == '__main__': Knapsack()

