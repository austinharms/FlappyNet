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
	Layer(Node* input, uint32_t inputCount, Node* output, uint32_t outputCount) {
		_inputs = input;
		_outputs = output;
		_inputCount = inputCount;
		_outputCount = outputCount;
		_connectionCount = _inputCount * _outputCount;
		_connectionWeights = (float*)malloc(_connectionCount * sizeof(float));
		assert(_connectionWeights != nullptr);
		resetConnectionWeights();
	}

	~Layer() {
		//free(_connections);
		free(_connectionWeights);
	}

	// Calculate the output node values
	void compute(bool useInputThreshold = true) {
		clearOutputNodes();
		for (uint32_t i = 0; i < _inputCount; ++i) {
			uint32_t index = i * _outputCount;
			for (uint32_t o = 0; o < _outputCount; ++o) {
				if (useInputThreshold) {
					if (_inputs[i].Sum >= _inputs[i].Threshold)
						_outputs[o].Sum += _connectionWeights[index + o];
				}
				else
				{
					_outputs[o].Sum += _inputs[i].Sum * _connectionWeights[index + o];
				}
			}
		}
	}
	
	// Randomises Output Node Weights and Internal Connections
	void randomise(uint32_t nodeCount = 2, uint32_t connectionCount = 5) {
		for (uint32_t i = 0; i < nodeCount; ++i)
			_outputs[(uint32_t)(getRandValue01() * (_outputCount - 1))].Threshold += getRandValue();
		for (uint32_t i = 0; i < connectionCount; ++i)
			_connectionWeights[(uint32_t)(getRandValue01() * (_connectionCount - 1))] += getRandValue();
	}

	float* getConnectionWeights() const {
		return _connectionWeights;
	}

	uint32_t getConnectionCount() const {
		return _connectionCount;
	}

	void saveLayer(uint8_t* buffer) {
		memcpy(buffer, _connectionWeights, getSize());
	}

	uint64_t getSize() const {
		return _connectionCount * sizeof(float);
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

	// Set the Sum of all output nodes to 0
	void clearOutputNodes() {
		for (uint32_t i = 0; i < _outputCount; ++i)
			_outputs[i].Sum = 0;
	}

	// Set all ConnectionWeights to 1
	void resetConnectionWeights() {
		for (uint32_t i = 0; i < _connectionCount; ++i)
			_connectionWeights[i] = 1;
	}

	Node* _inputs;
	Node* _outputs;
	//Connection* _connections;
	float* _connectionWeights;
	uint32_t _connectionCount;
	uint32_t _inputCount;
	uint32_t _outputCount;
};