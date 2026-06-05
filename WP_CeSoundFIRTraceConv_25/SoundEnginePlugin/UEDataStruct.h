#pragma once
#include <vector>

struct UEDataStruct
{
public:
	std::vector<std::vector<float>> impulses;

	std::vector<float> T60;

	signed int version;
};