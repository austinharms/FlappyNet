#pragma once
#include <stdint.h>
#include <malloc.h>
#include <random>
#include <casetup.h>

class Layer
{
public:
	Layer(float* inputs, uint32_t inputCount, float* outputs, uint32_t outputCount) {
		_inputs = inputs;
		_outputs = outputs;
		_inputCount = inputCount;
		_outputCount = outputCount;
		_connectionCount = _inputCount * _outputCount;

		_outputThresholds = (float*)malloc((_connectionCount + _outputCount) * sizeof(float));
		_connectionWeights = _outputThresholds + _outputCount;
		if (_outputThresholds == nullptr) {
			std::cout << "Failed to allocate " << ((_connectionCount + _outputCount) * sizeof(float)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		resetOutputThresholds();
		resetConnectionWeights();
	}

	~Layer() {
		free(_outputThresholds);
	}

	// Calculate the output node values
	void compute() {
		for (uint32_t o = 0; o < _outputCount; ++o) {
			for (uint32_t i = 0; i < _inputCount; ++i) {
				_outputs[o] += _inputs[i] * _connectionWeights[i * _outputCount + o];
			}

			_outputs[o] = _outputs[o] >= _outputThresholds[o];
		}
	}
	
	// Randomises Output Node Weights and Internal Connections
	void randomise(uint32_t nodeCount = 2, uint32_t connectionCount = 5) {
		for (uint32_t i = 0; i < nodeCount; ++i)
			_outputThresholds[(uint32_t)(getRandValue01() * (_outputCount - 1))] += getRandValue();
		for (uint32_t i = 0; i < connectionCount; ++i)
			_connectionWeights[(uint32_t)(getRandValue01() * (_connectionCount - 1))] += getRandValue();
	}

	uint8_t* getBytes() const {
		return (uint8_t*)_outputThresholds;
	}

	uint64_t getSize() const {
		return (_connectionCount + _outputCount) * sizeof(float);
	}

private:
	// Returns a random value between -1 and 1
	float getRandValue() const {
		return ((float)rand() / RAND_MAX) * 2 - 1;
	}

	// Returns a random value between 0 and 1
	float getRandValue01() const {
		return ((float)rand() / RAND_MAX) * 1;
	}

	// Set all ConnectionWeights to 1
	void resetConnectionWeights() {
		for (uint32_t i = 0; i < _connectionCount; ++i)
			_connectionWeights[i] = 1;
	}

	void resetOutputThresholds() {
		for (uint32_t i = 0; i < _outputCount; ++i)
			_outputThresholds[i] = 1;
	}

	float* _inputs;
	float* _outputs;
	float* _connectionWeights;
	float* _outputThresholds;
	uint32_t _connectionCount;
	uint32_t _inputCount;
	uint32_t _outputCount;
};