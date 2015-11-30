mtx = matrix(c(1, 0, 0, 0, 2, 0, 0, 0, 0 ), byrow = TRUE, nrow = 3, ncol = 3)
svd_dec = svd(mtx)
dplus = 1/svd_dec$d
dplus[3] = 0
