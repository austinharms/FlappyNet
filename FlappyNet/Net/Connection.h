#pragma once
#include <stdint.h>

struct Connection
{
public:
	void* InputNode;
	void* OutputNode;
	float Weight;
};