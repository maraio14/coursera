alpha = 2

attributes=matrix(c(1,0,1,0,1,2, 1,1,0,0,1,6, 0,1,0,1,0,2), nrow = 3, ncol = 6, byrow=TRUE)
attributes[,6] = attributes[,6]*alpha

u = attributes[1,]
v = attributes[2,]
cosUV = u%*%v / (norm(as.matrix(u), "f")*norm(as.matrix(v), "f"))
print ("AB") 
cosUV
u = attributes[1,]
v = attributes[3,]
cosUV = u%*%v / (norm(as.matrix(u), "f")*norm(as.matrix(v), "f"))
print ("AC")
cosUV
u = attributes[2,]
v = attributes[3,]
cosUV = u%*%v / (norm(as.matrix(u), "f")*norm(as.matrix(v), "f"))
print ("BC")
cosUV
