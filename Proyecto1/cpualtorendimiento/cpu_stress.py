import numpy as np

# Stress the CPU con alto contenido de computations
x = np.random.rand(1000, 10)
while True:
    np.dot(x, x)
