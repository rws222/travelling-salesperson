// tspfinal.c -- Experiments on fast TSP algorithm 
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <oneapi/tbb.h>

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

// Dist geomdist(Point a, Point b) { 
//   return (Dist) sqrt(
//     sqr(a.x - b.x) + sqr(a.y - b.y)
//   );
// }
Dist geomdist(int i, int j) { 
  return (Dist) (sqrt(sqr(c[i].x-c[j].x) + sqr(c[i].y-c[j].y)));
}

void solve() { 
    int i, j;
    // for (i = 0; i < n; i++) {
    //     for (j = 0; j < n; j++) {
    //       distarr[i][j] = geomdist(i, j);
    //     }
    // }
    tbb::flow_control gc(tbb::global_control::max_allowed_parallelism, 16);

    tbb::parallel_for(0, n, [&](int i){
      tbb::parallel_for(0, n, [&](int j){
        distarr[i][j] = geomdist(i, j);
      });
    });

    Dist optimal_cost = INF;
    Point optimal_path[MAXN];
    do {
        Dist temp_cost = 0;
        for(int i = 1 ; i < n; i++){
            temp_cost += distarr[c[i-1].orig_index][c[i].orig_index];
        }

        // temp_cost = tbb::parallel_reduce(
        //   tbb::blocked_range<int>(1, n),
        //   0.0,
        //   [&](tbb::blocked_range<int> r, double running_sum) {
        //     for (int i = r.begin(); i < r.end(); ++i) {
        //       running_sum += distarr[c[i-1].orig_index][c[i].orig_index];
        //     }
        //     return running_sum;
        //   },
        //   std::plus<double>()
        // );

        if(temp_cost < optimal_cost){
            optimal_cost = temp_cost;
            minsum = optimal_cost;
            for(int j = 0; j < n ; j++)
              optimal_path[j] = c[j];
        }
    } while(next_permutation(c+1, c+n));
}

// DRIVER
int main(int argc, char *argv[]) { 
  int i, start;
  float secs;
  FILE *fp = fopen("rand60.txt", "r");
  n = 0;
  while (fscanf(fp, "%f %f", &c[n].x, &c[n].y) != EOF){
    c[n].orig_index = n;
    n++;
  }
  while (scanf("%d", &n) != EOF) {
    start = clock();
    solve();
    secs = ((float) clock() - start) / CLOCKS_PER_SEC; 
    printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  }
  return 0;
}