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

double Neuron::process_inputs(std::vector<int> inputs)
{
	double activation = 0;
	for (int i=0; i<num_weights; i++)
	{
		activation += inputs[i] * weights[i];
	}
	// Apply sigmoid function to activation level.
	return 1 / (1 + exp(activation));
}

OutputNeuron::OutputNeuron(int num_weights):
  Neuron(num_weights)
{};

OutputNeuron::~OutputNeuron() {}

double OutputNeuron::process_inputs(std::vector<double> inputs)
{
	double activation = 0;
	for (int i=0; i<num_weights; i++)
	{
		activation += inputs[i] * weights[i];
	}
	return activation;
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

NeuralNet::~NeuralNet() {}

double NeuralNet::process_inputs(std::vector<int> inputs)
{
	std::vector<double> hidden_layer_outputs;
	for (std::vector<Neuron>::iterator neuron = hidden_layer.begin();
		   neuron != hidden_layer.end();
		   neuron++)
	{
		hidden_layer_outputs.push_back(neuron->process_inputs(inputs));
	}

	return output_neuron.process_inputs(hidden_layer_outputs);
}