from subprocess import run
import matplotlib.pyplot as plt
from textwrap import wrap

nodes = [5, 7, 9, 11, 13, 15]
xticks1 = [5, 7, 9, 11, 13, 15]

threads = [1, 3, 5, 7, 9, 11, 13, 15]
xticks2 = [1, 3, 5, 7, 9, 11, 13, 15]

brute = []
complete = []
parallel_complete = []
parallel_complete = []
for n in nodes:
    result = run("./bruteForce -n "+str(n), shell=True, check=True, capture_output=True)
    brute.append(int(result.stdout))
    print("./bruteForce -n "+str(n))
    result = run("./complete -n "+str(n), shell=True, check=True, capture_output=True)
    complete.append(int(result.stdout))
    print("./complete -n "+str(n))
    result = run("./parallel_complete -n "+str(n), shell=True, check=True, capture_output=True)
    parallel_complete.append(int(result.stdout))
    print("./parallel_complete -n "+str(n))
fig, ax = plt.subplots()
ax.plot(nodes, brute, label="Brute Force")
ax.plot(nodes, complete, label="Optimized")
ax.plot(nodes, parallel_complete, label="Parallel Optimized")
ax.set(xlabel='Number of Nodes in Cycle', ylabel='Execution Time (μs)')
title = ax.set_title("\n".join(wrap("Time (μs) vs. Number of Nodes in Cycle when Threads=8", 60)))
ax.legend()
fig.tight_layout()
ax.set_xticks(xticks1)
fig.savefig("plots/nVSt.png")
plt.close(fig)

print()

brute = []
complete = []
parallel_complete = []
for t in threads:
    result = run("./bruteForce -n 15 -t "+str(t), shell=True, check=True, capture_output=True)
    brute.append(int(result.stdout))
    print("./bruteForce -n 15 -t "+str(t))
    result = run("./complete -n 15 -t "+str(t), shell=True, check=True, capture_output=True)
    complete.append(int(result.stdout))
    print("./complete -n 15 -t "+str(t))
    result = run("./parallel_complete -n 15 -t "+str(t), shell=True, check=True, capture_output=True)
    parallel_complete.append(int(result.stdout))
    print("./parallel_complete -n 15 -t "+str(t))
fig, ax = plt.subplots()
ax.plot(threads, brute, label="Brute Force")
ax.plot(threads, complete, label="Optimized")
ax.plot(threads, parallel_complete, label="Parallel Optimized")
ax.set(xlabel='Number of Threads', ylabel='Execution Time (μs)')
title = ax.set_title("\n".join(wrap("Time (μs) vs. Threads when Nodes=15", 60)))
ax.legend()
fig.tight_layout()
ax.set_xticks(xticks2)
fig.savefig("plots/tVSt.png")
plt.close(fig)

print()