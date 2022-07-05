#ifndef __GANDALF_CPU_H
#define __GANDALF_CPU_H

#include "bus.h"
#include "cpu_registers.h"
#include "timing_handler.h"

namespace gandalf {

  class CPU : public Bus::AddressHandler {
  public:
    CPU(TimingHandler& timing_handler, Bus& bus);
    ~CPU();

    void Tick();

    byte Read(word address) const override;
    void Write(word address, byte value) override;
    std::set<word> GetAddresses() const override;

    Registers& GetRegisters() { return registers_; }

  private:
    void CheckInterrupts();
    void Execute(byte opcode);

    Registers registers_;
    Bus& bus_;
    TimingHandler& timing_handler_;
    byte opcode_;
    bool halt_;
    bool stop_;
    bool halt_bug_;
    bool ei_executed_;
  };

} // namespace gandalf
#endif
