refCol=matrix(c(2/7, 3/7, 6/7 ), nrow = 3, ncol = 1)
norm(refCol, "f")

newCol1=matrix(c(.429, .857, .286), nrow = 3, ncol = 1)
norm(newCol1, "f")
t(newCol1)%*%refCol

newCol2=matrix(c(-.702, .117, .702), nrow = 3, ncol = 1)
norm(newCol2, "f")
t(newCol2)%*%refCol

newCol3=matrix(c(2.250, -.500, -.750), nrow = 3, ncol = 1)
norm(newCol3, "f")
t(newCol3)%*%refCol


newCol4=matrix(c(.702, -.702, .117), nrow = 3, ncol = 1)
norm(newCol4, "f")
t(newCol4)%*%refCol
