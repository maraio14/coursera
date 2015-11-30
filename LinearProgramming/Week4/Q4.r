mydata = read.table("Dropbox/Linear Programming/Week4/Data_Corr.txt")
t = mydata[,1]

ones = rep(1,length(t))

A = cbind(ones, t, t^2, t^3, t^4)
pseudoInvA = solve((t(A)%*%A))%*%t(A)
b=mydata[,2]

#cross-check
library("pracma")
pracmaA = pinv(A)
detach("package:pracma", unload=TRUE)

c_vect = pseudoInvA %*% b
c_vect_pracme = pracmaA %*% b