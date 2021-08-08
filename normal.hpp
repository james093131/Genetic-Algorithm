#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
using namespace std;
typedef vector<char> c1d;
typedef vector<c1d> c2d;
typedef vector<c2d> c3d;
typedef vector<c3d> c4d;
typedef vector<int> i1d;
typedef vector<i1d> i2d;
typedef vector<i2d> i3d;
typedef vector<i3d> i4d;
typedef vector<double> d1d;
typedef vector<d1d> d2d;
typedef vector<d2d> d3d;
typedef vector<d3d> d4d;

class GA
{
public:
    i1d Run_result;
    i1d Run_evaluation;

public:
    void run(int Run, int MAX_NFE, int Dim, int Pop, double Crossover, double Mutation, int S_method)
    {
        srand((unsigned)time(NULL));
        Run_set(MAX_NFE / Pop, Run);
        double START, END;
        START = clock();
        for (int i = 0; i < Run; i++)
        {
            Set(Pop, Dim, Crossover, Mutation);
            Initial(Pop, Dim, MAX_NFE);
            while (NFE < MAX_NFE)
            {
                Transition(Pop, Dim, S_method);
                Evaluation(Pop, Dim, MAX_NFE);
            }
            Run_result[i] = Current_best_obj;
        }
        END = clock();
        Output(Run, MAX_NFE, Dim, Pop, Crossover, Mutation, START, END, S_method);
    }

private:
    i2d chromosome;       //solution
    i1d objective;        //objective value
    int Current_best_obj; //record current best objective value.;
    double CR;            //crossover rate
    double M;             //Mutation rate
    int NFE;

private:
    inline double randomDouble() //uniform distribution [0, 1)
    {
        return (double)rand() / (double)RAND_MAX;
    }
    inline int randomInt(int min, int max)
    {
        return (rand() % (max - min + 1) + min);
    }

