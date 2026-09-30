import numpy as np
import matplotlib.pyplot as plt
# ==============================
# 1) load data from file
# ==============================
data = np.loadtxt("TSPDATA.txt", skiprows=1)
coords = data[:, 1:3]          
n = len(coords)
# ==============================
# 2) calculate distances and eta
# ==============================
dist = np.linalg.norm(coords[:, np.newaxis] - coords, axis=2)
dist[dist == 0] = np.inf
eta = 1.0 / dist
# ==============================
# 3) calculate lnn from all cities
# ==============================
def nearest_neighbor_all():
    best = np.inf
    for start in range(n):
        visited = {start}
        current = start
        length = 0
        while len(visited) < n:
            next_city = min(
                [c for c in range(n) if c not in visited],
                key=lambda c: dist[current][c]
            )
            length += dist[current][next_city]
            visited.add(next_city)
            current = next_city
        length += dist[current][start]
        best = min(best, length)
    return best
Lnn = nearest_neighbor_all()
# ==============================
# graph ---> initial cities
# ==============================
plt.figure()
plt.scatter(coords[:, 0], coords[:, 1], color='red')
plt.title("Initial Cities")
plt.show()
# ==============================
# Experiments Parameters
# ==============================
alphas = [1, 2]
betas = [2, 5]
rhos = [0.1, 0.3]
iterations = 20      
q0 = 0.9
xi = 0.1
overall_best_length = np.inf
overall_best_tour = None
best_params = None
# ==============================
# 4) Experiments Loop
# ==============================
for alpha in alphas:
    for beta in betas:
        for rho in rhos:
            print(f"\nRunning with α={alpha}, β={beta}, ρ={rho}")
            tau0 = 1 / (n * Lnn)
            tau = np.full((n, n), tau0)
            best_length = np.inf
            best_tour = None
            # ==============================
            # ACS Algorithm
            # ==============================
            for it in range(iterations):
                for start_city in range(n):
                    tour = [start_city]
                    visited = {start_city}
                    while len(tour) < n:
                        i = tour[-1]
                        unvisited = [c for c in range(n) if c not in visited]
                        q = np.random.rand()
                        # State Transition Rule
                        if q <= q0:
                            next_city = max(
                                unvisited,
                                key=lambda j: (tau[i][j] ** alpha) * (eta[i][j] ** beta)
                            )
                        else:
                            probs = np.array([
                                (tau[i][j] ** alpha) * (eta[i][j] ** beta)
                                for j in unvisited
                            ])
                            probs /= probs.sum()
                            next_city = np.random.choice(unvisited, p=probs)
                        tour.append(next_city)
                        visited.add(next_city)
                        # Local Update
                        tau[i][next_city] = (1 - xi) * tau[i][next_city] + xi * tau0
                        tau[next_city][i] = tau[i][next_city]
                    # calculation of length
                    length = sum(dist[tour[k]][tour[k+1]] for k in range(n - 1))
                    length += dist[tour[-1]][tour[0]]
                    if length < best_length:
                        best_length = length
                        best_tour = tour
                # Global Update (best only)
                tau *= (1 - rho)
                for k in range(n - 1):
                    i, j = best_tour[k], best_tour[k + 1]
                    tau[i][j] += rho * (1 / best_length)
                    tau[j][i] = tau[i][j]
                i, j = best_tour[-1], best_tour[0]
                tau[i][j] += rho * (1 / best_length)
                tau[j][i] = tau[i][j]
                print(f"Iteration {it + 1}: Best = {best_length:.4f}")
            print(f"Final Best Length = {best_length:.4f}")
            # save best result
            if best_length < overall_best_length:
                overall_best_length = best_length
                overall_best_tour = best_tour
                best_params = (alpha, beta, rho)
# ==============================
# 5) best result
# ==============================
print("\n==============================")
print("Best Overall Solution")
print(f"Length = {overall_best_length:.4f}")
print(f"Parameters α,β,ρ = {best_params}")
print("==============================")
# ==============================
# graph ---> best path
# ==============================
plt.figure(figsize=(10, 6))
plt.scatter(coords[:, 0], coords[:, 1], color='red')

path = overall_best_tour + [overall_best_tour[0]]
plt.plot(coords[path, 0], coords[path, 1], 'b-')

plt.title(f"Best Tour Length: {overall_best_length:.4f}")
plt.show()