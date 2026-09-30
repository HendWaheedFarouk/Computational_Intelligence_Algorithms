import numpy as np
import random
import matplotlib.pyplot as plt

best_fitness_history = []
avg_fitness_history = []

# objective function --- minimization
def objective_function(p):
    x, y = p
    return (x - 2)**2 + (y - 2)**2

# initial points
points = [np.array([0.0, 0.0]), np.array([1.0, 0.0]), np.array([0.0, 1.0])]

# parameters
alpha = random.uniform(0.5, 1.5)
gamma = random.uniform(1.5, 2.5)
beta = random.uniform(0.1, 0.5)
print(f"Parameters used: alpha={alpha:.2f}, gamma={gamma:.2f}, beta={beta:.2f}\n")

# algorithm
def solve_simplex(points, iterations=20):
    for i in range(iterations):
        # sort --- points
        points.sort(key=objective_function)
        xb = points[0]  # Best (Xb)
        xg = points[1]  # Good (Xg)
        xw = points[2]  # Worst (Xw)
        # calculate fitness --- points
        f_b = objective_function(xb)
        f_g = objective_function(xg)
        f_w = objective_function(xw)
        
        best_fitness_history.append(f_b)
        avg_fitness_history.append((f_b + f_g + f_w) / 3)
        # calculate centroid
        xc = (xb + xg) / 2
        # reflection (X_r)
        xr = xc + alpha * (xc - xw)
        f_r = objective_function(xr)

        if f_b <= f_r < f_g:
            points[2] = xr
        elif f_r < f_b:
            xe = xc + gamma * (xr - xc)
            if objective_function(xe) < f_r:
                points[2] = xe
            else:
                points[2] = xr
        else:
            if f_g <= f_r < f_w:
                # outside contraction
                c_out = xc + beta * (xr - xc)
                if objective_function(c_out) <= f_r:
                    points[2] = c_out
            else:
                # inside contraction
                c_in = xc + beta * (xw - xc)
                if objective_function(c_in) < f_w:
                    points[2] = c_in
        
        print(f"Iteration {i+1}: Best fitness = {objective_function(points[0]):.4f} at {points[0]}")

    return points[0]

# run
best_solution = solve_simplex(points)
print(f"\nFinal Best Solution: {best_solution}")
print(f"Objective Value: {objective_function(best_solution):.4f}")

# graphs
plt.figure(figsize=(10, 6))
plt.plot(best_fitness_history, label='Best Fitness', color='blue', linewidth=2)
plt.plot(avg_fitness_history, label='Average Fitness', color='orange', linestyle='--')

plt.title('Simplex Search (Nelder-Mead) Optimization')
plt.xlabel('Generations / Iterations')
plt.ylabel('Fitness Value')
plt.legend()
plt.grid(True)
plt.show()