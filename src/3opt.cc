// mintspopt.c -- Petite chained 2-, 2h-, and 3-opt for TSP
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <chrono>

typedef double Dist; // INPUT POINTS 
Dist ptarr[101][2];
int n = 0;

#define sqr(x) ((x) * (x))
Dist dist(int i, int j) { 
  return (sqrt(sqr(ptarr[i][0] - ptarr[j][0]) +
    + sqr(ptarr[i][1] - ptarr[j][1])));
}

int p[102]; // PERM VECTOR: current tour is in p[0..n-1] 
#define dp(i, j) (dist(p[i], p[j]))
void swap(int i, int j) {
  int t = p[i];
  p[i] = p[j];
  p[j] = t;
}

void reverse(int i, int j) {// reverse p[i..j] 
  for(;i<j;i++,j--)
    swap(i, j);
}

Dist tourcost() {
  Dist sum = dp(0, n-1);
  for (int i = 1; i < n; i++)
    sum += dp(i-1, i); 
  return(sum);
}

int savedp[3][101];
Dist savedcost[3];
void saveperm(int m) {
  savedcost[m] = tourcost();
  for(int i=0; i<n; i++) 
    savedp[m][i] = p[i];
}
void restoreperm(int m) {
  for (int i = 0; i < n; i++)
    p[i] = savedp[m][i];
}
void checkperm(int m) { 
  if (tourcost() < savedcost[m])
    saveperm(m);
}

void kopt() { // Perform 2-, 2h- and 3-opting at once 
  int i, j, origtop, foundopt, shift;
  do { // k-opt as long as improving
    shift = rand() % n; // randomly rotate perm 
    reverse(0, shift);
    reverse(shift+1, n-1);
    if (rand() % 2)
      reverse(0, n-1); // and reverse it half 
    foundopt = 0;
    for (origtop = n; origtop > 0; origtop--) {
      if (origtop < n) { // Shift down
        reverse(1, n-1); // ba = (a^rb^r)^r
        reverse(0, n-1);
      }
      for (i = 3; i < origtop; i++) // 2-opt
        // Consider swap p[0]<->p[1] & p[i-1]<->p[i] 
        if ((dp(0, 1) + dp(i-1, i)) > (dp(0, i-1) + dp(1, i)) ) {
          reverse(1, i-1);
          foundopt = 1;
        }
      for (i=2;i<n-1;i++) {//2h-opt=21/2opt
        // Consider moving p[i] between p[0] and p[1]
        if((dp(0,1)+dp(i-1,i)+dp(i, i+1)) > (dp(0, i) + dp(i, 1) + dp(i-1, i+1))) {
          // move p[i] down & shift p[1..i-1] up by 1 
          reverse(1, i-1);
          reverse(1, i);
          foundopt = 1;
        }
      }
      p[n] = p[0]; // wrap around
      for (i = 2; i < n-3; i++) { // 3-opt
        for (j = i+2; j < n; j++) {
          // Consider moving p[i+1..j-1] after p[0] 
          Dist origdist, ndist1, ndist2;
          origdist = dp(0,1) + dp(i, i+1) + dp(j-1, j); 
          ndist1 = dp(0,i+1) + dp(1,j-1) + dp(i, j); 
          ndist2 = dp(0,j-1) + dp(1, i+1) + dp(i, j); 
          if (ndist1<origdist || ndist2<origdist) {
            // p[i+1..j-1] between p[0] and p[1] 
            if (ndist1 < ndist2)
              reverse(i+1, j-1); 
            reverse(1, i);
            reverse(1, j-1);
            foundopt = 1;
          }
        }
      }
    }
  } while (foundopt);
}

void kick() {// replace tour ABCD with ADCB -- double bridge
  int i, j, k; // A=[0,i-1] B=[i,j-1] C=[j,k-1] D=[k,n-1] 
  int d; // dither boundaries by Uniform[-d,d-1]
  j = n / 2;          // middle of [0, n)
  d = 1 + n / 12;
  j+=rand()%(2*d)-d;  // dither j by d
  i = j / 2;          // middle of [0, j)
  d = 1 + n / 30;
  i+=rand()%(2*d)-d;  // dither i by new d
  k = j + (n - j) / 2;     // middle of [j, n)
  k += rand() % (2*d) - d; // dither k by new d 
  reverse(i, j-1);
  reverse(j, k-1);
  reverse(k, n-1);
  reverse(0, n-1);    // ABCD = A(BrCrDr)r
}

int main() { 
  int i, j, starts = 4, runs = 250;
  while (scanf("%lf %lf", ptarr[n]+0, ptarr[n]+1) != EOF) 
    ++n;
  
  // start timer
  auto start = std::chrono::high_resolution_clock::now();

  savedcost[0] = 999999999; // init best tour ever in perm[0]
  for (i = 0; i < starts; i++) { // chained 3-opt
    for(j=0;j<n;j++) // fill array
      p[j] = j;
    for(j=n;j>1;j--) // randomly shuffle
      swap(j-1, rand() % j);
    kopt();
    checkperm(0); // best tour ever in perm[0]
    saveperm(1); // best from this start in perm[1]
    for (j=1;j<runs;j++) { // did 1 run for first
      restoreperm(1);
      kick();
      kopt();
      checkperm(1); // best from this start in perm[1] 
      checkperm(0); // best tour ever in perm[0]
    }
  }
  restoreperm(0);

  auto end = std::chrono::high_resolution_clock::now();
  int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  printf("Ran in %ld milliseconds\n", time);

  printf(" tourcost=%g\n", tourcost());
  if (1)
    for (i = 0; i < n; i++)
      printf("%g\t%g\n", ptarr[p[i]][0], ptarr[p[i]][1]);
  
  return 0;
}