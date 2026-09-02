#include <iostream>
#include <SPSCQueue.hpp>

int main(int argc, char* argv[])
{
    sya::SPSCQueue<int, 10> q;
	std::cout << "Hello, world and ";
	for (uint8_t i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << " ";
	}
	std::cout << "\n";
	return 0;
}