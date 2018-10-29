import numpy as np

# exp_dist <-> f(x, beta) = (1/beta) * exp(-(1/beta) * x)

beta = 10
process_count = 20
a=np.round(np.random.exponential(scale=beta, size=(process_count,2)))
np.savetxt("process.csv", a, delimiter=",",fmt="%i")