#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <fstream> 
#include <sstream>
using namespace std;

static random_device rd;
static mt19937 gen(rd());
static uniform_int_distribution<> dis(0, 1);
static uniform_real_distribution<> dist(0.0, 1.0);

// Generation of population
vector<string> init_pop(int popSize, int bits) {
    vector<string>population;
    for (int i = 0; i < popSize; i++) {
        string res;
        for (int j = 0; j < bits; j++) {
            res += to_string(dis(gen));
        }
        population.push_back(res);
    }
    return population;
}
// Decoding from binary to decimal in range (-2,2)
double decode(const string& s) {
    int decimal = stoi(s, nullptr, 2);
    int maxVal = pow(2, s.size()) - 1;
    double realValue = -2 + (decimal * (4.0 / maxVal));
    return realValue;
}
// Convert from gray to binary
string grayToBinary(const string& gray) {
    string binary;
    binary += gray[0];
    for (int i = 1; i < gray.size(); i++)
    {
        if (gray[i] == binary[i - 1])
            binary += '0';
        else
            binary += '1';
    }
    return binary;
}
// Compute fitness
double fitness(const string& s, bool useGray) {
    int half = s.size() / 2;
    string x1_bits = s.substr(0, half);
    string x2_bits = s.substr(half, half);
    if (useGray)
    {
        x1_bits = grayToBinary(x1_bits);
        x2_bits = grayToBinary(x2_bits);
    }
    double x1 = decode(x1_bits);
    double x2 = decode(x2_bits);
    double f = 8 - pow(x1 + 0.0317, 2) + pow(x2, 2);
    double penalty = abs(x1 + x2 - 1);
    return f - penalty;
}
// Compute relative fitness
vector<double> computeRelative(const vector<string>& population, bool useGray) {
    vector<double>relative;
    double totalFitness = 0.0;
    for (const auto& s : population) {
        totalFitness += max(0.0, fitness(s, useGray));
    }
    if (totalFitness == 0) {
        for (int i = 0; i < population.size(); i++)
            relative.push_back(1.0 / population.size());
        return relative;
    }
    for (const auto& s : population) {
        relative.push_back(max(0.0, fitness(s, useGray)) / totalFitness);
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
vector<string> crossOver(vector<string>& parents, double pcross, int bits) {
    uniform_int_distribution<> distribution(1, bits - 1);
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
vector<string> Execution(int popSize, int bits, double pcross, double pmute, int numOfGenerations, vector<double>& best_hist, vector<double>& avg_hist, bool useGray) {
    vector<string> population = init_pop(popSize, bits);
    for (int j = 0; j < numOfGenerations; j++) {
        vector<string> newPopulation;
        vector<double> relative = computeRelative(population, useGray);
        vector<double> comulative = computeComulative(relative);
        double best = 0.0;
        double Avg = 0.0;
        for (const auto& s : population) {
            double f = fitness(s, useGray);
            best = max(best, f);
            Avg += f;
        }
        Avg /= population.size();
        best_hist.push_back(best);
        avg_hist.push_back(Avg);
        for (int i = 0; i < popSize / 2; i++) {
            vector<string> parents = selection(population, comulative);
            vector<string> offSprings = crossOver(parents, pcross, bits);
            vector<string> mutated = mutation(offSprings, pmute);
            newPopulation.push_back(mutated[0]);
            newPopulation.push_back(mutated[1]);
        }
        population = newPopulation;
    }
    return population;
}
// Execution of the program with elitism
vector<string> ExecutionWithElitism(int popSize, int bits, double pcross, double pmute, int numOfGenerations, vector<double>& best_hist, vector<double>& avg_hist, bool useGray) {
    vector<string> population = init_pop(popSize, bits);
    for (int gen = 0; gen < numOfGenerations; gen++) {
        vector<string> newPopulation;
        double best = 0.0;
        double avg = 0.0;
        for (const auto& s : population) {
            double f = fitness(s, useGray);
            best = max(best, f);
            avg += f;
        }
        avg /= population.size();
        best_hist.push_back(best);
        avg_hist.push_back(avg);
        vector<string> sortedPop = population;
        sort(sortedPop.begin(), sortedPop.end(), [useGray](const string& a, const string& b) {
            return fitness(a, useGray) > fitness(b, useGray);
            });
        newPopulation.push_back(sortedPop[0]);
        newPopulation.push_back(sortedPop[1]);
        for (int i = 0; i < (popSize - 2) / 2; i++) {
            vector<string> parents = selection(population, computeComulative(computeRelative(population, useGray)));
            vector<string> offSprings = crossOver(parents, pcross, bits);
            vector<string> mutated = mutation(offSprings, pmute);
            newPopulation.push_back(mutated[0]);
            newPopulation.push_back(mutated[1]);
        }
        population = newPopulation;
    }
    return population;
}
// Main program
int main() {
    int popSize = 100;
    int numOfGenerations = 100;
    double pcross = 0.6;
    double pmute = 0.05;
    vector<int> bits_list = { 16, 20, 32 };
    bool useGray = false;

    cout << "Running GA for different bit lengths..." << endl;

    for (int bits : bits_list) {
        cout << "\nBits: " << bits << endl;
        vector<vector<double>> best_elite(10);
        vector<vector<double>> avg_elite(10);
        vector<vector<double>> best_no_elite(10);
        vector<vector<double>> avg_no_elite(10);
        for (int run = 0; run < 10; run++) {
            gen.seed(rd());
            vector<double> best_hist_elite;
            vector<double> avg_hist_elite;
            vector<string> finalGen_elite = ExecutionWithElitism(popSize, bits, pcross, pmute,
                numOfGenerations, best_hist_elite, avg_hist_elite, useGray);
            best_elite[run] = best_hist_elite;
            avg_elite[run] = avg_hist_elite;
            vector<double> best_hist_no_elite;
            vector<double> avg_hist_no_elite;
            vector<string> finalGen_no_elite = Execution(popSize, bits, pcross, pmute,
                numOfGenerations, best_hist_no_elite, avg_hist_no_elite, useGray);
            best_no_elite[run] = best_hist_no_elite;
            avg_no_elite[run] = avg_hist_no_elite;
        }
        ostringstream filename;
        filename << "GA_results_" << bits << "bits.csv";
        ofstream file(filename.str());
        file << "Run;Best_Elite;Avg_Elite;Best_NoElite;Avg_NoElite\n";
        for (int run = 0; run < 10; run++) {
            double mean_best_elite = 0.0, mean_avg_elite = 0.0;
            double mean_best_no_elite = 0.0, mean_avg_no_elite = 0.0;
            for (int g = 0; g < numOfGenerations; g++) {
                mean_best_elite += best_elite[run][g];
                mean_avg_elite += avg_elite[run][g];
                mean_best_no_elite += best_no_elite[run][g];
                mean_avg_no_elite += avg_no_elite[run][g];
            }
            mean_best_elite /= numOfGenerations;
            mean_avg_elite /= numOfGenerations;
            mean_best_no_elite /= numOfGenerations;
            mean_avg_no_elite /= numOfGenerations;
            file << (run + 1) << ";"
                << mean_best_elite << ";" << mean_avg_elite << ";"
                << mean_best_no_elite << ";" << mean_avg_no_elite << "\n";
        }
        file.close();
        cout << "Results saved to " << filename.str() << endl;
    }
    return 0;
}