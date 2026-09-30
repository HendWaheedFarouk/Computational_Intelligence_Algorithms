# Computational Intelligence & Metaheuristic Optimization Algorithms

This repository contains Python implementations, execution reports, and problem specifications for **7 Computational Intelligence & Optimization assignments** completed for the **Computational Intelligence (CI)** course at the Faculty of Computers and Artificial Intelligence, Cairo University (OR&DS Dept).

Each folder represents an individual assignment containing the **Source Code**, the **Assignment Specification (PDF)**, and the **Analytical Performance Report**.

---

## Assignments Overview

### Assignment 1: Simple Binary Genetic Algorithm (SGA)
* **Problem:** Solving the classic **OneMax** binary optimization problem.
* **Techniques:** Binary Chromosome Encoding, Roulette Wheel Selection, 1-Point Crossover ($p_{cross}=0.6$), Bit-Flip Mutation ($p_{mut}=0.05$), and Elitism.

### Assignment 2: GA for Continuous Function Optimization
* **Problem:** Maximizing $F(x_1, x_2) = 8 - (x_1 + 0.0317)^2 + (x_2)^2$ under bound constraints and penalty functions ($x_1 + x_2 = 1$).
* **Techniques:** Standard vs. Gray Decoding comparison, Variable bit-precisions, and Penalty functions for constrained optimization.

### Assignment 3: Real-Coded Genetic Algorithm (RCGA)
* **Problem:** Optimizing continuous functions using real-number representation.
* **Techniques:** Real Value Encoding, Arithmetic Crossover, Gaussian Mutation ($\sigma=0.5$), and Tournament Selection with varying $k$ values.

### Assignment 4: Particle Swarm Optimization (PSO)
* **Problem:** Maximizing non-linear function $f(x_1,x_2) = \sin(2x_1 - 0.5\pi) + 3\cos(x_2) + 0.5x_1$.
* **Techniques:** Swarm Intelligence, Particle Velocity and Position updates, $p_{best}$ & $g_{best}$ tracking, and Particle trajectory plotting.

### Assignment 5: Ant Colony System (ACS) for Traveling Salesperson Problem (TSP)
* **Problem:** Finding the shortest tour across 30 cities using coordinates data (`TSPDATA.txt`).
* **Techniques:** Pheromone matrix updates ($\tau$), Nearest Neighbor Heuristic ($L_{nn}$), State transition rules, and Evaporation rate ($\rho$) tuning.

### Assignment 6: Simplex Search Method (Nelder-Mead Algorithm)
* **Problem:** Minimizing benchmark function $f(x,y) = (x-2)^2 + (y-2)^2$.
* **Techniques:** Direct search method using geometric simplices, Reflection, Expansion, and Contraction operators without derivatives.

### Assignment 7: Cuckoo Search (CS) Algorithm
* **Problem:** Minimizing non-linear benchmark functions.
* **Techniques:** Nature-inspired metaheuristic using **Lévy Flights** (random walks with heavy-tailed steps), Nest abandonment probability ($p_a = 0.25$), and Global optimum convergence.

---
 
**Languages:** Python, C++
