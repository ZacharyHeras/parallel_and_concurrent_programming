#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define INPUT_SIZE 3
#define HIDDEN_LAYERS 100
#define OUTPUT_SIZE 1
#define TRAINING_SET_SIZE 1000
#define EPOCHS 15000
#define LEARNING_RATE 0.000001
#define THREAD_COUNT 6

double get_time(void) {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (double)tval.tv_sec + (double)tval.tv_usec / 1000000.0;
}

double relu(double x) {
    return x > 0 ? x : 0.0;
}

double d_relu(double x) {
    return x > 0 ? 1.0 : 0.0;
}

double get_random_weight() {
    return 2.0 * ((double)rand() / RAND_MAX) - 1.0;
}

typedef struct {
    double input[INPUT_SIZE];
    double hidden_layer_1[HIDDEN_LAYERS];
    double hidden_layer_2[HIDDEN_LAYERS];
    double output[OUTPUT_SIZE];

    double weights_hidden_1[INPUT_SIZE][HIDDEN_LAYERS];
    double weights_hidden_2[HIDDEN_LAYERS][HIDDEN_LAYERS];
    double weights_output[HIDDEN_LAYERS][OUTPUT_SIZE];

    double bias_hidden_1[HIDDEN_LAYERS];
    double bias_hidden_2[HIDDEN_LAYERS];
    double bias_output[OUTPUT_SIZE];
} NeuralNetwork;

typedef struct {
    double weights_hidden_1[INPUT_SIZE][HIDDEN_LAYERS];
    double weights_hidden_2[HIDDEN_LAYERS][HIDDEN_LAYERS];
    double weights_output[HIDDEN_LAYERS][OUTPUT_SIZE];

    double bias_hidden_1[HIDDEN_LAYERS];
    double bias_hidden_2[HIDDEN_LAYERS];
    double bias_output[OUTPUT_SIZE];
} NeuralNetworkGradients;

void initialize_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_SIZE; i++)
        for (int j = 0; j < HIDDEN_LAYERS; j++)
            nn->weights_hidden_1[i][j] = get_random_weight();

    for (int i = 0; i < HIDDEN_LAYERS; i++)
        for (int j = 0; j < HIDDEN_LAYERS; j++)
            nn->weights_hidden_2[i][j] = get_random_weight();

    for (int i = 0; i < HIDDEN_LAYERS; i++)
        for (int j = 0; j < OUTPUT_SIZE; j++)
            nn->weights_output[i][j] = get_random_weight();

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->bias_hidden_1[i] = get_random_weight();
        nn->bias_hidden_2[i] = get_random_weight();
    }

    for (int i = 0; i < OUTPUT_SIZE; i++)
        nn->bias_output[i] = get_random_weight();
}

void forward(NeuralNetwork *nn) {
    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->hidden_layer_1[i] = nn->bias_hidden_1[i];
        for (int j = 0; j < INPUT_SIZE; j++)
            nn->hidden_layer_1[i] += nn->input[j] * nn->weights_hidden_1[j][i];
        nn->hidden_layer_1[i] = relu(nn->hidden_layer_1[i]);
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->hidden_layer_2[i] = nn->bias_hidden_2[i];
        for (int j = 0; j < HIDDEN_LAYERS; j++)
            nn->hidden_layer_2[i] += nn->hidden_layer_1[j] * nn->weights_hidden_2[j][i];
        nn->hidden_layer_2[i] = relu(nn->hidden_layer_2[i]);
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->output[i] = nn->bias_output[i];
        for (int j = 0; j < HIDDEN_LAYERS; j++)
            nn->output[i] += nn->hidden_layer_2[j] * nn->weights_output[j][i];
        nn->output[i] = relu(nn->output[i]);
    }
}

void zero_grad(NeuralNetworkGradients *grad) {
    memset(grad, 0, sizeof(NeuralNetworkGradients));
}

void backward(NeuralNetwork *nn, NeuralNetworkGradients *grad, double target) {
    double d_output[OUTPUT_SIZE] = {0};
    double d_hidden_2[HIDDEN_LAYERS] = {0};
    double d_hidden_1[HIDDEN_LAYERS] = {0};

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        double residual = nn->output[i] - target;
        d_output[i] = 2 * residual * d_relu(nn->output[i]);
        grad->bias_output[i] += d_output[i];
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            d_hidden_2[i] += d_output[j] * nn->weights_output[i][j];
            grad->weights_output[i][j] += d_output[j] * nn->hidden_layer_2[i];
        }
        d_hidden_2[i] *= d_relu(nn->hidden_layer_2[i]);
        grad->bias_hidden_2[i] += d_hidden_2[i];
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            d_hidden_1[i] += d_hidden_2[j] * nn->weights_hidden_2[i][j];
            grad->weights_hidden_2[i][j] += d_hidden_2[j] * nn->hidden_layer_1[i];
        }
        d_hidden_1[i] *= d_relu(nn->hidden_layer_1[i]);
        grad->bias_hidden_1[i] += d_hidden_1[i];
    }

    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            grad->weights_hidden_1[i][j] += d_hidden_1[j] * nn->input[i];
        }
    }
}

