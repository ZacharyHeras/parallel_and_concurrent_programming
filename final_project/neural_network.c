#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

    return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

double relu(double x) {
    return x > 0 ? x : 0.0;
}

double d_relu(double x) {
    return x > 0 ? 1.0 : 0.0;
}

double get_random_weight() {
    return 2.0 * ((double) rand() / RAND_MAX) - 1.0;
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
    
    double d_hidden_2[OUTPUT_SIZE];
    double d_hidden_1[HIDDEN_LAYERS];
    double d_input[HIDDEN_LAYERS];
} NeuralNetwork;

void initialize_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_1[i][j] = get_random_weight();
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_2[i][j] = get_random_weight();
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_output[i][j] = get_random_weight();
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->bias_hidden_1[i] = get_random_weight();
        nn->bias_hidden_2[i] = get_random_weight();
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_output[i] = get_random_weight();
    }
}

void forward(NeuralNetwork *nn) {
    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->hidden_layer_1[i] = 0.0;
        for (int j = 0; j < INPUT_SIZE; j++) {
            nn->hidden_layer_1[i] += nn->input[j] * nn->weights_hidden_1[j][i];
        }
        nn->hidden_layer_1[i] += nn->bias_hidden_1[i];
        nn->hidden_layer_1[i] = relu(nn->hidden_layer_1[i]);
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->hidden_layer_2[i] = 0.0;
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->hidden_layer_2[i] += nn->hidden_layer_1[j] * nn->weights_hidden_2[j][i];
        }
        nn->hidden_layer_2[i] += nn->bias_hidden_2[i];
        nn->hidden_layer_2[i] = relu(nn->hidden_layer_2[i]);
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->output[i] = 0.0;
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->output[i] += nn->hidden_layer_2[j] * nn->weights_output[j][i];
        }
        nn->output[i] += nn->bias_output[i];
        nn->output[i] = relu(nn->output[i]);
    }
}

void backward(NeuralNetwork *nn, double target) {
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        double d_output = 2 * (nn->output[i] - target);
        nn->d_hidden_2[i] = d_output * d_relu(nn->output[i]);
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->d_hidden_1[i] = 0.0;
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->d_hidden_1[i] += nn->d_hidden_2[j] * nn->weights_output[i][j];
        }
        nn->d_hidden_1[i] *= d_relu(nn->hidden_layer_2[i]);
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->d_input[i] = 0.0;
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->d_input[i] += nn->d_hidden_1[j] * nn->weights_hidden_2[i][j];
        }
        nn->d_input[i] *= d_relu(nn->hidden_layer_1[i]);
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_output[i][j] -= LEARNING_RATE * nn->d_hidden_2[j] * nn->hidden_layer_2[i];
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_2[i][j] -= LEARNING_RATE * nn->d_hidden_1[j] * nn->hidden_layer_1[i];
        }
    }

    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_LAYERS; j++) {
            nn->weights_hidden_1[i][j] -= LEARNING_RATE * nn->d_input[j] * nn->input[i];
        }
    }

    for (int i = 0; i < HIDDEN_LAYERS; i++) {
        nn->bias_hidden_1[i] -= LEARNING_RATE * nn->d_input[i];
        nn->bias_hidden_2[i] -= LEARNING_RATE * nn->d_hidden_1[i];
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->bias_output[i] -= LEARNING_RATE * nn->d_hidden_2[i];
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

    NeuralNetwork nn;
    initialize_network(&nn);
    
    double inputs[TRAINING_SET_SIZE][INPUT_SIZE];
    double targets[TRAINING_SET_SIZE];
    get_training_data(inputs, targets);

    double starttime = get_time();

    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        double mse = 0.0;

        for (int i = 0; i < TRAINING_SET_SIZE; i++) {
            for (int j = 0; j < INPUT_SIZE; j++) {
                nn.input[j] = inputs[i][j];
            }

            forward(&nn);
            backward(&nn, targets[i]);

            mse += pow(targets[i] - nn.output[0], 2);
        }

        if (epoch % 100 == 0) {
            printf("Epoch %d, Error: %lf\n", epoch, mse / TRAINING_SET_SIZE);
        }
    }

    double endtime = get_time();

    printf("Time taken = %lf seconds\n", endtime-starttime);

    double test_input[3] = {1.0, 2.0, 3.0};
    for (int i = 0; i < INPUT_SIZE; i++) {
        nn.input[i] = test_input[i];
    }

    forward(&nn);

    printf("Test input: [1.0, 2.0, 3.0] Output: %lf\n", nn.output[0]);

    return 0;
}
