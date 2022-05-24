#pragma once
#include <stdint.h>
#include <malloc.h>
#include <random>
#include <casetup.h>
#include "Node.h"
//#include "Connection.h"

class Layer
{
public:
	Layer(float* inputs, uint32_t inputCount, float* outputs, uint32_t outputCount) {
		_inputs = inputs;
		_outputs = outputs;
		_inputCount = inputCount;
		_outputCount = outputCount;
		_connectionCount = _inputCount * _outputCount;

		_inputThresholds = (float*)malloc((_connectionCount + _inputCount) * sizeof(float));
		_connectionWeights = _inputThresholds + _outputCount;
		if (_inputThresholds == nullptr) {
			std::cout << "Failed to allocate " << ((_connectionCount + _outputCount) * sizeof(float)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		for (uint32_t i = 0; i < _inputCount; ++i)
			_inputThresholds[i] = 1;

		resetConnectionWeights();
	}

	~Layer() {
		free(_inputThresholds);
	}

	// Calculate the output node values
	void compute(bool useInputThreshold = true) {
		for (uint32_t i = 0; i < _inputCount; ++i) {
			uint32_t index = i * _outputCount;
			for (uint32_t o = 0; o < _outputCount; ++o) {
				if (useInputThreshold) {
					if (_inputs[i] >= _inputThresholds[i])
						_outputs[o] += _connectionWeights[index + o];
				}
				else
				{
					_outputs[o] += _inputs[i] * _connectionWeights[index + o];
				}
			}
		}
	}
	
	// Randomises Output Node Weights and Internal Connections
	void randomise(uint32_t nodeCount = 2, uint32_t connectionCount = 5) {
		for (uint32_t i = 0; i < nodeCount; ++i)
			_inputThresholds[(uint32_t)(getRandValue01() * (_inputCount - 1))] += getRandValue();
		for (uint32_t i = 0; i < connectionCount; ++i)
			_connectionWeights[(uint32_t)(getRandValue01() * (_connectionCount - 1))] += getRandValue();
	}

	uint8_t* getBytes() const {
		return (uint8_t*)_inputThresholds;
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

	float* _inputs;
	float* _outputs;
	//Connection* _connections;
	float* _connectionWeights;
	float* _inputThresholds;
	uint32_t _connectionCount;
	uint32_t _inputCount;
	uint32_t _outputCount;
};