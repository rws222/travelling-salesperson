from subprocess import run
import matplotlib.pyplot as plt
from textwrap import wrap

nodes = [5, 7, 9, 11, 13]
xticks1 = [5, 7, 9, 11, 13]

# threads = [1, 3, 5, 7, 9, 11, 13, 15]
# xticks2 = [1, 3, 5, 7, 9, 11, 13, 15]

# nodes2 = [3, 5, 7, 9, 11]
# xticks3 = [3, 5, 7, 9, 11]

# brute = []
# complete = []
# parallel_complete = []
# parallel_brute = []
# for n in nodes:
#     # result = run("./bruteForce -n "+str(n), shell=True, check=True, capture_output=True)
#     # brute.append(int(result.stdout))
#     # print("./bruteForce -n "+str(n))
#     result = run("./complete -n "+str(n), shell=True, check=True, capture_output=True)
#     complete.append(int(result.stdout))
#     print("./complete -n "+str(n))
#     result = run("./parallel_complete -n "+str(n), shell=True, check=True, capture_output=True)
#     parallel_complete.append(int(result.stdout))
#     print("./parallel_complete -n "+str(n))
#     result = run("./parallel_bruteForce -n "+str(n), shell=True, check=True, capture_output=True)
#     parallel_brute.append(int(result.stdout))
#     print("./parallel_bruteForce -n "+str(n))
# fig, ax = plt.subplots()
# # ax.plot(nodes, brute, label="Brute Force")
# ax.plot(nodes, complete, label="Optimized")
# ax.plot(nodes, parallel_complete, label="Parallel Optimized")
# ax.plot(nodes, parallel_brute, label="Parallel Brute Force")
# ax.set(xlabel='Number of Nodes in Cycle', ylabel='Execution Time (μs)')
# title = ax.set_title("\n".join(wrap("Time (μs) vs. Number of Nodes in Cycle when Threads=8", 60)))
# ax.legend()
# fig.tight_layout()
# ax.set_xticks(xticks1)
# fig.savefig("plots/nVSt.png")
# plt.close(fig)

# print()

# brute = []
# complete = []
# parallel_complete = []
# parallel_brute = []
# for t in threads:
#     # result = run("./bruteForce -n 15 -t "+str(t), shell=True, check=True, capture_output=True)
#     # brute.append(int(result.stdout))
#     # print("./bruteForce -n 15 -t "+str(t))
#     result = run("./complete -n 11 -t "+str(t), shell=True, check=True, capture_output=True)
#     complete.append(int(result.stdout))
#     print("./complete -n 11 -t "+str(t))
#     result = run("./parallel_complete -n 11 -t "+str(t), shell=True, check=True, capture_output=True)
#     parallel_complete.append(int(result.stdout))
#     print("./parallel_complete -n 11 -t "+str(t))
#     result = run("./parallel_bruteForce -n 11 -t "+str(t), shell=True, check=True, capture_output=True)
#     parallel_brute.append(int(result.stdout))
#     print("./parallel_bruteForce -n 11 -t "+str(t))
# fig, ax = plt.subplots()
# # ax.plot(threads, brute, label="Brute Force")
# ax.plot(threads, complete, label="Optimized")
# ax.plot(threads, parallel_complete, label="Parallel Optimized")
# ax.plot(threads, parallel_brute, label="Parallel Brute Forc")
# ax.set(xlabel='Number of Threads', ylabel='Execution Time (μs)')
# title = ax.set_title("\n".join(wrap("Time (μs) vs. Threads when Nodes=11", 60)))
# ax.legend()
# fig.tight_layout()
# ax.set_xticks(xticks2)
# fig.savefig("plots/tVSt.png")
# plt.close(fig)

# print()

