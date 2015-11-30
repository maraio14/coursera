ratings=matrix(c(1,2,3,4,5,2,3,2,5,3,5,5,5,3,2), nrow = 3, ncol = 5, byrow=TRUE)

avg_rows = rowMeans(ratings)
avg_rows
ratings = ratings - avg_rows
ratings
avg_cols = colMeans(ratings)
avg_cols
ratings = t(ratings) - avg_cols
ratings
ratings = t(ratings)
ratings