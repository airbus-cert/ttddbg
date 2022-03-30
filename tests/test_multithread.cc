

#include <thread>
#include <mutex>
#include <iostream>
void display(int start, int nb)
{
	auto heap = (char*)malloc(0x10);
	memset(heap, 0, 0x10);
	heap[5] = 0x77;
	// put a breakpoint here in the trace!
	std::cout << "hello from thread " << std::endl;
	for (int i = start; i < start + nb; ++i)
		std::cout << i << ",";
}
int main() {
	std::thread t1(display, 0, 5);
	std::thread t2([]() { display(5, 5); });
	t1.join();
	t2.join();
	return 0;
}