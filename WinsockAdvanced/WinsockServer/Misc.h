#include <cmath>
#include <string>
#pragma once

static int NumberOfDigits(int number)
{
	return std::floor(std::log10(number) + 1);
}

static const char* IntToChar(int number)
{
	int size = NumberOfDigits(number);
	char *buffer = (char*)calloc(size, sizeof(char));
	size++;
	_itoa_s(number, buffer, size, 10);
	//std::string conversion = std::to_string(number);
	//const std::string::size_type size = conversion.size();
	//char* buffer = new char[size + 1];
	//memcpy(buffer, conversion.c_str(), size + 1);
	return buffer;
}