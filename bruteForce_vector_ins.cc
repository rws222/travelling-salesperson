// tspfinal.c -- Experiments on fast TSP algorithm 
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <immintrin.h>

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

double dist(Point p1, Point p2) {
    __m256d x1 = _mm256_set1_pd(p1.x);
    __m256d y1 = _mm256_set1_pd(p1.y);
    __m256d x2 = _mm256_set_pd(p2.x, p2.x, p2.x, p2.x);
    __m256d y2 = _mm256_set_pd(p2.y, p2.y, p2.y, p2.y);
    __m256d dx = _mm256_sub_pd(x1, x2);
    __m256d dy = _mm256_sub_pd(y1, y2);
    __m256d dx2 = _mm256_mul_pd(dx, dx);
    __m256d dy2 = _mm256_mul_pd(dy, dy);
    __m256d dist2 = _mm256_add_pd(dx2, dy2);
    __m256d dist = _mm256_sqrt_pd(dist2);
    double dist_arr[4];
    _mm256_store_pd(dist_arr, dist);
    return dist_arr[0];
}

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
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j += 4) {
        Point p1 = c[i];
        Point p2 = c[j];
        double d1 = dist(p1, p2);
        double d2 = dist(p1, c[j+1]);
        double d3 = dist(p1, c[j+2]);
        double d4 = dist(p1, c[j+3]);
        distarr[i][j] = d1;
        distarr[i][j+1] = d2;
        distarr[i][j+2] = d3;
        distarr[i][j+3] = d4;
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