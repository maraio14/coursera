__author__ = 'justme'

from math import sqrt


class Point :
    def __init__(self, pairXY):
        assert(isinstance(pairXY, tuple))
        self.x= pairXY[0]
        self.y = pairXY[1]

class Cluster:
    def __init__(self, initialPoint, idx):
        self._listPoints = [initialPoint]
        self.centroid = Point((initialPoint.x, initialPoint.y))
        self.idx = idx
    def add_point(self, point):
        #self.centroid.x = (self.centroid.x * len(listPoints) + point.x)/(len(listPoints) + 1)
        #self.centroid.y = (self.centroid.y * len(listPoints) + point.y)/(len(listPoints) + 1)
        self._listPoints.append(point)

    def update_centroid(self):
        sum_x = 0
        sum_y = 0
        for pt in self._listPoints :
            sum_x += pt.x
            sum_y += pt.y
        self.centroid.x = sum_x / len(self._listPoints)
        self.centroid.y = sum_y / len(self._listPoints)

def getClosestCluster(listClusters, pt):
    closestCluster = None
    min_dist = float("Inf")
    for cluster in listClusters :
        #dist_to_centroid = sqrt((pt.x - cluster.centroid.x)**2 + (pt.y - cluster.centroid.y)**2)
        dist_to_centroid = abs(pt.x - cluster.centroid.x) + abs(pt.y - cluster.centroid.y)
        if dist_to_centroid < min_dist :
            closestCluster = cluster
            min_dist = dist_to_centroid
    return closestCluster

def buildClusters(listPoints, listClusters, clusters) :
    for pt in listPoints:
        if(len(listClusters) < clusters):
            listClusters.append(Cluster(pt, len(listClusters)))
        else:
            cluster = getClosestCluster(listClusters, pt)
            cluster.add_point(pt)

def updateClusters(listClusters):
     for cluster in listClusters :
         cluster.update_centroid()

def printClusters(listClusters):
    for cluster in listClusters:
        print('[' + str(cluster.idx)+':(' + str(cluster.centroid.x) + ',' + str(cluster.centroid.y) +')] ' ,end='')
        for point in cluster._listPoints:
            print('(' + str(point.x) + ',' + str(point.y) + ') ', end = '')
        print('')

def clusters(listPoints, clusters) :
    listClusters = []
    buildClusters(listPoints, listClusters, clusters)
    print('Iteration 1')
    #printClusters(listClusters)

    updateClusters(listClusters)
    print('Update clusters after Iteration 1')
    printClusters(listClusters)
    buildClusters(listPoints, listClusters, clusters)
    print('Iteration 2')
    printClusters(listClusters)



if __name__ == '__main__':
    # listPoints = [Point(( 25,125)), Point(( 44,105)), Point((29,97)), Point((35,63)), Point((55,63)),
    #               Point(( 42, 57)), Point((23,40)), Point((64,37)), Point((33,22)), Point((55,20)),
    #               Point((28,145)), Point((65,140)), Point((50,130)), Point((38,115)), Point((55,118)),
    #               Point((50,90)), Point((63,88)), Point((43,83)),  Point((50,60)), Point((50,30))]

    listPoints = [Point(( 1,1)), Point((4,4)), Point(( 2,1)), Point((2,2)), Point((3, 3)), Point((4,2)),
                  Point(( 2, 4))]
    clusters(listPoints, 2)