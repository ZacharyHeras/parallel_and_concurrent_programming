import matplotlib.pyplot as plt


def main():
    thread_counts = [1, 2, 4, 8, 10, 16, 20]
    speedups = [1.00, 1.88, 3.27, 3.30, 2.73, 2.06, 1.94]
    efficiencies = [1.00, 0.94, 0.82, 0.41, 0.27, 0.13, 0.10]

    for thread_count, speedup in zip(thread_counts, speedups):
        plt.bar(thread_count, speedup, label=str(thread_count))

    plt.title('Speedup vs. Thread Count')
    plt.xlabel('Thread Count')
    plt.ylabel('Speedup')
    plt.show()

    for thread_count, efficiency in zip(thread_counts, efficiencies):
        plt.bar(thread_count, efficiency, label=str(thread_count))

    plt.title('Efficiency vs. Thread Count')
    plt.xlabel('Thread Count')
    plt.ylabel('Efficiency')
    plt.show()


if __name__ == '__main__':
    main()
