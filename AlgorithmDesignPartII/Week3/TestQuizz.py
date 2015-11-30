__author__ = 'justme'

def Print(mtx) :
    print ("20*")
    for row in mtx :
        print (row)

def OptimalBST(freqs) :

    matrix = [ [ 0 for col in range(len(freqs))] for row in range(len(freqs)) ]

    for s in range(len(freqs)) :

        for i in range(len(freqs) - s) :

            min_cost = float("inf")
            sumpk = 0
            for r in range(i , i + s + 1 ) :
                sumpk += freqs[r]
            for r in range(i , i + s + 1 ) :
                item1 = 0
                if(i <= r-1) :
                    item1 = matrix[i][r-1]
                item2 = 0
                if(r + 1 <= i + s) :
                    item2 = matrix[r +1][i + s]
                cost = sumpk + item1 + item2
                min_cost = min(cost, min_cost)
            matrix[i][i+s] = min_cost

        Print(matrix)

if __name__ == '__main__': OptimalBST([0.2, 0.05, 0.17, 0.1, 0.20, 0.03, 0.25])