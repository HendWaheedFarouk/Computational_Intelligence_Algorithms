import numpy as np
import math
import matplotlib.pyplot as plt

# Objective function
def objective_function(x, y):
    return (x - 2)**2 + (y - 2)**2

# Generation of new solutions (via Lévy flights) 
def levy_flight(Lambda=1.5):
    sigma = (math.gamma(1 + Lambda) * np.sin(np.pi * Lambda / 2) / 
             (math.gamma((1 + Lambda) / 2) * Lambda * 2**((Lambda - 1) / 2)))**(1 / Lambda)
    u = np.random.normal(0, sigma, size=2)
    v = np.random.normal(0, 1, size=2)
    return u / abs(v)**(1 / Lambda)

# Run program
def solve_cuckoo(with_elitism=True):
    # Parameters
    n = 20                 
    pa = 0.25              
    alpha = 1             
    iterations = 100      
    var_range = [-5, 5]

    # Initialize nests
    nests = np.random.uniform(var_range[0], var_range[1], (n, 2))
    fitness = np.array([objective_function(x, y) for x, y in nests])

    best_history = []
    avg_history = []

    # Elitism
    best_idx = np.argmin(fitness)
    global_best_nest = nests[best_idx].copy()
    global_best_fitness = fitness[best_idx]

    for t in range(iterations):
        for i in range(n):
            L = levy_flight()
            theta = np.random.uniform(0, 2 * np.pi)
            step_x = alpha * L[0] * np.cos(theta)
            step_y = alpha * L[1] * np.sin(theta)
            new_sol = nests[i] + np.array([step_x, step_y])
            new_sol = np.clip(new_sol, var_range[0], var_range[1])
            f_new = objective_function(new_sol[0], new_sol[1])
            if f_new < fitness[i]:
                nests[i], fitness[i] = new_sol, f_new

        # Abandon some nests
        for i in range(n):
            if np.random.rand() < pa:
                nests[i] = np.random.uniform(var_range[0], var_range[1], 2)
                fitness[i] = objective_function(nests[i][0], nests[i][1])

        # Elitism
        if with_elitism:
            current_best_idx = np.argmin(fitness)
            current_min = fitness[current_best_idx]
            if current_min < global_best_fitness:
                global_best_fitness = current_min
                global_best_nest = nests[current_best_idx].copy()
            else:
                worst_idx = np.argmax(fitness)
                nests[worst_idx] = global_best_nest.copy()
                fitness[worst_idx] = global_best_fitness

        best_history.append(np.min(fitness))
        avg_history.append(np.mean(fitness))

    return best_history, avg_history

# Printing depending on --- bool variable
best_elit, avg_elit = solve_cuckoo(with_elitism=True)
best_no_elit, avg_no_elit = solve_cuckoo(with_elitism=False)

# Final results
print(f"--- Final Results (with Elitism) ---")
print(f"Best Fitness: {best_elit[-1]}")
print(f"Average Fitness: {avg_elit[-1]}")

print(f"--- Final Results (without Elitism) ---")
print(f"Best Fitness: {best_no_elit[-1]}")
print(f"Average Fitness: {avg_no_elit[-1]}")

# Graphs
plt.figure(figsize=(10, 6))
plt.plot(best_elit, label='Best Fitness (With Elitism)')
plt.plot(best_no_elit, label='Best Fitness (Without Elitism)')
plt.plot(avg_elit, '--', label='Average Fitness (With Elitism)')
plt.plot(avg_no_elit, '--', label='Average Fitness (Without Elitism)')
plt.xlabel('Iterations')
plt.ylabel('Fitness Value')
plt.title('Cuckoo Search Algorithm')
plt.legend()
plt.grid(True)
plt.show()