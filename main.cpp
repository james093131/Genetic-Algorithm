#include "normal.hpp"

int main(int argc, const char *argv[])
{
    int Run = atoi(argv[1]);
    int MAX_NFE = atoi(argv[2]);
    int Dim = atoi(argv[3]);
    int Pop = atoi(argv[4]);
    double CR = atof(argv[5]);
    double Mutation = atof(argv[6]);
    int Selection_method = atoi(argv[7]);
    GA ga;
    ga.run(Run, MAX_NFE, Dim, Pop, CR, Mutation, Selection_method);
}