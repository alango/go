#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "neural_net.h"

Neuron::Neuron(int num_weights):
  num_weights(num_weights)
{
	// Initialise weights and bias randomly.
	for (int i=0; i<num_weights+1; i++)
	{
		weights.push_back(((double) rand() / RAND_MAX) - 0.5);
	}
}

Neuron::~Neuron() {}

double Neuron::process_inputs(std::vector<int> inputs)
{
  inputs.push_back(1); // Input for the bias.
	double activation = 0;
	for (int i=0; i<num_weights+1; i++)
	{
		activation += inputs[i] * weights[i];
	}
	// Apply sigmoid function to activation level.
  // std::cout << "hidden layer output: " << 1 / (1 + exp(-activation)) << std::endl;
	return 1 / (1 + exp(-activation));
}

OutputNeuron::OutputNeuron(int num_weights):
  Neuron(num_weights)
{}

OutputNeuron::~OutputNeuron() {}

double OutputNeuron::process_inputs(std::vector<double> inputs)
{
  inputs.push_back(1); // Input for the bias.
	double activation = 0;
	for (int i=0; i<num_weights+1; i++)
	{
		activation += inputs[i] * weights[i];
	}
	// return activation;
  // std::cout << "output: " << 1 / (1 + exp(-activation)) << std::endl;
  return 1 / (1 + exp(-activation));
}

NeuralNet::NeuralNet(int input_size, int hidden_layer_size):
  hidden_layer_size(hidden_layer_size),
  output_neuron(hidden_layer_size),
  learning_rate(0.5)
{
	for (int i=0; i<hidden_layer_size; i++)
	{
		Neuron neuron = Neuron(input_size);
		hidden_layer.push_back(neuron);
	}
}

NeuralNet::~NeuralNet() {}

void NeuralNet::update_weights(std::vector<int> inputs, double target)
{
  std::vector<double> hidden_layer_outputs;
  for (std::vector<Neuron>::iterator neuron = hidden_layer.begin();
       neuron != hidden_layer.end();
       neuron++)
  {
    hidden_layer_outputs.push_back(neuron->process_inputs(inputs));
  }
  double output = output_neuron.process_inputs(hidden_layer_outputs);

	double error = output - target;
  std::cout << "error: " << error << std::endl;
  double d_error_d_activation = error * output * (1-output);
  // std::cout << "d_error_d_activation: " << d_error_d_activation <<std::endl;
  std::vector<double> new_output_neuron_weights;
  double delta;
  double new_weight;
  for (int i = 0; i != hidden_layer_size; i++)
  {
    // std::cout << "hidden_layer_outputs: " << hidden_layer_outputs[i] << std::endl;
    delta = d_error_d_activation * hidden_layer_outputs[i];
    // std::cout << "delta: " << delta << std::endl;
    new_weight = output_neuron.weights[i] - (learning_rate*delta);
    new_output_neuron_weights.push_back(new_weight);
  }

  // Iterate through each hidden layer neuron
  for (int i = 0; i != hidden_layer_size; i++)
  {
    delta = d_error_d_activation * output_neuron.weights[i] * hidden_layer_outputs[i]
            * (1-hidden_layer_outputs[i]);
    // std::cout << "hidden_layer delta: " << delta << std::endl;
    // Iterate through each weight in the neuron, including the bias.
    for (int j = 0; j < inputs.size(); j++)
    {
      hidden_layer[i].weights[j] -= learning_rate * delta * inputs[j];
    }
  }

  for (int i = 0; i != hidden_layer_size + 1; i++)
  {
    output_neuron.weights[i] = new_output_neuron_weights[i];
  }
}

void NeuralNet::read_weights_from_file()
{
  std::ifstream weights_file;
  weights_file.open("weights.txt");
  for (std::vector<Neuron>::iterator neuron = hidden_layer.begin();
       neuron != hidden_layer.end();
       neuron++)
  {
    for (std::vector<double>::iterator weight = neuron->weights.begin();
         weight != neuron->weights.end();
         weight++)
    {
      weights_file >> *weight;
    }
  }
  for (std::vector<double>::iterator weight = output_neuron.weights.begin();
       weight != output_neuron.weights.end();
       weight++)
  {
    weights_file >> *weight;
  }
  weights_file.close();
}

void NeuralNet::write_weights_to_file()
{
  std::ofstream weights_file;
  weights_file.open("weights1.txt");
  for (std::vector<Neuron>::iterator neuron = hidden_layer.begin();
       neuron != hidden_layer.end();
       neuron++)
  {
    for (std::vector<double>::iterator weight = neuron->weights.begin();
         weight != neuron->weights.end();
         weight++)
    {
      weights_file << *weight << "\n";
    }
  }
  for (std::vector<double>::iterator weight = output_neuron.weights.begin();
       weight != output_neuron.weights.end();
       weight++)
  {
    weights_file << *weight << "\n";
  }
  weights_file.close();
}