#ifndef NEURAL_NET
#define NEURAL_NET

#include <vector>

class Neuron
{
public:
	int num_weights;
	std::vector<double> weights;
public:
	Neuron(int num_weights);
	~Neuron();
	double process_inputs(std::vector<int> inputs);
};

class OutputNeuron : public Neuron
{
public:
  OutputNeuron(int num_weights);
  ~OutputNeuron();
  double process_inputs(std::vector<double> inputs);
};

class NeuralNet
{
private:
  int hidden_layer_size;
  std::vector<Neuron> hidden_layer;
  OutputNeuron output_neuron;
  double learning_rate;
public:
  NeuralNet(int input_size, int hidden_layer_size);
  ~NeuralNet();
  void update_weights(std::vector<int> inputs, double target);
};

#endif