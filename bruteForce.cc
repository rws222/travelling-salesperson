// tspfinal.c -- Experiments on fast TSP algorithm 
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <unistd.h>
#include <iostream>

// Data types and sizes 
#define MASKTYPE long 
#define MAXN 60
#define MAXBIN 9999991 
typedef double Dist; 
#define INF ((Dist) 1e20) 
#define ZERO ((Dist) 0)

using namespace std;

// Globals: points and perm vectors 
int n;
struct Point {
    int orig_index;
    float x;
    float y;
    bool operator<(const Point& r) {
      return orig_index < r.orig_index;
    }
};
Point c[MAXN];

int p[MAXN];
Dist minsum;
int minp[MAXN];
Dist distarr[MAXN][MAXN];

float sqr(float x) { 
  return x*x; 
}

Dist geomdist(int i, int j) { 
  return (Dist) (sqrt(sqr(c[i].x-c[j].x) + sqr(c[i].y-c[j].y)));
}

void solve() { 
    int i, j;
    int size = (sizeof(c)/sizeof(*c));
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
          distarr[i][j] = geomdist(i, j); 
        }
    }
    Dist optimal_cost = INF;
    Point optimal_path[n];
    do {
        Dist temp_cost = 0;
        for(int i = 1 ; i < n; i++){
            temp_cost += distarr[c[i-1].orig_index][c[i].orig_index];
        }
        temp_cost += distarr[c[n-1].orig_index][c[0].orig_index];
        if(temp_cost < optimal_cost){
            optimal_cost = temp_cost;
            minsum = optimal_cost;
            for(int jj = 0; jj < n ; jj++)
              optimal_path[jj] = c[jj];
        }
    } while(next_permutation(c+1, c+n));
}

// Report on how to use the command line to configure this program
void usage() {
    std::cout
        << "Command-Line Options:" << std::endl
        << "  -n <int> : number of nodes in path" << std::endl << std::endl;
    exit(0);
}

// Parse command line arguments using getopt()
void parseargs(int argc, char** argv, int& n) {
    // parse the command-line options
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n': n = atoi(optarg); break;
        }
    }
}

int main(int argc, char *argv[])
{

  n = 10;

  parseargs(argc, argv, n);

  assert(n > 1);

  int i;
  float secs;
  int j = 0;
  FILE *fp = fopen("rand60.txt", "r");
  while (fscanf(fp, "%f %f", &c[j].x, &c[j].y) != EOF){
    c[j].orig_index = j;
    j++;
  }
  // while (scanf("%d", &n) != EOF) {
  auto start = chrono::high_resolution_clock::now(); 
  solve();
  auto end = chrono::high_resolution_clock::now();
  cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " " << (float) minsum << "\n";
  // secs = ((float) clock() - start) / CLOCKS_PER_SEC;
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}