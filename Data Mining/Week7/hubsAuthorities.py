__author__ = 'justme'

import numpy as np

def compute_hubs_authorities(nodes_count):
    link_matrix = np.matrix(get_link_matrix(nodes_count))
    trans_link_matrix = np.transpose(link_matrix)

    hubs = np.transpose(np.matrix(np.ones(nodes_count)))
    authorities = np.transpose(np.matrix(np.zeros(nodes_count)))
    eps = 0.001
    found_eps = False
    step = 1
    while not found_eps:
        print("step :"+str(step))
        authorities_new = trans_link_matrix * hubs
        authorities_new /= np.max(authorities_new)
        hubs_new = link_matrix * authorities_new
        hubs_new /= np.max(hubs_new)

        if np.sum(abs(authorities_new - authorities)) <= eps and  np.sum(abs(hubs_new - hubs)) <= eps :
            found_eps = True
            print("Final values:")
        else:

            hubs = hubs_new
            authorities = authorities_new
            step +=1
            print(hubs)
            print(authorities)

    return (hubs, authorities)

def  get_link_matrix(nodes_count) :

    link_matrix = np.zeros(nodes_count**2).reshape(nodes_count, nodes_count)

    # link_matrix[0,1] = 1
    # link_matrix[1,0] = 1
    # link_matrix[0,2] = 1
    # link_matrix[2,3] = 1
    # link_matrix[3,2] = 1

    link_matrix[0,1] = 1
    link_matrix[1,2] = 1
    link_matrix[2,3] = 1


    return link_matrix


(hubs, authorities) = compute_hubs_authorities(4)
print(hubs)
print(authorities)
