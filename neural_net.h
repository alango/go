#ifndef NEURAL_NET
#define NEURAL_NET

#include <vector>

class Neuron
{
protected:
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
  std::vector<Neuron> hidden_layer;
  OutputNeuron output_neuron;
public:
  NeuralNet();
  ~NeuralNet();
  double process_inputs(std::vector<int> inputs);
};

#endif