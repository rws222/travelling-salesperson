// mintspopt.c -- Petite chained 2-, 2h-, and 3-opt for TSP
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <random>
#include <thread>
#include <time.h>
#include <limits>
#include <chrono>

typedef double Dist; // INPUT POINTS 
// size to make the arrays. needs to be more than the number of points you load in
const int max_size = 101;
Dist ptarr[max_size][2];
int n = 0;

// define math functions
#define sqr(x) ((x) * (x))
// distance between two points in the points array
Dist dist(int i, int j) { 
  return (sqrt(sqr(ptarr[i][0] - ptarr[j][0]) +
    + sqr(ptarr[i][1] - ptarr[j][1])));
}

// PERM VECTOR: current tour is in p[0..n-1] 
int p[102];

// distance between two indices in the current tour array
#define dp(i, j) (dist(p[i], p[j]))
// swap values at 2 indices in the current tour array
void swap(int i, int j) {
  int t = p[i];
  p[i] = p[j];
  p[j] = t;
}

// reverse p[i..j] 
void reverse(int i, int j) {
  for(; i < j; i++, j--)
    swap(i, j);
}

// gets cost of entire tour array
// could implement a parallel_reduce here, see if this runs faster on gpu also
Dist tourcost() {
  Dist sum = dp(0, n-1);
  for (int i = 1; i < n; i++)
    sum += dp(i-1, i); 
  return(sum);
}

// saved paths and costs
int savedp[3][101];
Dist savedcost[3];
// save the current tour cost, and current tour path
void saveperm(int m) {
  savedcost[m] = tourcost();
  for(int i=0; i<n; i++) 
    savedp[m][i] = p[i];
}
// restore a saved permutation back into the main tour array
void restoreperm(int m) {
  for (int i = 0; i < n; i++)
    p[i] = savedp[m][i];
}
// save the current perm only if it's cost is less that the current best
void checkperm(int m) { 
  if (tourcost() < savedcost[m])
    saveperm(m);
}

const int num_threads = 8;
// this 2d array is so that each thread can save it's own best tour.
// then, after all threads are joined you can compare and find the best one
int thread_p[num_threads][max_size];

// dp function for thread_p, not p
Dist thread_dp(int thread_num, int i, int j) {
  return dist(thread_p[thread_num][i], thread_p[thread_num][j]);
}

// swap function for the thread permutation array
void thread_swap(int thread_num, int i, int j) {
  int t = thread_p[thread_num][i];
  thread_p[thread_num][i] = thread_p[thread_num][j];
  thread_p[thread_num][j] = t;
}

// reverse function for the thread permutation array
void thread_reverse(int thread_num, int i, int j) {
  for(; i < j; i++, j--) {
    thread_swap(thread_num, i, j);
  }
}

Dist thread_tourcost(int thread_num) {
  Dist sum = thread_dp(thread_num, 0, n-1);
  for (int i = 1; i < n; i++) {
    sum += thread_dp(thread_num, i-1, i);
  }
  return sum;
}

// thread func. each thread improves its own tour in thread_p[thread_num], until it can't anymore
void thread_kopt(int thread_num) {
  // setup for rng
  static thread_local std::mt19937 gen;
  std::uniform_int_distribution<int> dis(0, INT32_MAX);
  // now, call `dis(gen)` to gen a thread-safe random number

  int i, j, foundopt, shift, origtop;
  do {
    shift = dis(gen) % n;
    thread_reverse(thread_num, 0, shift);
    thread_reverse(thread_num, shift + 1, n - 1);
    if (dis(gen) % 2) {
      thread_reverse(thread_num, 0, n - 1);
    }

    foundopt = 0;

    for (origtop = n; origtop > 0; origtop--) {
      if (origtop < n) { // Shift down
        thread_reverse(thread_num, 1, n-1); // ba = (a^rb^r)^r
        thread_reverse(thread_num, 0, n-1);
      }

      for (i = 3; i < origtop; i++) { // 2-opt
        // Consider swap p[0]<->p[1] & p[i-1]<->p[i] 
        if ((thread_dp(thread_num, 0, 1) + thread_dp(thread_num, i-1, i)) >
            (thread_dp(thread_num, 0, i-1) + thread_dp(thread_num, 1, i)) ) {
          thread_reverse(thread_num, 1, i-1);
          foundopt = 1;
        }
      }

      for (i = 2; i < n-1; i++) { //2h-opt=21/2opt
        // Consider moving p[i] between p[0] and p[1]
        if((thread_dp(thread_num, 0, 1) + thread_dp(thread_num, i-1, i) + thread_dp(thread_num, i, i+1)) >
           (thread_dp(thread_num, 0, i) + thread_dp(thread_num, i, 1) + thread_dp(thread_num, i-1, i+1))) {
          // move p[i] down & shift p[1..i-1] up by 1 
          thread_reverse(thread_num, 1, i-1);
          thread_reverse(thread_num, 1, i);
          foundopt = 1;
        }
      }
      // wrap around
      thread_p[thread_num][n] = thread_p[thread_num][0];
      for (i = 2; i < n - 3; i++) {
        for (j = i + 2; j < n; j++) {
          // Consider moving p[i+1..j-1] after p[0] 
          Dist origdist, ndist1, ndist2;
          origdist = thread_dp(thread_num, 0,1) + thread_dp(thread_num, i, i+1) + thread_dp(thread_num, j-1, j); 
          ndist1 = thread_dp(thread_num, 0,i+1) + thread_dp(thread_num, 1,j-1) + thread_dp(thread_num, i, j); 
          ndist2 = thread_dp(thread_num, 0,j-1) + thread_dp(thread_num, 1, i+1) + thread_dp(thread_num, i, j); 

          if (ndist1<origdist || ndist2<origdist) {
            // p[i+1..j-1] between p[0] and p[1] 
            if (ndist1 < ndist2) {
              thread_reverse(thread_num, i+1, j-1); 
            }
            thread_reverse(thread_num, 1, i);
            thread_reverse(thread_num, 1, j-1);
            foundopt = 1;
          }
        }
      }
    }
  } while (foundopt);
}

