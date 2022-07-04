#ifndef __GANDALF_GAMEBOY_H
#define __GANDALF_GAMEBOY_H

#include <memory>

#include "bus.h"
#include "timer.h"

namespace gandalf {
  class Gameboy {
  public:
    Gameboy();
    ~Gameboy();

  private:
    std::unique_ptr<Bus> bus_;

    Timer timer_;
  };
} // namespace gandalf
#endif
