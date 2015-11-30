__author__ = 'justme'
import graph

def FloydWarshall(graph) :

    Ak = [ [float("inf") for to_idx in range(len(graph.nodes)) ] for from_idx in range(len(graph.nodes)) ]
    Akp1 = Ak.copy()

    #Init FloydWarshall initial matrix
    for node in graph.nodes.values() :
        Ak[node.node_idx][node.node_idx] = 0
        for edge in node.edges :
            Ak[node.node_idx][edge.to_node.node_idx] = edge.weight

    for new_node in graph.nodes.values() :
        for head in graph.nodes.values() :
            for tail in graph.nodes.values():
                Akp1[head.node_idx][tail.node_idx] = min(Ak[head.node_idx][tail.node_idx], Ak[head.node_idx][new_node.node_idx] + Ak[new_node.node_idx][tail.node_idx])
        Ak = Akp1

    found_negative_cycle = False
    for idx in range(len(graph.nodes)) :
        if(Ak[idx][idx] < 0) :
            found_negative_cycle = True
            break;
    if found_negative_cycle :
        Ak = None
    return Ak

def FloydWarshallModified(graph) :

    Ak = [ [0 for to_idx in range(len(graph.nodes)) ] for from_idx in range(len(graph.nodes)) ]
    Akp1 = Ak.copy()

    for node in graph.nodes.values() :
        for edge in node.edges :
            Ak[node.node_idx][edge.to_node.node_idx] = edge.weight

    for new_node in graph.nodes.values() :
        for head in graph.nodes.values() :
            for tail in graph.nodes.values():
                Akp1[head.node_idx][tail.node_idx] = Ak[head.node_idx][tail.node_idx]+ Ak[head.node_idx][new_node.node_idx] * Ak[new_node.node_idx][tail.node_idx]
        Ak = Akp1
    return Ak

def Quizz3() :
    for line in FloydWarshallModified(graph.readGraph("Question3.txt", True)) :
        print (line)

def Quizz4(filename) :
    floyd = FloydWarshall(graph.readGraph(filename, True))
    if floyd :
        min_dist = float("inf")
        for line in floyd:
            for elem in line :
                if elem < min_dist :
                    min_dist = elem;
        print (filename + " min dist " + str(min_dist))
    else :
        print(filename + " found negative cycle")

def P1() :
    Quizz4("g1.txt")
    Quizz4("g2.txt")
    Quizz4("g3.txt")
    #Quizz4("test1.txt")
    #Quizz4("Question4.txt")

#if __name__ == "__main__": P1()
P1()