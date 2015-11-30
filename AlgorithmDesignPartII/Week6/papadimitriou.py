__author__ = 'justme'
import math
import random
import sys

class Clause:
    def __init__(self, var1, nonvar1, var2, nonvar2):
        assert(isinstance(var1, int))
        assert(isinstance(var2, int))
        assert(isinstance(nonvar1, bool))
        assert(isinstance(nonvar2, bool))
        self.var1 = var1
        self.nonvar1 = nonvar1
        self.var2 = var2
        self.nonvar2 = nonvar2

def EvalClause(clause, var1, var2):
    if clause.nonvar1 :
        var1 = not var1
    if clause.nonvar2 :
        var2 = not var2
    return (var1 | var2)

def Papadimitriou(filename) :
    f = open(filename, 'rt')
    f.seek(0)
    variables = f.readline()
    variables_count = int(variables)

    clausesVar1 = {}
    clausesVar2 = {}
    is_clause = True

    while is_clause :
        line = f.readline().split()

        if len(line) != 2 :
            is_clause = False
        else:
            var1 = int(line[0])
            nonvar1 = (var1<0)
            var1 = abs(var1)
            var2 = int(line[1])
            nonvar2 = (var2<0)
            var2 = abs(var2)

            clause = Clause(var1, nonvar1, var2, nonvar2)
            if var1 not in clausesVar1:
                clausesVar1[var1] = []
            clausesVar1[var1].append(clause)
            if var2 not in clausesVar2:
                clausesVar2[var2] =[]
            clausesVar2[var2].append(clause)
    f.close()

    outer_bound = math.ceil(math.log2(variables_count))
    inner_bound = 2*pow(variables_count, 2)

    for outer_idx in range(outer_bound) :

        #random init solution
        init_sol = [False]
        list_items = [False,True]
        for idx_var in range(variables_count) :
            init_sol.append(random.choice(list_items))
        #init_sol = int(init_sol,2)

        flips_idx = 0
        while flips_idx < inner_bound :
            #Try a first scan to all the clauses to check if we have a correct solution only by initial randomization
            has_sat = True
            for var1_idx in clausesVar1:
                for clause in clausesVar1[var1_idx] :
                    var1 = init_sol[clause.var1]
                    var2 = init_sol[clause.var2]
                    eval_clause = EvalClause(clause, var1, var2)

                    if not eval_clause :
                        has_sat = False
                        break

                if not has_sat:
                    break

            if not has_sat:
                flips_to_perform = True
                random_idx_to_flip = random.choice([clause.var1,clause.var2])
                while flips_to_perform:
                    #random_idx_to_flip = random.choice([clause.var1,clause.var2])
                    init_sol[random_idx_to_flip] ^= True
                    flips_idx += 1 #no of flips increased

                    flips_to_perform = False

                    if random_idx_to_flip in clausesVar1 :
                        for clause in clausesVar1[random_idx_to_flip]:
                            var1 = init_sol[clause.var1]
                            var2 = init_sol[clause.var2]
                            eval_clause = EvalClause(clause, var1, var2)
                            if not eval_clause :
                                flips_to_perform = False
                                random_idx_to_flip = clause.var2
                                break
                    if random_idx_to_flip in clausesVar2 :  
                        for clause in clausesVar2[random_idx_to_flip]:
                            var1 = init_sol[clause.var1]
                            var2 = init_sol[clause.var2]
                            eval_clause = EvalClause(clause, var1, var2)
                            if not eval_clause :
                                flips_to_perform = False
                                random_idx_to_flip = clause.var1
                                break

            else:
                return 1
    return 0


# out = open("output23456.txt", 'wt')
# #out = sys.stdout
# print("Begin processing..", file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat1.txt")), end='', file=out)
# #print(str(1), end='', file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat2.txt")), end='', file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat3.txt")), end='', file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat4.txt")), end='', file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat5.txt")), end='', file=out)
# out.flush()
# out.close()
# out = open("output23456.txt", 'a')
# print(str(Papadimitriou("2sat6.txt")), end='', file=out)
# out.flush()
# out.close()


#out = open("output23456.txt", 'wt')
out = sys.stdout
print(str(Papadimitriou("TC1SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC2SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC3NSAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC4SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC5SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC6SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC7SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC8SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC9NSAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC10SAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC11NSAT.txt")), end='', file=out)
out.flush()
print(str(Papadimitriou("TC12SAT.txt")), end='', file=out)
out.flush()
out.close()
