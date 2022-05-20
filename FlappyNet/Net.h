#pragma once
#include <stdint.h>
#include <malloc.h>
#include "Node.h"
#include "Connection.h"
#include <random>

class Net
{
public:
	Net(uint8_t inputs, uint8_t width, uint8_t length, uint8_t outputs) {
		_inputCount = inputs;
		_internalWidth = width;
		_internalLength = length;
		_outputCount = outputs;
		_totalNodeCount = inputs + (width * length) + outputs;
		_nodes = (Node*)malloc(_totalNodeCount * sizeof(Node));
		_connections = (Connection*)malloc((uint64_t)width * (length + 1) * 2 * sizeof(Connection));
		CreateLayer(_nodes, inputs, _nodes + inputs, width, _connections, width * 2);
		for (uint8_t i = 0; i < length; ++i) {
			if (i == length - 1) {
				CreateLayer(_nodes + inputs + width * i, width, _nodes + inputs + width * (i + 1), outputs, _connections + width * 2, width * 2);
			}
			else
			{
				CreateLayer(_nodes + inputs + width * i, width, _nodes + inputs + width * (i + 1), width, _connections + width * 2, width * 2);
			}
		}
	}

	void ClearNodes()
	{
		for (uint32_t i = _inputCount; i < _totalNodeCount; ++i) {
			_nodes[i].Sum = 0;
		}
	}

	void CalculateNodes() {

	}

	void SetInput(uint8_t inputIndex, float value) {
		_nodes[inputIndex].Sum = value;
	}

	void GetOutput(float* outSum, float* outThres = nullptr) {
		ClearNodes();
		if (outThres == nullptr) {
			for (uint8_t i = 0; i < _outputCount; ++i) {
				if (_nodes[_totalNodeCount - _outputCount + i].Sum > _nodes[_totalNodeCount - _outputCount + i].Threshold)
				{
					outSum[i] = 1;
				}
				else
				{
					outSum[i] = 0;
				}
			}
		}
		else
		{
			for (uint8_t i = 0; i < _outputCount; ++i) {
				outSum[i] = _nodes[_totalNodeCount - _outputCount + i].Sum;
				outThres[i] = _nodes[_totalNodeCount - _outputCount + i].Threshold;
			}
		}
	}

	~Net() {
		free(_nodes);
		free(_connections);
	}

	void CreateLayer(Node* inputNodes, uint8_t inputNodeCount, Node* outputNodes, uint8_t outputNodeCount, Connection* connections, uint16_t connectionCount) {
		for (uint16_t i = 0; i < connectionCount; ++i) {
			connections[i].Weight = ((float)rand()) / RAND_MAX * 100;
			uint32_t randIndex = rand() % (inputNodeCount + 1);
			if (randIndex == inputNodeCount) {
				connections[i].InputNode = nullptr;
			}
			else
			{
				connections[i].InputNode = inputNodes + randIndex;
			}

			randIndex = rand() % (outputNodeCount + 1);
			if (randIndex == outputNodeCount) {
				connections[i].OutputNode = nullptr;
			}
			else
			{
				connections[i].OutputNode = outputNodes + randIndex;
			}
		}
	}

private:
	uint8_t _inputCount;
	uint8_t _internalWidth;
	uint8_t _internalLength;
	uint8_t _outputCount;
	uint32_t _totalNodeCount;
	uint32_t _totalConnectionCount;
	Node* _nodes;
	Connection* _connections;
};