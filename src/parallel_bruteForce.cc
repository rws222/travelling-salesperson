#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <chrono>
#include <atomic>
#include <tbb/tbb.h>
#include <mutex>
#include <algorithm>
#include <iterator>
#include <thread>
#include <numeric>
#include <stdexcept>
#include <vector>

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
atomic<Dist> minsum = INF;
// Dist minsum = INF;
mutex m;
int t;
int minp[MAXN];
Dist distarr[MAXN][MAXN];

float sqr(float x) { 
  return x*x; 
}

Dist geomdist(int i, int j) { 
  return (Dist) (sqrt(sqr(c[i].x-c[j].x) + sqr(c[i].y-c[j].y)));
}

int factorial(int n) {
    if (n == 0)
       return 1;
    return n * factorial(n - 1);
}

void kth_permutation(int n, int k) {

  // Create a vector of integers from 0 to n
  std::vector<int> perm(n);
  std::iota(perm.begin(), perm.end(), 0);

  // Compute the factorial of n-1
  int f = 1;
  for (int i = 1; i < n; ++i) {
    f *= i;
  }

  // Generate the kth permutation using factorial number system
  for (int i = 0; i < n - 1; ++i) {
    int j = i + k / f;
    k %= f;
    f /= n - 1 - i;
    std::swap(perm[i], perm[j]);
  }

  Dist temp_cost = 0;
  temp_cost += distarr[c[0].orig_index][c[perm[0]+1].orig_index];
  for (int x = 1; x < n; x++) {
    temp_cost += distarr[c[perm[x-1]+1].orig_index][c[perm[x]+1].orig_index];
  }
  temp_cost += distarr[c[perm[n-1]+1].orig_index][c[0].orig_index];
        
  if (temp_cost < minsum) {
      minsum = temp_cost;
  }

  return;
}

void solve() { 
    int i, j;
    int size = (sizeof(c)/sizeof(*c));
    tbb::parallel_for(0, n, [&](int i) {
        tbb::parallel_for(0, n, [&](int j) {
            distarr[i][j] = geomdist(i, j); 
        });
      });
    // for (i = 0; i < n; i++) {
    //     for (j = 0; j < n; j++) {
    //       distarr[i][j] = geomdist(i, j); 
    //     }
    // }
    tbb::parallel_for(0, factorial(n-1), [&](int k){
      kth_permutation(n-1, k);
    });

    // atomic<Dist> optimal_cost = INF;
    // Point optimal_path[n];

}

// Report on how to use the command line to configure this program
void usage() {
    std::cout
        << "Command-Line Options:" << std::endl
        << " -n <int> : number of nodes in path" << std::endl 
        << " -t <int> : number of threads" << std:: endl << std::endl;
    exit(0);
}

// Parse command line arguments using getopt()
void parseargs(int argc, char** argv, int& n, int& t) {
    // parse the command-line options
    int opt;
    while ((opt = getopt(argc, argv, "n:t:")) != -1) {
        switch (opt) {
            case 'n': n = atoi(optarg);
            case 't': t = atoi(optarg); break;
        }
    }
}

int main(int argc, char *argv[])
{

  n = 10;
  t = 8;

  parseargs(argc, argv, n, t);

  assert(n > 1 &&  t > 0);

  tbb::global_control gc(tbb::global_control::max_allowed_parallelism, t);

  int i;
  float secs;
  int j = 0;
  FILE *fp = fopen("datasets/rand60.txt", "r");
  while (fscanf(fp, "%f %f", &c[j].x, &c[j].y) != EOF){
    c[j].orig_index = j;
    j++;
  }
  // while (scanf("%d", &n) != EOF) {
  auto start = chrono::high_resolution_clock::now(); 
  solve();
  auto end = chrono::high_resolution_clock::now();
  // cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " " << (float) minsum << "\n";
  // secs = ((float) clock() - start) / CLOCKS_PER_SEC;
  cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}