void apply_gradients(NeuralNetwork *nn, NeuralNetworkGradients *grad, int batch_size) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_1[i][j] -= LEARNING_RATE * grad->weights_hidden_1[i][j] / batch_size;
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_2[i][j] -= LEARNING_RATE * grad->weights_hidden_2[i][j] / batch_size;
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_output[i][j] -= LEARNING_RATE * grad->weights_output[i][j] / batch_size;
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->bias_hidden_1[i] -= LEARNING_RATE * grad->bias_hidden_1[i] / batch_size;
        nn->bias_hidden_2[i] -= LEARNING_RATE * grad->bias_hidden_2[i] / batch_size;
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_output[i] -= LEARNING_RATE * grad->bias_output[i] / batch_size;
    }
}

void get_training_data(double input[TRAINING_SET_SIZE][INPUT_SIZE], double output[TRAINING_SET_SIZE]) {
    for (int i = 0; i < TRAINING_SET_SIZE; i++) {
        input[i][0] = (double)rand() / RAND_MAX * 10.0;
        input[i][1] = (double)rand() / RAND_MAX * 10.0;
        input[i][2] = (double)rand() / RAND_MAX * 10.0;
        output[i] = 50 * pow(input[i][0], 2) + 50 * input[i][1] + input[i][2];
    }
}

int main() {
    srand(5);

    NeuralNetwork nn_global;
    initialize_network(&nn_global);

    double inputs[TRAINING_SET_SIZE][INPUT_SIZE];
    double targets[TRAINING_SET_SIZE];
    get_training_data(inputs, targets);

    double start_time = get_time();

    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        NeuralNetworkGradients grads_total = {0};
        double mse = 0.0;

        #pragma omp parallel num_threads(THREAD_COUNT)
        {
            NeuralNetwork nn_local = nn_global;
            NeuralNetworkGradients grad_local;
            zero_grad(&grad_local);
            double local_mse = 0.0;

            #pragma omp for
            for (int i = 0; i < TRAINING_SET_SIZE; i++) {
                for (int j = 0; j < INPUT_SIZE; j++)
                    nn_local.input[j] = inputs[i][j];

                forward(&nn_local);
                backward(&nn_local, &grad_local, targets[i]);

                local_mse += pow(targets[i] - nn_local.output[0], 2);
            }

            #pragma omp critical
            {
                for (int i = 0; i < INPUT_SIZE; i++) {
                    for (int j = 0; j < HIDDEN_LAYERS; j++) {
                        grads_total.weights_hidden_1[i][j] += grad_local.weights_hidden_1[i][j];
                    }
                }

                for (int i = 0; i < HIDDEN_LAYERS; i++) {
                    for (int j = 0; j < HIDDEN_LAYERS; j++) {
                        grads_total.weights_hidden_2[i][j] += grad_local.weights_hidden_2[i][j];
                    }
                }

                for (int i = 0; i < HIDDEN_LAYERS; i++) {
                    for (int j = 0; j < OUTPUT_SIZE; j++) {
                        grads_total.weights_output[i][j] += grad_local.weights_output[i][j];
                    }
                }

                for (int i = 0; i < HIDDEN_LAYERS; i++) {
                    grads_total.bias_hidden_1[i] += grad_local.bias_hidden_1[i];
                    grads_total.bias_hidden_2[i] += grad_local.bias_hidden_2[i];
                }

                for (int i = 0; i < OUTPUT_SIZE; i++) {
                    grads_total.bias_output[i] += grad_local.bias_output[i];
                }

                mse += local_mse;
            }
        }

        apply_gradients(&nn_global, &grads_total, TRAINING_SET_SIZE);

        if (epoch % 100 == 0) {
            printf("Epoch %d, MSE: %lf\n", epoch, mse / TRAINING_SET_SIZE);
        }
    }

    double end_time = get_time();
    printf("Training time: %lf seconds\n", end_time - start_time);

    double test_input[3] = {1.0, 2.0, 3.0};
    for (int i = 0; i < INPUT_SIZE; i++) {
        nn_global.input[i] = test_input[i];
    }

    forward(&nn_global);
    printf("Test input: [1.0, 2.0, 3.0] Output: %lf\n", nn_global.output[0]);

    return 0;
}
