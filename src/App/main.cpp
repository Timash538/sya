#include <LogMacros.hpp>

#include <thread>

int main()
{
    LOGI("sya Engine started");

    std::thread renderThread(
        []()
        {
            for (int i = 0; i < 5; ++i)
            {
                LOGD("Frame rendered", i);
            }
        });

    std::thread physicsThread(
        []()
        {
            for (int i = 0; i < 5; ++i)
            {
                LOGI("Physics updated", i);
            }
        });

    renderThread.join();
    physicsThread.join();

    LOGI("sya Engine shutdown");

    return 0;
}