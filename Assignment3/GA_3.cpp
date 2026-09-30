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
// Generation of population
struct Individual {
    double x1;
    double x2;
    double fitness;
};
vector<Individual> init_pop(int popSize, double Rmin, double Rmax) {
    uniform_real_distribution<> realDist(Rmin, Rmax);
    vector<Individual> population(popSize);
    for (auto& ind : population) {
        ind.x1 = realDist(gen);
        ind.x2 = realDist(gen);
    }
    return population;
}
// Compute fitness
double fitness(Individual& ind) {
    return 8 - pow(ind.x1 + 0.0317, 2) + pow(ind.x2, 2);
}
// Tournment selection
Individual tournament_selection(vector<Individual>& pop, int k) {
    uniform_int_distribution<> idx(0, pop.size() - 1);
    Individual best = pop[idx(gen)];
    for (int i = 1; i < k; i++) {
        Individual competitor = pop[idx(gen)];
        if (competitor.fitness > best.fitness)
            best = competitor;
    }
    return best;
}
// Arithmetic crossover
vector<Individual> arithmetic_crossover(Individual& p1, Individual& p2, double pcross) {
    vector<Individual> offsprings(2);
    uniform_real_distribution<> alphaDist(0.0, 1.0);
    if (dist(gen) <= pcross) {
        double alpha = alphaDist(gen);
        offsprings[0].x1 = alpha * p1.x1 + (1 - alpha) * p2.x1;
        offsprings[0].x2 = alpha * p1.x2 + (1 - alpha) * p2.x2;
        offsprings[1].x1 = alpha * p2.x1 + (1 - alpha) * p1.x1;
        offsprings[1].x2 = alpha * p2.x2 + (1 - alpha) * p1.x2;
    }
    else {
        offsprings[0] = p1;
        offsprings[1] = p2;
    }
    return offsprings;
}
// Gaussian mutation
void gaussian_mutation(Individual& ind, double sigma, double pMut, double Rmin, double Rmax) {
    normal_distribution<> gauss(0.0, sigma);
    if (dist(gen) <= pMut) {
        ind.x1 += gauss(gen);
        ind.x1 = max(Rmin, min(Rmax, ind.x1));
    }
    if (dist(gen) <= pMut) {
        ind.x2 += gauss(gen);
        ind.x2 = max(Rmin, min(Rmax, ind.x2));
    }
}
// Execution of the program
vector<Individual> Execution(int popSize, int numOfGenerations, double pcross, double pmut, double sigma, int k, double Rmin, double Rmax, vector<double>& best_hist, vector<double>& avg_hist) {
    vector<Individual> population = init_pop(popSize, Rmin, Rmax);
    for (int gen = 0; gen < numOfGenerations; gen++) {
        double best = -1e9;
        double avg = 0.0;
        for (auto& ind : population) {
            ind.fitness = fitness(ind);
            best = max(best, ind.fitness);
            avg += ind.fitness;
        }
        avg /= popSize;
        best_hist.push_back(best);
        avg_hist.push_back(avg);
        vector<Individual> newPopulation;
        while (newPopulation.size() < popSize) {
            Individual p1 = tournament_selection(population, k);
            Individual p2 = tournament_selection(population, k);
            vector<Individual> offsprings = arithmetic_crossover(p1, p2, pcross);
            gaussian_mutation(offsprings[0], sigma, pmut, Rmin, Rmax);
            gaussian_mutation(offsprings[1], sigma, pmut, Rmin, Rmax);
            newPopulation.push_back(offsprings[0]);
            if (newPopulation.size() < popSize)
                newPopulation.push_back(offsprings[1]);
        }
        population = newPopulation;
    }
    return population;
}
// Execution of the program with elitism
vector<Individual> ExecutionWithElitism( int popSize, int numOfGenerations, double pcross, double pmut, double sigma, int k, double Rmin, double Rmax, vector<double>& best_hist, vector<double>& avg_hist) {
    vector<Individual> population = init_pop(popSize, Rmin, Rmax);
    for (int gen = 0; gen < numOfGenerations; gen++) {
        double best = -1e9;
        double avg = 0.0;
        for (auto& ind : population) {
            ind.fitness = fitness(ind);
            best = max(best, ind.fitness);
            avg += ind.fitness;
        }
        avg /= popSize;
        best_hist.push_back(best);
        avg_hist.push_back(avg);
        sort(population.begin(), population.end(),
            [](const Individual& a, const Individual& b) {
                return a.fitness > b.fitness;
            });
        vector<Individual> newPopulation;
        newPopulation.push_back(population[0]);
        newPopulation.push_back(population[1]);
        while (newPopulation.size() < popSize) {
            Individual p1 = tournament_selection(population, k);
            Individual p2 = tournament_selection(population, k);
            vector<Individual> children = arithmetic_crossover(p1, p2, pcross);
            gaussian_mutation(children[0], sigma, pmut, Rmin, Rmax);
            gaussian_mutation(children[1], sigma, pmut, Rmin, Rmax);
            newPopulation.push_back(children[0]);
            if (newPopulation.size() < popSize)
                newPopulation.push_back(children[1]);
        }
        population = newPopulation;
    }
    return population;
}
// Keep results in file
void writeCSV(const string& filename,
    vector<double>& best,
    vector<double>& avg) {
    ofstream file(filename);
    file << "Generation;BestFitness;AvgFitness\n";
    for (int i = 0; i < best.size(); i++) {
        file << i + 1 << ";"
            << best[i] << ";"
            << avg[i] << "\n";
    }
    file.close();
}
// Main program
int main() {
    int popSize = 100;
    int numOfGenerations = 100;
    double pcross = 0.6;
    double pmut = 0.05;
    double sigma = 0.5;
    double Rmin = -2.0;
    double Rmax = 2.0;
    int k_small = 2;
    int k_large = popSize;
    // Without Elitism
    vector<double> best_noElite_k2, avg_noElite_k2;
    vector<double> best_noElite_kPop, avg_noElite_kPop;
    cout << "Running GA Without Elitism (k = 2)\n";
    Execution(popSize, numOfGenerations, pcross, pmut, sigma, k_small, Rmin, Rmax, best_noElite_k2, avg_noElite_k2);
    writeCSV("NoElitism_k2.csv", best_noElite_k2, avg_noElite_k2);
    cout << "Running GA Without Elitism (k = popSize)\n";
    Execution(popSize, numOfGenerations, pcross, pmut, sigma, k_large, Rmin, Rmax, best_noElite_kPop, avg_noElite_kPop);
    writeCSV("NoElitism_kPop.csv", best_noElite_kPop, avg_noElite_kPop);
    // With Elitism
    vector<double> best_Elite_k2, avg_Elite_k2;
    vector<double> best_Elite_kPop, avg_Elite_kPop;
    cout << "Running GA With Elitism (k = 2)\n";
    ExecutionWithElitism(popSize, numOfGenerations, pcross, pmut, sigma, k_small, Rmin, Rmax, best_Elite_k2, avg_Elite_k2);
    writeCSV("Elitism_k2.csv", best_Elite_k2, avg_Elite_k2);
    cout << "Running GA With Elitism (k = popSize)\n";
    ExecutionWithElitism(popSize, numOfGenerations, pcross, pmut, sigma, k_large, Rmin, Rmax, best_Elite_kPop, avg_Elite_kPop);
    writeCSV("Elitism_kPop.csv", best_Elite_kPop, avg_Elite_kPop);
    // Final results
    cout << "\n===== Final Best Fitness =====\n";
    cout << "No Elitism (k=2): " << best_noElite_k2.back() << endl;
    cout << "No Elitism (k=popSize): " << best_noElite_kPop.back() << endl;
    cout << "With Elitism (k=2): " << best_Elite_k2.back() << endl;
    cout << "With Elitism (k=popSize): " << best_Elite_kPop.back() << endl;
    cout << "\nDone.\n";
    return 0;
}