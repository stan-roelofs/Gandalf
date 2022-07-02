#ifndef __GANDALF_TIMING_HANDLER_H
#define __GANDALF_TIMING_HANDLER_H

namespace gandalf {

  class TimingHandler {
  public:
    virtual void Advance(byte cycles) = 0;
  };

} // namespace gandalf

#endif
