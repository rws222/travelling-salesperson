// tspfinal.c -- Experiments on fast TSP algorithm 
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <immintrin.h>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <tbb/tbb.h>

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

// Dist geomdist(int i, int j) { 
//   return (Dist) (sqrt(sqr(c[i].x-c[j].x) + sqr(c[i].y-c[j].y)));
// }

// double dist(Point p1, Point p2) {
//     __m256d x1 = _mm256_set1_pd(p1.x);
//     __m256d y1 = _mm256_set1_pd(p1.y);
//     __m256d x2 = _mm256_set_pd(p2.x, p2.x, p2.x, p2.x);
//     __m256d y2 = _mm256_set_pd(p2.y, p2.y, p2.y, p2.y);
//     __m256d dx = _mm256_sub_pd(x1, x2);
//     __m256d dy = _mm256_sub_pd(y1, y2);
//     __m256d dx2 = _mm256_mul_pd(dx, dx);
//     __m256d dy2 = _mm256_mul_pd(dy, dy);
//     __m256d dist2 = _mm256_add_pd(dx2, dy2);
//     __m256d dist = _mm256_sqrt_pd(dist2);
//     double dist_arr[4];
//     _mm256_store_pd(dist_arr, dist);
//     return dist_arr[0];
// }

// void solve() { 
//     int i, j;
//     for (i = 0; i < n; i++) {
//         for (j = 0; j < n; j++) {
//         Dist td = distarr[i][j] = geomdist(i, j); 
//         }
//     }
//     Dist optimal_cost = INF;
//     Point optimal_path[MAXN];
//     do {
//         Dist temp_cost = 0;
//         for(int i = 1 ; i < n; i++){
//             temp_cost += distarr[c[i-1].orig_index][c[i].orig_index];
//         }
//         if(temp_cost < optimal_cost){
//             optimal_cost = temp_cost;
//             minsum = optimal_cost;
//             for(int j = 0; j < n ; j++)
//               optimal_path[j] = c[j];
//         }
//     } while(next_permutation(c+1, c+n));
// }

void solve() {
    int i, j;
    Dist distarr[MAXN][MAXN];
    // for (i = 0; i < n; i++) {
    //     for (j = 0; j < n; j++) {
    //         distarr[i][j] = geomdist(i, j);
    //     }
    // }
    // calculate distance matrix using vector instructions
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j += 4) {
        __m256d x1 = _mm256_set1_pd(c[i].x);
        __m256d y1 = _mm256_set1_pd(c[i].y);
        __m256d x2 = _mm256_set_pd(c[j+3].x, c[j+2].x, c[j+1].x, c[j].x);
        __m256d y2 = _mm256_set_pd(c[j+3].y, c[j+2].y, c[j+1].y, c[j].y);
        __m256d dx = _mm256_sub_pd(x1, x2);
        __m256d dy = _mm256_sub_pd(y1, y2);
        __m256d dx2 = _mm256_mul_pd(dx, dx);
        __m256d dy2 = _mm256_mul_pd(dy, dy);
        __m256d dist2 = _mm256_add_pd(dx2, dy2);
        __m256d dist = _mm256_sqrt_pd(dist2);
        double dist_arr_4[4];
        _mm256_store_pd(dist_arr_4, dist);
        distarr[i][j] = dist_arr_4[0];
        distarr[i][j+1] = dist_arr_4[1];
        distarr[i][j+2] = dist_arr_4[2];
        distarr[i][j+3] = dist_arr_4[3];
      }
    }
    Dist optimal_cost = INF;
    Point optimal_path[MAXN];
    do {
        Dist temp_cost = 0;
        for(int i = 1 ; i < n; i++){
            temp_cost += distarr[c[i-1].orig_index][c[i].orig_index];
        }
        temp_cost += distarr[c[n-1].orig_index][c[0].orig_index];
        if(temp_cost < optimal_cost){
            optimal_cost = temp_cost;
            minsum = optimal_cost;
            for(int j = 0; j < n ; j++)
              optimal_path[j] = c[j];
        }
    } while(next_permutation(c+1, c+n));
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
  // cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " " << (float) minsum << "\n";
  // secs = ((float) clock() - start) / CLOCKS_PER_SEC;
  cout << "duration: " << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
  cout << "minsum: " << minsum << endl;
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}