// Perform 2-, 2h- and 3-opting at once 
void kopt() {
  // TODO: if you can avoid this memory transfer,
  // TODO: I'm sure it will be a lot faster
  // TODO: IDEA! have a global variable that updates with the index of the current best tour in thread_p.
  // TODO: Then, instead of referring to p everywhere else, refer to thread_p[current_best]
  // first load p into each thread's spot in thread_p
  for (int i = 0; i <= n; i++) {
    for (int j = 0; j < num_threads ; j++) {
      thread_p[j][i] = p[i];
    }
  }
  // thread_p 2d tour array is now ready to be k-opted

  // spawn `num_threads` threads, each running the improve do_while
  std::vector<std::thread> threads(num_threads);
  for (int i = 0; i < num_threads; i++) {
    threads[i] = std::thread(thread_kopt, i);
  }

  // wait until all threads are done
  for (auto& t : threads) {
    t.join();
  }

  // check which tour is best in thread_p, then change p to the best one
  int best_index = 0;
  Dist current_best_tour_cost = thread_tourcost(0);
  for (int i = 1; i < num_threads; i++) {
    Dist temp_tour_cost = thread_tourcost(i);
    if (temp_tour_cost < current_best_tour_cost) {
      current_best_tour_cost = temp_tour_cost;
      best_index = i;
    }
  }

  // now you know thread_p[best_index] is the best tour, so set p to it.
  for (int i = 0; i <= n; i++) {
    p[i] = thread_p[best_index][i];
  }
  // p now contains the best tour out of the ones created by each thread
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
  int i, j;
  // do 'starts' heuristic sets, each starting from a random tour.
  int starts = 4, runs = 250;
  // get input from points file, and set n accordingly
  // `cat 'points_file' | ./parallel_3opt` to run
  while (scanf("%lf %lf", ptarr[n]+0, ptarr[n]+1) != EOF) {
    ++n;
  }

  // for (int i = 0; i < n; i++) {
  //   printf("i=%d: %lf %lf\n", i, ptarr[i][0], ptarr[i][1]);
  // }
  // printf("n: %d\n", n);

  // start timer
  auto start = std::chrono::high_resolution_clock::now();

  savedcost[0] = 999999999; // init best tour ever in perm[0]
  for (i = 0; i < starts; i++) { // chained 3-opt
    // init tour array as the initial points array
    for(j = 0; j < n; j++) {
      p[j] = j;
    }
    
    // shuffle tour array to start with a random tour
    for(j = n; j > 1; j--) {
      swap(j - 1, rand() % j);
    }

    kopt();
    checkperm(0); // best tour ever in perm[0]
    saveperm(1); // best from this start in perm[1]

    for (j = 1; j < runs; j++) { // did 1 run for first
      restoreperm(1);
      kick();
      kopt();
      checkperm(1); // best from this start in perm[1] 
      checkperm(0); // best tour ever in perm[0]
    }
  }
  restoreperm(0);
  
  // end timer
  auto end = std::chrono::high_resolution_clock::now();
  int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  printf("Ran in %ld milliseconds\n", time);

  printf(" tourcost=%g\n", tourcost());
  if (1)
    for (i = 0; i < n; i++)
      printf("%g\t%g\n", ptarr[p[i]][0], ptarr[p[i]][1]);
  
  return 0;
}