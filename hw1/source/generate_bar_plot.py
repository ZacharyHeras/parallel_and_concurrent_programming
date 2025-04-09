import math

import matplotlib.pyplot as plt


def main():
    tests = {}
    tests['Test case 1'] = [36.10, 36.11, 36.1]
    tests['Test case 2'] = [913.24, 913.24, 913.08]
    tests['Test case 3'] = [4449.08, 4448.15, 4452.05]
    tests['Test case 4'] = [3661.31, 3663.24, 3663.42]
    tests['Test case 5'] = [35548.96, 35643.70, 35520.12]

    for test_case, test_results in tests.items():
        print(sum(test_results)/len(test_results))
        plt.bar(int(test_case[-1]),
                math.log10(sum(test_results)/len(test_results)),
                label=test_case)

    plt.title('Average Execution Time per Test Case')
    plt.xlabel('Test Case')
    plt.ylabel('Log10 of Execution Time')

    plt.show()


if __name__ == '__main__':
    main()
