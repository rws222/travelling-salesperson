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
mutex m;
int minp[MAXN];
Dist distarr[MAXN][MAXN];

float sqr(float x) { 
  return x*x; 
}

Dist geomdist(int i, int j) { 
  return (Dist) (sqrt(sqr(c[i].x-c[j].x) + sqr(c[i].y-c[j].y)));
}

unsigned int factorial(unsigned int n) {
    if (n == 0)
       return 1;
    return n * factorial(n - 1);
}

void solve() { 
    int i, j;
    int size = (sizeof(c)/sizeof(*c));
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
          distarr[i][j] = geomdist(i, j); 
        }
    }
    // atomic<Dist> optimal_cost = INF;
    // Point optimal_path[n];

    atomic<int> count{0};

    tbb::parallel_for(tbb::blocked_range<int>(2, n+1), [&](const tbb::blocked_range<int>& r) {
        for (int i = r.begin(); i != r.end(); ++i) {
          Point copy_c[MAXN];
          copy(begin(c), end(c), begin(copy_c));
          do {
            count++;
              Dist temp_cost = 0;
              for (int j = 1; j < n; j++) {
                  temp_cost += distarr[copy_c[j-1].orig_index][copy_c[j].orig_index];
              }
              temp_cost += distarr[copy_c[n-1].orig_index][copy_c[0].orig_index];
              if (temp_cost < minsum) {
                  minsum = temp_cost;
              }
          } while (std::next_permutation(copy_c+1, copy_c+i));
        }
    });
  cout << count << "\n";
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
  int t = 8;

  parseargs(argc, argv, n, t);

  assert(n > 1 &&  t > 0);

  tbb::global_control gc(tbb::global_control::max_allowed_parallelism, t);

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
  // cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}