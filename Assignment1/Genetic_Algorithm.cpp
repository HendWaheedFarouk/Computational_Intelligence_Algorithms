#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

static random_device rd;
static mt19937 gen(rd());
static uniform_int_distribution<> dis(0, 1);
static uniform_real_distribution<> dist(0.0, 1.0);
static uniform_int_distribution<> distribution(1, 4);

// Generation of population
vector<string> init_pop(int popSize) {
    vector<string>population;
    for (int i = 0; i < popSize; i++) {
        string res;
        for (int j = 0; j < 5; j++) {
            res += to_string(dis(gen));
        }
        population.push_back(res);
    }
    return population;
}
// Compute fitness
int fitness(const string& s) {
    return count(s.begin(), s.end(), '1');
}
// Compute relative fitness
vector<double> computeRelative(const vector<string>& population) {
    vector<double>relative;
    int totalFitness = 0;
    for (const auto& s : population) {
        totalFitness += fitness(s);
    }
    for (const auto& s : population) {
        relative.push_back((double)fitness(s) / totalFitness);
    }
    return relative;
}
// Compute comulative relative fitness
vector<double> computeComulative(const vector<double>& relative) {
    vector<double>comulative;
    double sum = 0.0;
    for (double r : relative) {
        sum += r;
        comulative.push_back(sum);
    }
    return comulative;
}
// Roulette wheel selection
vector<string> selection(const vector<string>& population, const vector<double>& comulative) {
    vector<string>parents;
    for (int i = 0; i < 2; i++) {
        double r = dist(gen);
        for (int j = 0; j < comulative.size(); j++) {
            if (r <= comulative[j]) {
                parents.push_back(population[j]);
                break;
            }
        }
    }
    return parents;
}
// One-point crossover
vector<string> crossOver(vector<string>& parents, double pcross) {
    vector<string> offSprings;
    if (dist(gen) <= pcross) {
        int rn = distribution(gen);
        string offSpring1 = parents[0].substr(0, rn) + parents[1].substr(rn);
        string offSpring2 = parents[1].substr(0, rn) + parents[0].substr(rn);
        offSprings.push_back(offSpring1);
        offSprings.push_back(offSpring2);
    }
    else {
        offSprings.push_back(parents[0]);
        offSprings.push_back(parents[1]);
    }
    return offSprings;
}
// Bit-flip mutation
vector<string> mutation(vector<string>& offSprings, double pmute) {
    vector<string> mutatedOffSprings;
    for (const auto& s : offSprings) {
        string mutated = s;
        for (int i = 0; i < mutated.size(); i++) {
            double r = dist(gen);
            if (r <= pmute) {
                mutated[i] = (mutated[i] == '0') ? '1' : '0';
            }
        }
        mutatedOffSprings.push_back(mutated);
    }
    return mutatedOffSprings;
}
// Execution of the program
vector<string> Execution(int popSize, double pcross, double pmute, int numOfGenerations, vector<int>& best_hist, vector<double>& avg_hist) {
    vector<string> population = init_pop(popSize);
    for (int j = 0; j < numOfGenerations; j++) {
        vector<string> newPopulation;
        vector<double> relative = computeRelative(population);
        vector<double> comulative = computeComulative(relative);
        int best = 0;
        double Avg = 0.0;
        for (const auto& s : population) {
            int f = fitness(s);
            best = max(best, f);
            Avg += f;
        }
        Avg /= population.size();
        best_hist.push_back(best);
        avg_hist.push_back(Avg);
        for (int i = 0; i < popSize / 2; i++) {
            vector<string> parents = selection(population, comulative);
            vector<string> offSprings = crossOver(parents, pcross);
            vector<string> mutated = mutation(offSprings, pmute);
            newPopulation.push_back(mutated[0]);
            newPopulation.push_back(mutated[1]);
        }
        population = newPopulation;
    }
    return population;
}
// Execution of the program with elitism
vector<string> ExecutionWithElitism(int popSize, double pcross, double pmute, int numOfGenerations, vector<int>& best_hist, vector<double>& avg_hist) {
    vector<string> population = init_pop(popSize);
    for (int gen = 0; gen < numOfGenerations; gen++) {
        vector<string> newPopulation;
        int best = 0;
        double avg = 0.0;
        for (const auto& s : population) {
            int f = fitness(s);
            best = max(best, f);
            avg += f;
        }
        avg /= population.size();
        best_hist.push_back(best);
        avg_hist.push_back(avg);
        vector<string> sortedPop = population;
        sort(sortedPop.begin(), sortedPop.end(), [](const string& a, const string& b) {
            return fitness(a) > fitness(b);
            });
        newPopulation.push_back(sortedPop[0]);
        newPopulation.push_back(sortedPop[1]);
        for (int i = 0; i < (popSize - 2) / 2; i++) {
            vector<string> parents = selection(population, computeComulative(computeRelative(population)));
            vector<string> offSprings = crossOver(parents, pcross);
            vector<string> mutated = mutation(offSprings, pmute);
            newPopulation.push_back(mutated[0]);
            newPopulation.push_back(mutated[1]);
        }
        population = newPopulation;
    }
    return population;
}

int main() {
    int popSize, numOfGenerations;
    double pcross, pmute;

    cout << "Welcome to the Genetic Algorithm Program!" << endl;
    cout << "This program finds the maximum number of 1's in a binary string using a genetic algorithm." << endl;
    cout << "\nEnter the following values:" << endl;
    cout << "Population size: ";
    cin >> popSize;
    cout << "Crossover probability (0 - 1): ";
    cin >> pcross;
    cout << "Mutation probability (0 - 1): ";
    cin >> pmute;
    cout << "Number of generations: ";
    cin >> numOfGenerations;

    vector<int> best_hist;
    vector<double> avg_hist;

    vector<string> lGeneration = ExecutionWithElitism(popSize, pcross, pmute, numOfGenerations, best_hist, avg_hist);
    cout << left << setw(12) << "Generation"
        << setw(12) << "Best"
        << setw(12) << "Avg" << endl;
    for (int i = 0; i < numOfGenerations; i++) {
        cout << setw(12) << i + 1
            << setw(12) << best_hist[i]
            << setw(12) << avg_hist[i]
            << endl;
    }
    cout << "\nFinal Generation: " << endl;
    for (const auto& s : lGeneration) {
        cout << s << endl;
    }
}