#include "bootrom.h"

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

  void HardwareDMG0(Bus& bus) {
    bus.Write(kP1, 0xCF);
    bus.Write(kSB, 0x00);
    bus.Write(kSC, 0x7E);
    bus.Write(kDIV, 0x18);
    bus.Write(kTIMA, 0x00);
    bus.Write(kTMA, 0x00);
    bus.Write(kTAC, 0xF8);
    bus.Write(kIF, 0xE1);
    bus.Write(kNR10, 0x80);
    bus.Write(kNR11, 0xBF);
    bus.Write(kNR12, 0xF3);
    bus.Write(kNR13, 0xFF);
    bus.Write(kNR14, 0xBF);
    bus.Write(kNR21, 0x3F);
    bus.Write(kNR22, 0x00);
    bus.Write(kNR23, 0xFF);
    bus.Write(kNR24, 0xBF);
    bus.Write(kNR30, 0x7F);
    bus.Write(kNR31, 0xFF);
    bus.Write(kNR32, 0x9F);
    bus.Write(kNR33, 0xFF);
    bus.Write(kNR34, 0xBF);
    bus.Write(kNR41, 0xFF);
    bus.Write(kNR42, 0x00);
    bus.Write(kNR43, 0x00);
    bus.Write(kNR44, 0xBF);
    bus.Write(kNR50, 0x77);
    bus.Write(kNR51, 0xF3);
    bus.Write(kNR52, 0xF1);
    bus.Write(kLCDC, 0x91);
    bus.Write(kSTAT, 0x81);
    bus.Write(kSCY, 0x00);
    bus.Write(kSCX, 0x00);
    bus.Write(kLY, 0x91);
    bus.Write(kLYC, 0x00);
    bus.Write(kDMA, 0xFF);
    bus.Write(kBGP, 0xFC);
    bus.Write(kOBP0, 0xFF);
    bus.Write(kOBP1, 0xFF);
    bus.Write(kWY, 0x00);
    bus.Write(kWX, 0x00);
    bus.Write(kKEY1, 0xFF);
    bus.Write(kVBK, 0xFF);
    bus.Write(kHDMA1, 0xFF);
    bus.Write(kHDMA2, 0xFF);
    bus.Write(kHDMA3, 0xFF);
    bus.Write(kHDMA4, 0xFF);
    bus.Write(kHDMA5, 0xFF);
    bus.Write(kRP, 0xFF);
    bus.Write(kBCPS, 0xFF);
    bus.Write(kBCPD, 0xFF);
    bus.Write(kOCPS, 0xFF);
    bus.Write(kOCPD, 0xFF);
    bus.Write(kSVBK, 0xFF);
    bus.Write(kIE, 0x00);
  }

  void HardwareDMG(Bus& bus) {
    bus.Write(kP1, 0xCF);
    bus.Write(kSB, 0x00);
    bus.Write(kSC, 0x7E);
    bus.Write(kDIV, 0xAB);
    bus.Write(kTIMA, 0x00);
    bus.Write(kTMA, 0x00);
    bus.Write(kTAC, 0xF8);
    bus.Write(kIF, 0xE1);
    bus.Write(kNR10, 0x80);
    bus.Write(kNR11, 0xBF);
    bus.Write(kNR12, 0xF3);
    bus.Write(kNR13, 0xFF);
    bus.Write(kNR14, 0xBF);
    bus.Write(kNR21, 0x3F);
    bus.Write(kNR22, 0x00);
    bus.Write(kNR23, 0xFF);
    bus.Write(kNR24, 0xBF);
    bus.Write(kNR30, 0x7F);
    bus.Write(kNR31, 0xFF);
    bus.Write(kNR32, 0x9F);
    bus.Write(kNR33, 0xFF);
    bus.Write(kNR34, 0xBF);
    bus.Write(kNR41, 0xFF);
    bus.Write(kNR42, 0x00);
    bus.Write(kNR43, 0x00);
    bus.Write(kNR44, 0xBF);
    bus.Write(kNR50, 0x77);
    bus.Write(kNR51, 0xF3);
    bus.Write(kNR52, 0xF1);
    bus.Write(kLCDC, 0x91);
    bus.Write(kSTAT, 0x85);
    bus.Write(kSCY, 0x00);
    bus.Write(kSCX, 0x00);
    bus.Write(kLY, 0x00);
    bus.Write(kLYC, 0x00);
    bus.Write(kDMA, 0xFF);
    bus.Write(kBGP, 0xFC);
    bus.Write(kOBP0, 0xFF);
    bus.Write(kOBP1, 0xFF);
    bus.Write(kWY, 0x00);
    bus.Write(kWX, 0x00);
    bus.Write(kKEY1, 0xFF);
    bus.Write(kVBK, 0xFF);
    bus.Write(kHDMA1, 0xFF);
    bus.Write(kHDMA2, 0xFF);
    bus.Write(kHDMA3, 0xFF);
    bus.Write(kHDMA4, 0xFF);
    bus.Write(kHDMA5, 0xFF);
    bus.Write(kRP, 0xFF);
    bus.Write(kBCPS, 0xFF);
    bus.Write(kBCPD, 0xFF);
    bus.Write(kOCPS, 0xFF);
    bus.Write(kOCPD, 0xFF);
    bus.Write(kSVBK, 0xFF);
    bus.Write(kIE, 0x00);
  }

  void HardwareSGB(Bus& bus) {
    bus.Write(kP1, 0xCF);
    bus.Write(kSB, 0x00);
    bus.Write(kSC, 0x7E);
    bus.Write(kDIV, 0x0); // TODO
    bus.Write(kTIMA, 0x00);
    bus.Write(kTMA, 0x00);
    bus.Write(kTAC, 0xF8);
    bus.Write(kIF, 0xE1);
    bus.Write(kNR10, 0x80);
    bus.Write(kNR11, 0xBF);
    bus.Write(kNR12, 0xF3);
    bus.Write(kNR13, 0xFF);
    bus.Write(kNR14, 0xBF);
    bus.Write(kNR21, 0x3F);
    bus.Write(kNR22, 0x00);
    bus.Write(kNR23, 0xFF);
    bus.Write(kNR24, 0xBF);
    bus.Write(kNR30, 0x7F);
    bus.Write(kNR31, 0xFF);
    bus.Write(kNR32, 0x9F);
    bus.Write(kNR33, 0xFF);
    bus.Write(kNR34, 0xBF);
    bus.Write(kNR41, 0xFF);
    bus.Write(kNR42, 0x00);
    bus.Write(kNR43, 0x00);
    bus.Write(kNR44, 0xBF);
    bus.Write(kNR50, 0x77);
    bus.Write(kNR51, 0xF3);
    bus.Write(kNR52, 0xF0);
    bus.Write(kLCDC, 0x91);
    bus.Write(kSTAT, 0x0); // TODO
    bus.Write(kSCY, 0x00);
    bus.Write(kSCX, 0x00);
    bus.Write(kLY, 0x5);
    bus.Write(kLYC, 0x00);
    bus.Write(kDMA, 0xFF);
    bus.Write(kBGP, 0xFC);
    bus.Write(kOBP0, 0xFF);
    bus.Write(kOBP1, 0xFF);
    bus.Write(kWY, 0x00);
    bus.Write(kWX, 0x00);
    bus.Write(kKEY1, 0xFF);
    bus.Write(kVBK, 0xFF);
    bus.Write(kHDMA1, 0xFF);
    bus.Write(kHDMA2, 0xFF);
    bus.Write(kHDMA3, 0xFF);
    bus.Write(kHDMA4, 0xFF);
    bus.Write(kHDMA5, 0xFF);
    bus.Write(kRP, 0xFF);
    bus.Write(kBCPS, 0xFF);
    bus.Write(kBCPD, 0xFF);
    bus.Write(kOCPS, 0xFF);
    bus.Write(kOCPD, 0xFF);
    bus.Write(kSVBK, 0xFF);
    bus.Write(kIE, 0x00);
  }

  void HardwareCGB(Bus& bus) {
    bus.Write(kP1, 0xCF);
    bus.Write(kSB, 0x00);
    bus.Write(kSC, 0x7F);
    bus.Write(kDIV, 0x0); // TODO
    bus.Write(kTIMA, 0x00);
    bus.Write(kTMA, 0x00);
    bus.Write(kTAC, 0xF8);
    bus.Write(kIF, 0xE1);
    bus.Write(kNR10, 0x80);
    bus.Write(kNR11, 0xBF);
    bus.Write(kNR12, 0xF3);
    bus.Write(kNR13, 0xFF);
    bus.Write(kNR14, 0xBF);
    bus.Write(kNR21, 0x3F);
    bus.Write(kNR22, 0x00);
    bus.Write(kNR23, 0xFF);
    bus.Write(kNR24, 0xBF);
    bus.Write(kNR30, 0x7F);
    bus.Write(kNR31, 0xFF);
    bus.Write(kNR32, 0x9F);
    bus.Write(kNR33, 0xFF);
    bus.Write(kNR34, 0xBF);
    bus.Write(kNR41, 0xFF);
    bus.Write(kNR42, 0x00);
    bus.Write(kNR43, 0x00);
    bus.Write(kNR44, 0xBF);
    bus.Write(kNR50, 0x77);
    bus.Write(kNR51, 0xF3);
    bus.Write(kNR52, 0xF1);
    bus.Write(kLCDC, 0x91);
    bus.Write(kSTAT, 0x0); // TODO
    bus.Write(kSCY, 0x00);
    bus.Write(kSCX, 0x00);
    bus.Write(kLY, 0x0); // TODO
    bus.Write(kLYC, 0x00);
    bus.Write(kDMA, 0x00);
    bus.Write(kBGP, 0xFC);
    bus.Write(kOBP0, 0xFF);
    bus.Write(kOBP1, 0xFF);
    bus.Write(kWY, 0x00);
    bus.Write(kWX, 0x00);
    bus.Write(kKEY1, 0xFF);
    bus.Write(kVBK, 0xFF);
    bus.Write(kHDMA1, 0xFF);
    bus.Write(kHDMA2, 0xFF);
    bus.Write(kHDMA3, 0xFF);
    bus.Write(kHDMA4, 0xFF);
    bus.Write(kHDMA5, 0xFF);
    bus.Write(kRP, 0xFF);
    bus.Write(kBCPS, 0xFF);
    bus.Write(kBCPD, 0xFF);
    bus.Write(kOCPS, 0xFF);
    bus.Write(kOCPD, 0xFF);
    bus.Write(kSVBK, 0xFF);
    bus.Write(kIE, 0x00);
  }

  void SimulateBootROM(Model model, Registers& registers, Bus& bus) {
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
      HardwareDMG0(bus);
      break;
    case Model::DMG:
    case Model::MGB:
      HardwareDMG(bus);
      break;
    case Model::SGB:
    case Model::SGB2:
      HardwareSGB(bus);
      break;
    case Model::CGB0:
    case Model::CGB:
      HardwareCGB(bus);
      break;
    }
  }
} // namespace gandalf