    void Set(int Pop, int Dim, double Crossover, double Mutation) //Set some parameter
    {
        chromosome.clear();
        chromosome.swap(chromosome);
        chromosome.assign(Pop, i1d(Dim, 0));
        objective.resize(Pop);
        Current_best_obj = 0;
        CR = Crossover;
        M = Mutation;
        NFE = 0;
    }
    void Run_set(int Iter, int Run)
    {
        Run_evaluation.resize(Iter, 0);
        Run_result.resize(Run, 0);
    }
    void Initial(int Pop, int Dim, int MAX_NFE) //random initial
    {
        for (int i = 0; i < Pop; i++)
        {
            for (int j = 0; j < Dim; j++)
            {
                chromosome[i][j] = randomInt(0, 1);
            }
        }
        Evaluation(Pop, Dim, MAX_NFE);
    }
    void Evaluation(int Pop, int Dim, int MAX_NFE)
    {
        int chc = 1; //record evaluation or not
        for (int i = 0; i < Pop; i++)
        {
            objective[i] = 0;
            for (int j = 0; j < Dim; j++)
            {
                objective[i] += chromosome[i][j];
            }
            if (objective[i] > Current_best_obj)
                Current_best_obj = objective[i];
            NFE++;
            if (NFE > MAX_NFE)
            {
                chc = 0;
                break;
            }
        }
        if (chc)
            Run_evaluation[NFE / Pop - 1] += Current_best_obj;
    }
    void Transition(int Pop, int Dim, int S_method)
    {
        i2d temp = Selection(Pop, Dim, S_method);
        Crossover(temp, Pop, Dim);
        Mutation(Pop);
    }
    i2d Selection(int Pop, int Dim, int S_method)
    {
        i2d temp;
        if (S_method == 0)
            temp = tournament(Pop, Dim);
        else
            temp = Roulette_wheel(Pop, Dim);
        return temp;
    }
    i2d tournament(int Pop, int Dim)
    {
        i2d temp = chromosome;
        int i = 0;
        while (i < Pop)
        {
            int c1 = rand() % (Pop);
            int c2;
            do
            {
                c2 = rand() % (Pop);
            } while (c1 == c2);
            int chc;
            if (objective[c1] > objective[c2])
            {
                chc = c1;
            }
            else
            {
                chc = c2;
            }
            temp[i] = chromosome[chc];
            i++;
        }
        return temp;
    }
    int P_index(d1d P) //check the probability fall into which region.
    {
        double r = randomDouble();
        int ind = 0;
        for (int i = 0; i < P.size(); i++)
        {
            if (r < P[i])
            {
                ind = i;
                break;
            }
        }
        return ind;
    }
    i2d Roulette_wheel(int Pop, int Dim)
    {
        i2d temp = chromosome;
        int sum_obj = 0;
        d1d P(objective.size(), 0);                //probability set
        for (int i = 0; i < objective.size(); i++) //calculate sum of objective value
        {
            sum_obj += objective[i];
        }
        P[0] = double(objective[0]) / sum_obj;
        for (int i = 1; i < P.size(); i++)
        {
            P[i] = double(objective[i]) / sum_obj + P[i - 1];
            // cout << P[i] << endl;
        }

        for (int i = 0; i < temp.size(); i++)
        {
            int ind = P_index(P);
            temp[i] = chromosome[ind];
        }
        return temp;
    }
    void Crossover(i2d temp, int Pop, int Dim)
    {
        int cut_point = randomInt(0, Dim - 1);
        for (int i = 1; i < Pop; i += 2)
        {
            for (int j = 0; j < Dim; j++)
            {
                if (j < cut_point)
                {
                    chromosome[i - 1][j] = temp[i - 1][j];
                    chromosome[i][j] = temp[i][j];
                }
                else
                {
                    if (randomDouble() < CR)
                        chromosome[i - 1][j] = temp[i][j];
                    else
                        chromosome[i - 1][j] = temp[i - 1][j];
                    if (randomDouble() < CR)
                        chromosome[i][j] = temp[i - 1][j];
                    else
                        chromosome[i][j] = temp[i][j];
                }
            }
        }
        if (Pop % 2 != 0)
            chromosome[Pop - 1] = temp[Pop - 1];
    }
    void Mutation(int Pop)
    {
        for (int i = 0; i < Pop; i++)
        {
            if (randomDouble() < M)
            {
                int M_point = randomInt(0, Pop - 1);
                if (chromosome[i][M_point] == 0)
                    chromosome[i][M_point] = 1;
                else
                    chromosome[i][M_point] = 0;
            }
        }
    }
    void Output(int run, int MAX_NFE, int dim, int pop, double CR, double M, double START, double END, int S_method)
    {
        int Run_AVG = 0;
        int Run_Best = 0;
        for (int i = 0; i < run; i++)
        {
            Run_AVG += Run_result[i];

            if (Run_result[i] > Run_Best)
                Run_Best = Run_result[i];
        }

        Run_AVG /= run;
        for (int i = 0; i < Run_evaluation.size(); i++)
        {
            cout << (i + 1) * pop << ' ' << double(Run_evaluation[i]) / run << endl; //using double to observe average convergence status.
        }
        cout << "# Run : " << run << endl;
        cout << "# Evaluation : " << MAX_NFE << endl;
        cout << "# Dimension : " << dim << endl;
        cout << "# Population : " << pop << endl;
        cout << "# Crossover Rate : " << CR << endl;
        cout << "# Mutation Rate : " << M << endl;
        if (S_method == 0)
            cout << "# Selection Method : Tournament selection " << endl;
        else
            cout << "# Selection Method : Roulette wheel selection " << endl;
        cout << "# Best Objective : " << Run_Best << endl;
        cout << "# Average Objective : " << Run_AVG << endl;
        cout << "# Execution Time : " << (END - START) / CLOCKS_PER_SEC << "(s)" << endl;
    }
};