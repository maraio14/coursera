> laplacian = matrix(c(2, -1, -1,  0,  0,  0, -1,  3,  0, -1,  0, -1, -1,  0,  2, -1,  0,  0, 0, -1, -1,  3, -1,  0, 0,  0,  0, -1,  2, -1, 0, -1,  0,  0, -1,  2), nrow = 6, ncol = 6, byrow=TRUE)
> eig = eigen(laplacian, symmetric=TRUE)
> evals = eig$values
> events = eig$vectors
> 
  > mom2 = events[,5]
> meanMom2 = mean(mom2)