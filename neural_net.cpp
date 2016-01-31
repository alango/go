#include <math.h>
#include <stdlib.h>
#include "neural_net.h"

Neuron::Neuron(int num_weights)
{
	// Initialise weights and bias randomly.
	for (int i=0; i<num_weights+1; i++)
	{
		weights.push_back((double) rand() / RAND_MAX);
	}
}

Neuron::~Neuron() {}

double Neuron::process(std::vector<int> inputs)
{
	double activation = 0;
	for (int i=0; i<num_weights; i++)
	{
		activation += inputs[i] * weights[i];
	}
	// Apply sigmoid function to activation level.
	return 1 / (1 + exp(activation));
}

NeuralNet::NeuralNet():
output_neuron(40)
{
	for (int i=0; i<40; i++)
	{
		Neuron neuron = Neuron(82);
		hidden_layer.push_back(neuron);
	}
}