# brute = []
# complete = []
# parallel_complete = []
# parallel_brute = []
# for n in nodes2:
#     result = run("./bruteForce -n "+str(n), shell=True, check=True, capture_output=True)
#     brute.append(int(result.stdout))
#     print("./bruteForce -n "+str(n))
#     result = run("./complete -n "+str(n), shell=True, check=True, capture_output=True)
#     complete.append(int(result.stdout))
#     print("./complete -n "+str(n))
#     result = run("./parallel_complete -n "+str(n), shell=True, check=True, capture_output=True)
#     parallel_complete.append(int(result.stdout))
#     print("./parallel_complete -n "+str(n))
#     result = run("./parallel_bruteForce -n "+str(n), shell=True, check=True, capture_output=True)
#     parallel_brute.append(int(result.stdout))
#     print("./parallel_bruteForce -n "+str(n))
# fig, ax = plt.subplots()
# ax.plot(nodes2, brute, label="Brute Force")
# ax.plot(nodes2, complete, label="Optimized")
# ax.plot(nodes2, parallel_complete, label="Parallel Optimized")
# ax.plot(nodes2, parallel_brute, label="Parallel Brute Force")
# ax.set(xlabel='Number of Nodes in Cycle', ylabel='Execution Time (μs)')
# title = ax.set_title("\n".join(wrap("Time (μs) vs. Number of Nodes in Cycle when Threads=8", 60)))
# ax.legend()
# fig.tight_layout()
# ax.set_xticks(xticks3)
# fig.savefig("plots/n2VSt.png")
# plt.close(fig)

# print()

# p_comp = []
# v_comp = []
# comp = []
# for n in nodes:
#     sum_p = 0
#     sum_v = 0
#     sum_c = 0
#     for i in range(5):
#         result = run("./vector_complete -n "+str(n), shell=True, check=True, capture_output=True)
#         sum_v = sum_v + int(result.stdout)
#     v_comp.append(sum_v/5)
#     print("./vector_complete -n "+str(n))
#     for i in range(5):
#         result = run("./complete -n "+str(n), shell=True, check=True, capture_output=True)
#         sum_c = sum_c + int(result.stdout)
#     comp.append(sum_c/5)
#     print("./complete -n "+str(n))
#     # for i in range(5):
#     #     result = run("./parallel_complete -n "+str(n), shell=True, check=True, capture_output=True)
#     #     sum_p = sum_p + int(result.stdout)
#     # p_comp.append(sum_p/5)
#     # print("./parallel_complete -n "+str(n))
# fig, ax = plt.subplots()
# ax.plot(nodes, v_comp, label="Vector Complete")
# ax.plot(nodes, comp, label="Complete")
# # ax.plot(nodes, p_comp, label="Parallel Complete")
# ax.set(xlabel='Number of Nodes in Cycle', ylabel='Execution Time (μs)')
# title = ax.set_title("\n".join(wrap("Time (μs) vs. Number of Nodes in Cycle", 60)))
# ax.legend()
# fig.tight_layout()
# ax.set_xticks(xticks1)
# fig.savefig("plots/vVSt.png")
# plt.close(fig)

# print()

p_bf = []
bf = []
for n in nodes:
    sum_bf = 0
    sum_pbf = 0
    for i in range(5):
        result = run("./build/parallel_bruteForce -n "+str(n)+" -t 16", shell=True, check=True, capture_output=True)
        sum_pbf = sum_pbf + int(result.stdout)
    p_bf.append(sum_pbf/5)
    print("./build/parallel_bruteForce -n "+str(n)+" -t 16")
    for i in range(5):
        result = run("./build/bruteForce -n "+str(n)+" -t 16", shell=True, check=True, capture_output=True)
        sum_bf = sum_bf + int(result.stdout)
    bf.append(sum_bf/5)
    print("./build/bruteForce -n "+str(n)+" -t 16")
fig, ax = plt.subplots()
ax.plot(nodes, p_bf, label="Parallel Brute Force")
ax.plot(nodes, bf, label="Brute Force")
# ax.plot(nodes, p_comp, label="Parallel Complete")
ax.set(xlabel='Number of Nodes in Cycle', ylabel='Execution Time (μs)')
title = ax.set_title("\n".join(wrap("Time (μs) vs. Number of Nodes in Cycle when Threads=16", 60)))
ax.legend()
fig.tight_layout()
ax.set_xticks(xticks1)
fig.savefig("../plots/bfVSt.png")
plt.close(fig)

print()
