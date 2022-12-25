#include "bootrom.h"

#include <gandalf/constants.h>

namespace gandalf {

  void RegistersDMG0(Registers& registers) {
    registers.a() = 0x01;
    registers.f() = 0x00;
    registers.b() = 0xFF;
    registers.c() = 0x13;
    registers.d() = 0x00;
    registers.e() = 0xC1;
    registers.h() = 0x84;
    registers.l() = 0x03;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void RegistersDMG(Registers& registers) {
    registers.a() = 0x01;
    registers.f() = 0x80;
    registers.b() = 0x00;
    registers.c() = 0x13;
    registers.d() = 0x00;
    registers.e() = 0xD8;
    registers.h() = 0x01;
    registers.l() = 0x4D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void RegistersMGB(Registers& registers) {
    registers.a() = 0xFF;
    registers.f() = 0x80;
    registers.b() = 0x00;
    registers.c() = 0x13;
    registers.d() = 0x00;
    registers.e() = 0xD8;
    registers.h() = 0x01;
    registers.l() = 0x4D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void RegistersSGB(Registers& registers) {
    registers.a() = 0x01;
    registers.f() = 0x00;
    registers.b() = 0x00;
    registers.c() = 0x14;
    registers.d() = 0x00;
    registers.e() = 0x00;
    registers.h() = 0xC0;
    registers.l() = 0x60;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void RegistersSGB2(Registers& registers) {
    registers.a() = 0xFF;
    registers.f() = 0x00;
    registers.b() = 0x00;
    registers.c() = 0x14;
    registers.d() = 0x00;
    registers.e() = 0x00;
    registers.h() = 0xC0;
    registers.l() = 0x60;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void RegistersCGB(Registers& registers) {
    registers.a() = 0x11;
    registers.f() = 0x80;
    registers.b() = 0x00;
    registers.c() = 0x00;
    registers.d() = 0xFF;
    registers.e() = 0x56;
    registers.h() = 0x00;
    registers.l() = 0x0D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
  }

  void HardwareDMG0(Memory& memory) {
    memory.Write(kP1, 0xCF);
    memory.Write(kSB, 0x00);
    memory.Write(kSC, 0x7E);
    memory.Write(kDIV, 0x18);
    memory.Write(kTIMA, 0x00);
    memory.Write(kTMA, 0x00);
    memory.Write(kTAC, 0xF8);
    memory.Write(kIF, 0xE1);
    memory.Write(kNR10, 0x80);
    memory.Write(kNR11, 0xBF);
    memory.Write(kNR12, 0xF3);
    memory.Write(kNR13, 0xFF);
    memory.Write(kNR14, 0xBF);
    memory.Write(kNR21, 0x3F);
    memory.Write(kNR22, 0x00);
    memory.Write(kNR23, 0xFF);
    memory.Write(kNR24, 0xBF);
    memory.Write(kNR30, 0x7F);
    memory.Write(kNR31, 0xFF);
    memory.Write(kNR32, 0x9F);
    memory.Write(kNR33, 0xFF);
    memory.Write(kNR34, 0xBF);
    memory.Write(kNR41, 0xFF);
    memory.Write(kNR42, 0x00);
    memory.Write(kNR43, 0x00);
    memory.Write(kNR44, 0xBF);
    memory.Write(kNR50, 0x77);
    memory.Write(kNR51, 0xF3);
    memory.Write(kNR52, 0xF1);
    memory.Write(kLCDC, 0x91);
    memory.Write(kSTAT, 0x81);
    memory.Write(kSCY, 0x00);
    memory.Write(kSCX, 0x00);
    memory.Write(kLY, 0x91);
    memory.Write(kLYC, 0x00);
    memory.Write(kDMA, 0xFF);
    memory.Write(kBGP, 0xFC);
    memory.Write(kOBP0, 0xFF);
    memory.Write(kOBP1, 0xFF);
    memory.Write(kWY, 0x00);
    memory.Write(kWX, 0x00);
    memory.Write(kKEY1, 0xFF);
    memory.Write(kVBK, 0xFF);
    memory.Write(kHDMA1, 0xFF);
    memory.Write(kHDMA2, 0xFF);
    memory.Write(kHDMA3, 0xFF);
    memory.Write(kHDMA4, 0xFF);
    memory.Write(kHDMA5, 0xFF);
    memory.Write(kRP, 0xFF);
    memory.Write(kBCPS, 0xFF);
    memory.Write(kBCPD, 0xFF);
    memory.Write(kOCPS, 0xFF);
    memory.Write(kOCPD, 0xFF);
    memory.Write(kSVBK, 0xFF);
    memory.Write(kIE, 0x00);
  }

  void HardwareDMG(Memory& memory) {
    memory.Write(kP1, 0xCF);
    memory.Write(kSB, 0x00);
    memory.Write(kSC, 0x7E);
    memory.Write(kDIV, 0xAB);
    memory.Write(kTIMA, 0x00);
    memory.Write(kTMA, 0x00);
    memory.Write(kTAC, 0xF8);
    memory.Write(kIF, 0xE1);
    memory.Write(kNR10, 0x80);
    memory.Write(kNR11, 0xBF);
    memory.Write(kNR12, 0xF3);
    memory.Write(kNR13, 0xFF);
    memory.Write(kNR14, 0xBF);
    memory.Write(kNR21, 0x3F);
    memory.Write(kNR22, 0x00);
    memory.Write(kNR23, 0xFF);
    memory.Write(kNR24, 0xBF);
    memory.Write(kNR30, 0x7F);
    memory.Write(kNR31, 0xFF);
    memory.Write(kNR32, 0x9F);
    memory.Write(kNR33, 0xFF);
    memory.Write(kNR34, 0xBF);
    memory.Write(kNR41, 0xFF);
    memory.Write(kNR42, 0x00);
    memory.Write(kNR43, 0x00);
    memory.Write(kNR44, 0xBF);
    memory.Write(kNR50, 0x77);
    memory.Write(kNR51, 0xF3);
    memory.Write(kNR52, 0xF1);
    memory.Write(kLCDC, 0x91);
    memory.Write(kSTAT, 0x85);
    memory.Write(kSCY, 0x00);
    memory.Write(kSCX, 0x00);
    memory.Write(kLY, 0x00);
    memory.Write(kLYC, 0x00);
    memory.Write(kDMA, 0xFF);
    memory.Write(kBGP, 0xFC);
    memory.Write(kOBP0, 0xFF);
    memory.Write(kOBP1, 0xFF);
    memory.Write(kWY, 0x00);
    memory.Write(kWX, 0x00);
    memory.Write(kKEY1, 0xFF);
    memory.Write(kVBK, 0xFF);
    memory.Write(kHDMA1, 0xFF);
    memory.Write(kHDMA2, 0xFF);
    memory.Write(kHDMA3, 0xFF);
    memory.Write(kHDMA4, 0xFF);
    memory.Write(kHDMA5, 0xFF);
    memory.Write(kRP, 0xFF);
    memory.Write(kBCPS, 0xFF);
    memory.Write(kBCPD, 0xFF);
    memory.Write(kOCPS, 0xFF);
    memory.Write(kOCPD, 0xFF);
    memory.Write(kSVBK, 0xFF);
    memory.Write(kIE, 0x00);
  }

  void HardwareSGB(Memory& memory) {
    memory.Write(kP1, 0xCF);
    memory.Write(kSB, 0x00);
    memory.Write(kSC, 0x7E);
    memory.Write(kDIV, 0x0); // TODO
    memory.Write(kTIMA, 0x00);
    memory.Write(kTMA, 0x00);
    memory.Write(kTAC, 0xF8);
    memory.Write(kIF, 0xE1);
    memory.Write(kNR10, 0x80);
    memory.Write(kNR11, 0xBF);
    memory.Write(kNR12, 0xF3);
    memory.Write(kNR13, 0xFF);
    memory.Write(kNR14, 0xBF);
    memory.Write(kNR21, 0x3F);
    memory.Write(kNR22, 0x00);
    memory.Write(kNR23, 0xFF);
    memory.Write(kNR24, 0xBF);
    memory.Write(kNR30, 0x7F);
    memory.Write(kNR31, 0xFF);
    memory.Write(kNR32, 0x9F);
    memory.Write(kNR33, 0xFF);
    memory.Write(kNR34, 0xBF);
    memory.Write(kNR41, 0xFF);
    memory.Write(kNR42, 0x00);
    memory.Write(kNR43, 0x00);
    memory.Write(kNR44, 0xBF);
    memory.Write(kNR50, 0x77);
    memory.Write(kNR51, 0xF3);
    memory.Write(kNR52, 0xF0);
    memory.Write(kLCDC, 0x91);
    memory.Write(kSTAT, 0x0); // TODO
    memory.Write(kSCY, 0x00);
    memory.Write(kSCX, 0x00);
    memory.Write(kLY, 0x5);
    memory.Write(kLYC, 0x00);
    memory.Write(kDMA, 0xFF);
    memory.Write(kBGP, 0xFC);
    memory.Write(kOBP0, 0xFF);
    memory.Write(kOBP1, 0xFF);
    memory.Write(kWY, 0x00);
    memory.Write(kWX, 0x00);
    memory.Write(kKEY1, 0xFF);
    memory.Write(kVBK, 0xFF);
    memory.Write(kHDMA1, 0xFF);
    memory.Write(kHDMA2, 0xFF);
    memory.Write(kHDMA3, 0xFF);
    memory.Write(kHDMA4, 0xFF);
    memory.Write(kHDMA5, 0xFF);
    memory.Write(kRP, 0xFF);
    memory.Write(kBCPS, 0xFF);
    memory.Write(kBCPD, 0xFF);
    memory.Write(kOCPS, 0xFF);
    memory.Write(kOCPD, 0xFF);
    memory.Write(kSVBK, 0xFF);
    memory.Write(kIE, 0x00);
  }

  void HardwareCGB(Memory& memory) {
    memory.Write(kP1, 0xCF);
    memory.Write(kSB, 0x00);
    memory.Write(kSC, 0x7F);
    memory.Write(kDIV, 0x0); // TODO
    memory.Write(kTIMA, 0x00);
    memory.Write(kTMA, 0x00);
    memory.Write(kTAC, 0xF8);
    memory.Write(kIF, 0xE1);
    memory.Write(kNR10, 0x80);
    memory.Write(kNR11, 0xBF);
    memory.Write(kNR12, 0xF3);
    memory.Write(kNR13, 0xFF);
    memory.Write(kNR14, 0xBF);
    memory.Write(kNR21, 0x3F);
    memory.Write(kNR22, 0x00);
    memory.Write(kNR23, 0xFF);
    memory.Write(kNR24, 0xBF);
    memory.Write(kNR30, 0x7F);
    memory.Write(kNR31, 0xFF);
    memory.Write(kNR32, 0x9F);
    memory.Write(kNR33, 0xFF);
    memory.Write(kNR34, 0xBF);
    memory.Write(kNR41, 0xFF);
    memory.Write(kNR42, 0x00);
    memory.Write(kNR43, 0x00);
    memory.Write(kNR44, 0xBF);
    memory.Write(kNR50, 0x77);
    memory.Write(kNR51, 0xF3);
    memory.Write(kNR52, 0xF1);
    memory.Write(kLCDC, 0x91);
    memory.Write(kSTAT, 0x0); // TODO
    memory.Write(kSCY, 0x00);
    memory.Write(kSCX, 0x00);
    memory.Write(kLY, 0x0); // TODO
    memory.Write(kLYC, 0x00);
    memory.Write(kDMA, 0x00);
    memory.Write(kBGP, 0xFC);
    memory.Write(kOBP0, 0xFF);
    memory.Write(kOBP1, 0xFF);
    memory.Write(kWY, 0x00);
    memory.Write(kWX, 0x00);
    memory.Write(kKEY1, 0xFF);
    memory.Write(kVBK, 0xFF);
    memory.Write(kHDMA1, 0xFF);
    memory.Write(kHDMA2, 0xFF);
    memory.Write(kHDMA3, 0xFF);
    memory.Write(kHDMA4, 0xFF);
    memory.Write(kHDMA5, 0xFF);
    memory.Write(kRP, 0xFF);
    memory.Write(kBCPS, 0xFF);
    memory.Write(kBCPD, 0xFF);
    memory.Write(kOCPS, 0xFF);
    memory.Write(kOCPD, 0xFF);
    memory.Write(kSVBK, 0xFF);
    memory.Write(kIE, 0x00);
  }

  void SimulateBootROM(Model model, Registers& registers, Memory& memory) {
    // CPU registers
    switch (model) {
    case Model::DMG0:
      RegistersDMG0(registers);
      break;
    case Model::DMG:
      RegistersDMG(registers);
      break;
    case Model::MGB:
      RegistersMGB(registers);
      break;
    case Model::SGB:
      RegistersSGB(registers);
      break;
    case Model::SGB2:
      RegistersSGB2(registers);
      break;
    case Model::CGB0:
    case Model::CGB:
      RegistersCGB(registers);
      break;
    }

    // Hardware registers
    switch (model) {
    case Model::DMG0:
      HardwareDMG0(memory);
      break;
    case Model::DMG:
    case Model::MGB:
      HardwareDMG(memory);
      break;
    case Model::SGB:
    case Model::SGB2:
      HardwareSGB(memory);
      break;
    case Model::CGB0:
    case Model::CGB:
      HardwareCGB(memory);
      break;
    }
  }
} // namespace gandalf