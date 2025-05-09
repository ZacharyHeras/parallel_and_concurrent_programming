import time

import torch as pt
import torch.nn as nn
import torch.optim as optim

print(pt.__config__.show())

INPUT_SIZE = 3
HIDDEN_LAYERS = 100
OUTPUT_SIZE = 1
TRAINING_SET_SIZE = 1000
EPOCHS = 15000
LEARNING_RATE = 0.000001

neural_network = nn.Sequential(nn.Linear(INPUT_SIZE, HIDDEN_LAYERS),
                               nn.ReLU(),
                               nn.Linear(HIDDEN_LAYERS, HIDDEN_LAYERS),
                               nn.ReLU(),
                               nn.Linear(HIDDEN_LAYERS, OUTPUT_SIZE),
                               nn.ReLU())

optimizer = optim.SGD(neural_network.parameters(), lr=LEARNING_RATE)
loss_fn = nn.MSELoss()

inputs = pt.rand(1000, 3) * 10
targets = 50 * inputs[:, 0:1] ** 2 + 50 * inputs[:, 1:2] + inputs[:, 2:3]

starttime = time.time()

for epoch in range(EPOCHS):
    mse = 0

    outputs = neural_network(inputs)
    loss = loss_fn(outputs, targets)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    if epoch % 100 == 0:
        print(f"Epoch {epoch}, Error: {loss.item()}")

endtime = time.time()
print(f"Training time: {endtime - starttime} seconds")

test_input = pt.tensor([[1.0, 2.0, 3.0]])

output = neural_network(test_input)

print(f"Test input: [1.0, 2.0, 3.0] Output: {output.item()}")
