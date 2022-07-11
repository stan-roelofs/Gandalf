#ifndef __GANDALF_CPU_H
#define __GANDALF_CPU_H

#include "bus.h"
#include "cpu_registers.h"
#include "io.h"

namespace gandalf {

  class CPU : public Bus::AddressHandler {
  public:
    CPU(IO& io, Bus& bus);
    ~CPU();

    void Tick();

    byte Read(word address) const override;
    void Write(word address, byte value) override;
    std::set<word> GetAddresses() const override;

    Registers& GetRegisters() { return registers_; }

  private:
    void CheckInterrupts();
    void Execute(byte opcode);
    void InterruptServiceRoutine();

    Registers registers_;
    Bus& bus_;
    IO& io_;
    byte opcode_;
    bool halt_;
    bool stop_;
    bool halt_bug_;
    bool ei_pending_;
  };

} // namespace gandalf
#endif
