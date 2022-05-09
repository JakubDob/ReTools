#include "threaded_function.h"

namespace utility {
ThreadedFunction::ThreadedFunction(Console &c, std::function<void()> body,
                                   std::string description, bool infiniteLoop,
                                   int msSleepTime)
    : body(body),
      description(description),
      infiniteLoop(infiniteLoop),
      c(c),
      msSleepTime(msSleepTime),
      on([&]() {
          if (this->th.joinable()) {
              th.join();
          }
          if (this->description != "") {
              c.print(this->description, " is called\n");
          }
          if (this->infiniteLoop) {
              this->shouldRun = true;
              auto infLambda = [&]() {
                  while (this->shouldRun) {
                      this->body();
                      std::this_thread::sleep_for(
                          std::chrono::milliseconds(this->msSleepTime));
                  }
              };
              this->th = std::thread(infLambda);
          } else {
              this->th = std::thread(this->body);
          }
      }),
      off([&]() {
          if (this->th.joinable()) {
              this->shouldRun = false;
              this->th.join();
          }
          if (this->description != "") {
              c.print(this->description, " is off\n");
          }
      }) {}
}  // namespace utility