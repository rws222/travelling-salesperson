// tspfinal.c -- Experiments on fast TSP algorithm 
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

// GENERAL SUPPORT
void swap(int i, int j) { 
  int t = p[i]; p[i] = p[j]; p[j] = t;
} 
#define dist2(i, j) (distarr[i][j])
atomic<Dist> maxdiamdist;
// int maxdiamindexi;
atomic_int maxdiamindexi;

void check(Dist sum) { 
  int i;
  sum += dist2(p[0], p[n-1]);
  if (sum < minsum) {
    minsum = sum;
    tbb::parallel_for(0, n, [&] (int i) {
        minp[i] = p[i];
    });
    // for (i = 0; i < n; i++)
    //   minp[i] = p[i];
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
  // Dist result = tbb::parallel_reduce(
  //   tbb::blocked_range<Tptr>(bin[h], NULL),
  //   Dist(),
  //   [&](const tbb::blocked_range<Tptr>& range, Dist init) -> Dist {
  //     for (Tptr p = range.begin(); p != range.end(); ++p) {
  //       if (p->arg == mask) {
  //         init = p->val;
  //         break;
  //       }
  //     }
  //     return init;
  //   },
  //   [](Dist x, Dist y) -> Dist {
  //     return x + y;
  //   });

  // if (result > 0) {
  //   return result;
  // }
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
    // tbb::parallel_for(0, m, [&] (int i) {
    //     order[i] = i;
    // });
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
  // tbb::parallel_for(0, n, [&] (int i) {
  //   tbb::parallel_for(0, n, [&] (int j) {
  //       Dist td = distarr[i][j] = geomdist(i, j);
  //       if (td > maxdiamdist) {
  //           maxdiamdist = td; 
  //           maxdiamindexi = i;
  //       }
  //       });
  //   });
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      Dist td = distarr[i][j] = geomdist(i, j); 
      if (td > maxdiamdist) {
        maxdiamdist = td; 
        maxdiamindexi = i;
      }
    }
  }

  // tbb::parallel_for(0, MAXN, [&] (int i) {
  //   bit[i] = (Mask) 1 << i;
  // });
  for (i = 0; i < MAXN; i++) {
    bit[i] = (Mask) 1 << i;
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

  FILE *fp = fopen("datasets/rand60.txt", "r");
  int j = 0;
  while (fscanf(fp, "%f %f", &c[j].x, &c[j].y) != EOF)
    j++;
  // while (scanf("%d", &n) != EOF) {
  auto start = chrono::high_resolution_clock::now(); 
  solve();
  auto end = chrono::high_resolution_clock::now();
  // cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " " << (float) minsum << "\n";
  cout << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";
  // secs = ((float) clock() - start) / CLOCKS_PER_SEC;
  // printf("%d\t%7.2f\t%10.4f\n", n, secs, (float) minsum);
  // }
  return 0;
}
