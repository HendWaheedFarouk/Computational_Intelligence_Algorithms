#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
using namespace std;
static random_device rd;
static mt19937 gen(rd());
static uniform_real_distribution<> dist(0.0, 1.0);
const double pi = acos(-1.0);

struct Particle {
    double x1, x2;
    double v1, v2;
    double pbest_x1, pbest_x2;
    double fitness;
    double pbest_fitness;
};
// Calculate fitness
double calculate_fitness(double x1, double x2) {
    return sin(2 * x1 - 0.5 * pi) + 3 * cos(x2) + 0.5 * x1;
}
// Initialization
vector<Particle> init_swarm(int swarmSize) {
    vector<Particle> swarm(swarmSize);
    uniform_real_distribution<> dist_x1(-2.0, 3);
    uniform_real_distribution<> dist_x2(-2.0, 1);
    uniform_real_distribution<> dist_v(-0.1, 1.0);
    for (auto& p : swarm) {
        p.x1 = dist_x1(gen);
        p.x2 = dist_x2(gen);
        p.v1 = dist_v(gen);
        p.v2 = dist_v(gen);
        p.fitness = calculate_fitness(p.x1, p.x2);
        p.pbest_x1 = p.x1;
        p.pbest_x2 = p.x2;
        p.pbest_fitness = p.fitness;
    }
    return swarm;
}
// PSO algorithm
void run_PSO(bool useElitism, int swarmSize, int generations, vector<double>& best_hist, vector<double>& avg_hist, const string& particleFileName) {
    ofstream particleFile(particleFileName);
    particleFile << "Generation;Particle;x1;x2\n";
    double c1 = 1.5;
    double c2 = 1.5;
    vector<Particle> swarm = init_swarm(swarmSize);
    double gbest_x1, gbest_x2, gbest_fitness = -1e9;
    for (int gen_idx = 0; gen_idx < generations; gen_idx++) {
        double current_sum_fitness = 0;
        double current_best = -1e9;
        for (auto& p : swarm) {
            if (p.fitness > gbest_fitness) {
                gbest_fitness = p.fitness;
                gbest_x1 = p.x1;
                gbest_x2 = p.x2;
            }
            if (p.fitness > current_best) current_best = p.fitness;
            current_sum_fitness += p.fitness;
        }
        best_hist.push_back(gbest_fitness);
        avg_hist.push_back(current_sum_fitness / swarmSize);
        for (auto& p : swarm) {
            p.v1 = p.v1 + c1 * dist(gen) * (p.pbest_x1 - p.x1) + c2 * dist(gen) * (gbest_x1 - p.x1);
            p.v2 = p.v2 + c1 * dist(gen) * (p.pbest_x2 - p.x2) + c2 * dist(gen) * (gbest_x2 - p.x2);
            p.v1 = max(-0.1, min(1.0, p.v1));
            p.v2 = max(-0.1, min(1.0, p.v2));
            p.x1 += p.v1;
            p.x2 += p.v2;
            p.x1 = max(-2.0, min(3.0, p.x1));
            p.x2 = max(-2.0, min(1.0, p.x2));
            p.fitness = calculate_fitness(p.x1, p.x2);
            if (p.fitness > p.pbest_fitness) {
                p.pbest_fitness = p.fitness;
                p.pbest_x1 = p.x1;
                p.pbest_x2 = p.x2;
            }
        }
        for (int i = 0; i < swarm.size(); i++) {
            particleFile << gen_idx << ";" << i << ";"
                << swarm[i].x1 << ";" << swarm[i].x2 << "\n";
        }
        if (useElitism) {
            swarm[0].x1 = gbest_x1;
            swarm[0].x2 = gbest_x2;
            swarm[0].fitness = gbest_fitness;
        }
    }
    particleFile.close();
}
void writeCSV(const string& filename, vector<double>& best, vector<double>& avg) {
    ofstream file(filename);
    file << "Generation;BestFitness;AvgFitness\n";
    for (int i = 0; i < best.size(); i++) {
        file << i + 1 << ";" << best[i] << ";" << avg[i] << "\n";
    }
    file.close();
}
int main() {

    int swarmSize = 50;
    int generations = 100;
    vector<double> best_noElite, avg_noElite;
    vector<double> best_Elite, avg_Elite;
    cout << "Running PSO Without Elitism...\n";
    run_PSO(false, swarmSize, generations, best_noElite, avg_noElite, "Particles_NoElitism.csv");
    writeCSV("PSO_NoElitism.csv", best_noElite, avg_noElite);
    cout << "Running PSO With Elitism...\n";
    run_PSO(true, swarmSize, generations, best_Elite, avg_Elite, "Particles_Elitism.csv");
    writeCSV("PSO_Elitism.csv", best_Elite, avg_Elite);
    cout << "Final Results (With Elitism): " << best_Elite.back() << endl;
    cout << "Done! Files PSO_NoElitism.csv and PSO_Elitism.csv generated.\n";

    return 0;
}