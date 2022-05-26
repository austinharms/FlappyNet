#pragma once
#include <stdint.h>
#include <malloc.h>
#include <cassert>
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
		for (uint32_t i = 0; i < _layerCount; ++i)
			memcpy(_layers[i].getBytes(), other._layers[i].getBytes(), _layers[i].getSize());
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
		for (uint32_t i = 0; i < _layerCount; ++i) {
			memcpy(_layers[i].getBytes(), bytes + offset, _layers[i].getSize());
			offset += _layers[i].getSize();
		}
		
		assert(offset == getSize());
	}

	~Net() {
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].~Layer();
		free(_layers);
		free(_inputValues);
	}

	void compute() {
		// Clear Old Outputs, NOT Inputs
		memset(_values, 0, (_valueCount - _inputCount) * sizeof(float));
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].compute();
	}

	void randomise() {
		for (uint32_t i = 0; i < _layerCount; ++i)
			_layers[i].randomise();
	}

	void setInput(uint32_t inputIndex, float value) {
		assert(inputIndex < _inputCount);
		_inputValues[inputIndex] = value;
	}

	float getOutput(uint32_t outputIndex) {
		assert(outputIndex < _outputCount);
		return _outputValues[outputIndex];
	}

	uint32_t getNodeCount() const {
		return _valueCount;
	}

	Net& operator=(const Net&) = delete;

	uint64_t getSize() const {
		uint64_t size = sizeof(_inputCount) + sizeof(_outputCount) + sizeof(_layerSize) + sizeof(_layerCount);
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

		for (uint32_t i = 0; i < _layerCount; ++i) {
			memcpy(bytes + offset, _layers[i].getBytes(), _layers[i].getSize());
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
		_valueCount = _inputCount + _outputCount + (_layerSize * layerCount);
		_inputValues = (float*)malloc(_valueCount * sizeof(float));

		if (_inputValues == nullptr) {
			std::cout << "Failed to allocate " << (_valueCount * sizeof(float)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		_values = _inputValues + _inputCount;
		_outputValues = _values + (_layerSize * layerCount);

		_layers = (Layer*)malloc(_layerCount * sizeof(Layer));
		if (_layers == nullptr) {
			std::cout << "Failed to allocate " << (_layerCount * sizeof(Layer)) << "bytes of memory, exiting" << std::endl;
			exit(-1);
		}

		new(_layers) Layer(_inputValues, _inputCount, _values, _layerSize);
		for (uint32_t i = 1; i < _layerCount - 1; ++i)
			new(_layers + i) Layer(_values + ((i - 1) * _layerSize), _layerSize, _values + (i * _layerSize), _layerSize);

		new(_layers + _layerCount - 1) Layer(_values + (_layerSize - 1) * _layerSize, _layerSize, _outputValues, _outputCount);
	}

	float* _values;
	float* _inputValues;
	float* _outputValues;
	Layer* _layers;
	uint64_t _valueCount;
	uint32_t _inputCount;
	uint32_t _outputCount;
	uint32_t _layerCount;
	uint32_t _layerSize;
};