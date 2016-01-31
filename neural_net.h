#ifndef NEURAL_NET
#define NEURAL_NET

class Neuron
{
private:
	int num_weights;
	std::vector<double> weights;
public:
	Neuron(int num_weights);
	~Neuron();
	double process(std::vector<int> inputs)
};

class NeuralNet
{
private:
  std::vector<Neuron> hidden_layer;
  Neuron output_neuron;
public:
  NeuralNet();
  ~NeuralNet();
};

#endif