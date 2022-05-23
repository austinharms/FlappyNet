#pragma once
#include <stdint.h>
#include <malloc.h>
#include <cassert>
#include "Node.h"
#include "Layer.h"
#include <random>

class Net
{
public:
	Net(uint32_t inputCount, uint32_t outputCount, uint32_t layerSize, uint32_t layerCount) {
		init(inputCount, outputCount, layerSize, layerCount);
	}

	Net(const Net& other) {
		init(other._inputCount, other._outputCount, other._layerSize, other._layerCount - 1);
		for (uint64_t i = 0; i < _nodeCount; ++i)
			_inputNodes[i].Threshold = other._inputNodes[i].Threshold;
		for (uint32_t i = 0; i < _layerCount; ++i)
			memcpy(_layers[i].getConnectionWeights(), other._layers[i].getConnectionWeights(), _layers[i].getConnectionCount() * sizeof(float));
	}

	Net(const uint8_t* bytes) {
		uint64_t offset = 0;

		uint32_t inputCount;
		memcpy(&inputCount, bytes + offset, sizeof(inputCount));
		offset += sizeof(inputCount);

		uint32_t outputCount;
		memcpy(&outputCount, bytes + offset, sizeof(outputCount));
		offset += sizeof(outputCount);

		uint32_t layerSize;
		memcpy(&layerSize, bytes + offset, sizeof(layerSize));
		offset += sizeof(layerSize);

		uint32_t layerCount;
		memcpy(&layerCount, bytes + offset, sizeof(layerCount));
		offset += sizeof(layerCount);
		
		init(inputCount, outputCount, layerSize, layerCount - 1);
		memcpy(_nodes, bytes + offset, sizeof(Node) * _nodeCount);
		offset += sizeof(Node) * _nodeCount;

		for (uint32_t i = 0; i < _layerCount; ++i) {
			memcpy(_layers[i].getConnectionWeights(), bytes + offset, _layers[i].getConnectionCount() * sizeof(float));
			offset += _layers[i].getConnectionCount() * sizeof(float);
		}
		
		assert(offset == getSize());
	}

	~Net() {
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].~Layer();
		free(_layers);
		delete[] _inputNodes;
	}

	void compute() {
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].compute(i != 0);
	}

	void randomise() {
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].randomise();
	}

	void setInput(uint32_t inputIndex, float value) {
		assert(inputIndex < _inputCount);
		_inputNodes[inputIndex].Sum = value;
	}

	float getOutput(uint32_t outputIndex) {
		assert(outputIndex < _outputCount);
		return _outputNodes[outputIndex].Sum / _outputNodes[outputIndex].Threshold;
	}

	float getNodeSum(uint32_t nodeIndex) const {
		assert(nodeIndex < _nodeCount);
		return _inputNodes[nodeIndex].Sum;
	}

	float getNodeThreshold(uint32_t nodeIndex) const {
		assert(nodeIndex < _nodeCount);
		return _inputNodes[nodeIndex].Threshold;
	}

	uint32_t getNodeCount() const {
		return _nodeCount;
	}

	Net& operator=(const Net&) = delete;

	uint64_t getSize() const {
		uint64_t size = _nodeCount * sizeof(Node);
		size += sizeof(_inputCount) + sizeof(_outputCount) + sizeof(_layerSize) + sizeof(_layerCount);
		for (uint32_t i = 0; i < _layerCount; ++i)
			size += _layers[i].getSize();

		return size;
	}

	uint8_t* getBytes() {
		uint64_t offset = 0;
		uint8_t* bytes = (uint8_t*)malloc(getSize());
		memcpy(bytes + offset, &_inputCount, sizeof(_inputCount));
		offset += sizeof(_inputCount);
		memcpy(bytes + offset, &_outputCount, sizeof(_outputCount));
		offset += sizeof(_outputCount);
		memcpy(bytes + offset, &_layerSize, sizeof(_layerSize));
		offset += sizeof(_layerSize);
		memcpy(bytes + offset, &_layerCount, sizeof(_layerCount));
		offset += sizeof(_layerCount);

		memcpy(bytes + offset, _nodes, sizeof(Node) * _nodeCount);
		offset += sizeof(Node) * _nodeCount;

		for (uint32_t i = 0; i < _layerCount; ++i) {
			_layers[i].saveLayer(bytes + offset);
			offset += _layers[i].getSize();
		}

		assert(offset == getSize());
		return bytes;
	}

private:
	void init(uint32_t inputCount, uint32_t outputCount, uint32_t layerSize, uint32_t layerCount) {
		_inputCount = inputCount;
		_outputCount = outputCount;
		_layerSize = layerSize;
		_layerCount = layerCount + 1;
		_nodeCount = _inputCount + _outputCount + (_layerSize * layerCount);
		_inputNodes = new Node[_nodeCount];
		if (_inputNodes == nullptr) {
			std::cout << "Failed to allocate " << (_nodeCount * sizeof(Node)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		_nodes = _inputNodes + _inputCount;
		_outputNodes = _nodes + (_layerSize * layerCount);
		_layers = (Layer*)malloc(_layerCount * sizeof(Layer));
		if (_layers == nullptr) {
			std::cout << "Failed to allocate " << (_layerCount * sizeof(Layer)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		new(_layers) Layer(_inputNodes, _inputCount, _nodes, _layerSize);
		for (uint32_t i = 1; i < _layerCount - 1; ++i)
			new(_layers + i) Layer(_nodes + ((i - 1) * _layerSize), _layerSize, _nodes + (i * _layerSize), _layerSize);

		new(_layers + _layerCount - 1) Layer(_nodes + (_layerSize - 1) * _layerSize, _layerSize, _outputNodes, _outputCount);
	}

	Node* _nodes;
	Node* _inputNodes;
	Node* _outputNodes;
	Layer* _layers;
	uint64_t _nodeCount;
	uint32_t _inputCount;
	uint32_t _outputCount;
	uint32_t _layerCount;
	uint32_t _layerSize;
};