from binary_heap import BinaryHeap

__author__ = 'justme';

import os
import binary_heap

def test():
    fileName = "jobs.txt"
    f = open(fileName, 'rt')
    f.seek(0)
    job_no = int(f.readline())
    priority = binary_heap.PriorityRatio()

    heap = BinaryHeap(priority)
    if job_no != 0 :
        job_cnt = 0
        while job_cnt < job_no:
            line = f.readline().split()
            (w, l) = (int(line[0]), int(line[1]))
            heap.insert((w,l))
            job_cnt  = job_cnt+1

    f.close()
    length = 0
    weighted_sum = 0
    while not heap.is_empty():
        d = heap.extract()
        length += d[1]
        weighted_sum += length*d[0]
        #print d

    print weighted_sum
# #####################################################################
######################################################################
if __name__ == '__main__': test()

