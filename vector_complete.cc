// tspfinal.c -- Experiments on fast TSP algorithm 
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <chrono>
#include <immintrin.h>

// Data types and sizes 
#define MASKTYPE long 
#define MAXN 60
#define MAXBIN 9999991 
typedef float Dist; 
#define INF ((Dist) 1e20) 
#define ZERO ((Dist) 0)

using namespace std;

// Globals: points and perm vectors 
int n;
typedef struct point {
  float x;
  float y; 
} Point;
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

// Dist* geomdist(int i, int j) {
//   Dist* result = (Dist*) malloc(sizeof(float) * 8);
//   __m256i x = _mm256_setr_epi32(c[i].x, c[i].x, c[i].x, c[i].x, c[i].x, c[i].x, c[i].x, c[i].x);
//   __m256i y = _mm256_setr_epi32(c[i].y, c[i].y, c[i].y, c[i].y, c[i].y, c[i].y, c[i].y, c[i].y);
//   __m256i j_offsets = _mm256_setr_epi32(j, j+1, j+2, j+3, j+4, j+5, j+6, j+7);
//   __m256i jx = _mm256_i32gather_epi32((int*) &c[0].x, j_offsets, sizeof(Point));
//   __m256i jy = _mm256_i32gather_epi32((int*) &c[0].y, j_offsets, sizeof(Point));
//   __m256i dx = _mm256_sub_epi32(x, jx);
//   __m256i dy = _mm256_sub_epi32(y, jy);
//   __m256i dx2 = _mm256_mullo_epi32(dx, dx);
//   __m256i dy2 = _mm256_mullo_epi32(dy, dy);
//   __m256i sum = _mm256_add_epi32(dx2, dy2);
//   __m256 sqrts = _mm256_sqrt_ps(_mm256_cvtepi32_ps(sum));
//   _mm256_store_ps(result, sqrts);
//   return result;
// }

// Dist* geomdist(int p1, int p2) {
//     __m256 x1 = _mm256_set1_ps(c[p1].x);
//     __m256 y1 = _mm256_set1_ps(c[p1].y);
//     __m256 x2 = _mm256_set_ps(c[p2].x, c[p2].x, c[p2].x, c[p2].x, c[p2].x, c[p2].x, c[p2].x, c[p2].x);
//     __m256 y2 = _mm256_set_ps(c[p2].y, c[p2].y, c[p2].y, c[p2].y, c[p2].y, c[p2].y, c[p2].y, c[p2].y);
//     __m256 dx = _mm256_sub_ps(x1, x2);
//     __m256 dy = _mm256_sub_ps(y1, y2);
//     __m256 dx2 = _mm256_mul_ps(dx, dx);
//     __m256 dy2 = _mm256_mul_ps(dy, dy);
//     __m256 dist2 = _mm256_add_ps(dx2, dy2);
//     __m256 dist = _mm256_sqrt_ps(dist2);
//     Dist dist_arr[4];
//     _mm256_store_ps(dist_arr, dist);
//     return dist_arr[0];
// }

// GENERAL SUPPORT
void swap(int i, int j) { 
  int t = p[i]; p[i] = p[j]; p[j] = t;
} 
#define dist2(i, j) (distarr[i][j])
Dist maxdiamdist;
int maxdiamindexi;

void check(Dist sum) { 
  int i;
  sum += dist2(p[0], p[n-1]);
  if (sum < minsum) {
    minsum = sum;
    for (i = 0; i < n; i++)
      minp[i] = p[i];
  }
}

// MST
typedef MASKTYPE Mask; 
Mask bit[MAXN];
struct link {
  int city; 
  int nnfrag; 
  Dist nndist;
} q[MAXN];

Dist mstdist(Mask mask) { 
  int i, m, mini, newcity, n;
  Dist mindist, thisdist, totaldist; 
  totaldist = ZERO;
  n = 0;
  for (i = 0; i < MAXN; i++) {
    if (mask & bit[i])
      q[n++].city = i;
  }
  newcity = q[n-1].city;
  for (i = 0; i < n-1; i++) 
    q[i].nndist = INF;
  for (m = n-1; m > 0; m--) {
    mindist = INF;
    for (i = 0; i < m; i++) {
      thisdist = dist2(q[i].city, newcity);
      if (thisdist < q[i].nndist) {
        q[i].nndist = thisdist;
        q[i].nnfrag = newcity; 
      }
      if (q[i].nndist < mindist) { 
        mindist = q[i].nndist; 
        mini = i;
      }
    }
    newcity = q[mini].city; 
    totaldist += mindist; 
    q[mini] = q[m-1];
  }
  return totaldist;
} 
Mask allinmask;

