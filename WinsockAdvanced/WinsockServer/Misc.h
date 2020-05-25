#include <cmath>
#include <string>
#pragma once

static int NumberOfDigits(int number)
{
	return (int)std::floor(std::log10(number) + 1);
}

static std::string IntToString(int number)
{
	return std::to_string(number);
}