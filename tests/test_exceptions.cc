

#include <thread>
#include <mutex>
#include <iostream>
void test_cpp_exception()
{
	try
	{
		throw "My c++ exception";
	}
	catch (...)
	{
		std::cout << "Exception c++" << std::endl;
	}
}

void test_structured_exception()
{
	__try
	{
		// Test write access violation
		auto myArray = new uint8_t[5];
		myArray = nullptr;

		myArray[500] = 5;
	}
	__finally
	{
		std::cout << "Exception write acess" << std::endl;
	}
}



int main() {

	test_cpp_exception();
	test_structured_exception();
	return 0;
}