// HASH TABLE FOR MST LENGTHS
typedef struct tnode *Tptr;
typedef struct tnode {
  Mask arg;
  Dist val;
  Tptr next;
} Tnode;
Tptr bin[MAXBIN];

Dist mstdistlookup(Mask mask) { 
  Tptr p;
  int h = mask % MAXBIN;
  for (p = bin[h]; p != NULL; p = p->next)
    if (p->arg == mask) 
      return p->val;
  p = (Tptr) malloc(sizeof(Tnode)); 
  p->arg = mask;
  p->val = mstdist(mask);
  p->next = bin[h];
  bin[h] = p; 
  return p->val;
}

// MAIN SEARCH 
void search(int m, Dist sum, Mask mask) {
  int i, j, top, half, t;
  int order[MAXN]; // Another perm vector, sorted in distance from end 
  if (sum + mstdistlookup(mask | bit[p[m]] | bit[p[n-1]]) > minsum)
    return; 
  if (m == 1) {
    check(sum + dist2(p[0], p[1]));
  } else {
    for (i = 0; i < m; i++) {
      order[i] = i;
    }
    for (top = m; top > 1; top -= half) { // Approx sort via tourn
      half = top / 2;
      for (i=0, j=top-half; i<half; i++, j++) {
        if (dist2(p[order[i]], p[m]) > dist2(p[order[j]], p[m])) {
          t = order[i];
          order[i] = order[j];
          order[j] = t;
        }
      }
    }
    for (i = 0; i < m; i++) {
      swap(order[i], m-1); 
      search(m-1, sum + dist2(p[m-1], p[m]), mask & ~bit[p[m-1]]);
      swap(order[i], m-1);
    }
  }
}

void solve() { 
  int i, j;
  maxdiamdist = ZERO;

//   for (i = 0; i < n; i++) {
//     for (j = 0; j < n; j++) {
//       Dist td = distarr[i][j] = geomdist(i, j); 
//       if (td > maxdiamdist) {
//         maxdiamdist = td; 
//         maxdiamindexi = i;
//       }
//     }
//   }

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j+=8) {
        // float* dist_ptr = (float*) geomdist(i,j);
        // __m256 A=_mm256_loadu_ps(dist_ptr);
        // _mm256_store_ps(&distarr[i][j], A);
        // free(dist_ptr);
        // __m256 mask = _mm_set_256(geomdist(i,j+7), geomdist(i,j+6), geomdist(i,j+5), geomdist(i,j+4), geomdist(i,j+3), geomdist(i,j+2), geomdist(i,j+1), geomdist(i,j));
        // _mm_store_256ps((__m256 *)&distarr[i][j], mask);
        __m256 mask = _mm256_set_ps(geomdist(i,j+7), geomdist(i,j+6), geomdist(i,j+5), geomdist(i,j+4), geomdist(i,j+3), geomdist(i,j+2), geomdist(i,j+1), geomdist(i,j));
        _mm256_store_ps(&distarr[i][j], mask);
    }
  }

//   for (i = 0; i < MAXN; i++) {
//     bit[i] = (Mask) 1 << i;
//   }
  for (i = 0; i < (MAXN/4); i++) {
    __m128i mask = _mm_set_epi32(1 << (i+3), 1 << (i+2), 1 << (i+1), 1 << i);
    _mm_store_si128((__m128i *)&bit[i], mask);
  }

  allinmask = 0;
  for (i = 0; i < n; i++) {
    allinmask |= bit[i];
    p[i] = i;
  }
  minsum = INF;
  for (i = 0; i < MAXBIN; i++) {
    bin[i] = NULL;
    swap(n-1, maxdiamindexi);
  }
  search(n-1, ZERO, allinmask & ~bit[p[n-1]]);
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
  FILE *fp = fopen("rand60.txt", "r");
  int j = 0;
  while (fscanf(fp, "%f %f", &c[j].x, &c[j].y) != EOF)
    j++;
  // while (scanf("%d", &n) != EOF) {
  auto start = chrono::high_resolution_clock::now(); 
  solve();
  auto end = chrono::high_resolution_clock::now();
  cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " " << (float) minsum << "\n";
//   cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
  // secs = ((float) clock() - start) / CLOCKS_PER_SEC;
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}