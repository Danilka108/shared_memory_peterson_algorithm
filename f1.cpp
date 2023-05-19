#include "mutex.cpp"
#include <chrono>
#include <thread>

int main() {
  Mutex mutex(0, "f1.cpp", sizeof(int) * 3);

  if (mutex.is_initialized()) {
    auto guard = mutex.lock();
    (*guard)[0] += 1;

    // for (int i = 0; i < 1000000000; i++) {
    //   for (int i = 0; i < 1000000000; i++) {
    //     for (int i = 0; i < 1000000000; i++) {
    //       for (int i = 0; i < 1000000000; i++) {
    //       }
    //     }
    //   }
    // }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    (*guard)[2] += 1;
  } else {
    auto guard = mutex.lock();
    (*guard)[0] = 1;
    (*guard)[1] = 0;
    (*guard)[2] = 1;
  }

  {
    auto guard = mutex.lock();
    printf(
        "Program 1 was spawn %d times, program 2 was spawn %d times, total %d",
        (*guard)[0], (*guard)[1], (*guard)[2]);
  }

  return 0;
}
