#include <Windows.h>
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

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}


extern "C" __declspec(dllexport) void __cdecl Run(void)
{
    std::thread t1(display, 0, 5);
    std::thread t2([]() { display(5, 5); });
    t1.join();
    t2.join();
    Sleep(60000);
}