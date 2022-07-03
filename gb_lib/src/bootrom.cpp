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
    bus.Write(P1, 0xCF);
    bus.Write(SB, 0x00);
    bus.Write(SC, 0x7E);
    bus.Write(DIV, 0x18);
    bus.Write(TIMA, 0x00);
    bus.Write(TMA, 0x00);
    bus.Write(TAC, 0xF8);
    bus.Write(IF, 0xE1);
    bus.Write(NR10, 0x80);
    bus.Write(NR11, 0xBF);
    bus.Write(NR12, 0xF3);
    bus.Write(NR13, 0xFF);
    bus.Write(NR14, 0xBF);
    bus.Write(NR21, 0x3F);
    bus.Write(NR22, 0x00);
    bus.Write(NR23, 0xFF);
    bus.Write(NR24, 0xBF);
    bus.Write(NR30, 0x7F);
    bus.Write(NR31, 0xFF);
    bus.Write(NR32, 0x9F);
    bus.Write(NR33, 0xFF);
    bus.Write(NR34, 0xBF);
    bus.Write(NR41, 0xFF);
    bus.Write(NR42, 0x00);
    bus.Write(NR43, 0x00);
    bus.Write(NR44, 0xBF);
    bus.Write(NR50, 0x77);
    bus.Write(NR51, 0xF3);
    bus.Write(NR52, 0xF1);
    bus.Write(LCDC, 0x91);
    bus.Write(STAT, 0x81);
    bus.Write(SCY, 0x00);
    bus.Write(SCX, 0x00);
    bus.Write(LY, 0x91);
    bus.Write(LYC, 0x00);
    bus.Write(DMA, 0xFF);
    bus.Write(BGP, 0xFC);
    bus.Write(OBP0, 0xFF);
    bus.Write(OBP1, 0xFF);
    bus.Write(WY, 0x00);
    bus.Write(WX, 0x00);
    bus.Write(KEY1, 0xFF);
    bus.Write(VBK, 0xFF);
    bus.Write(HDMA1, 0xFF);
    bus.Write(HDMA2, 0xFF);
    bus.Write(HDMA3, 0xFF);
    bus.Write(HDMA4, 0xFF);
    bus.Write(HDMA5, 0xFF);
    bus.Write(RP, 0xFF);
    bus.Write(BCPS, 0xFF);
    bus.Write(BCPD, 0xFF);
    bus.Write(OCPS, 0xFF);
    bus.Write(OCPD, 0xFF);
    bus.Write(SVBK, 0xFF);
    bus.Write(IE, 0x00);
  }

  void HardwareDMG(Bus& bus) {
    bus.Write(P1, 0xCF);
    bus.Write(SB, 0x00);
    bus.Write(SC, 0x7E);
    bus.Write(DIV, 0xAB);
    bus.Write(TIMA, 0x00);
    bus.Write(TMA, 0x00);
    bus.Write(TAC, 0xF8);
    bus.Write(IF, 0xE1);
    bus.Write(NR10, 0x80);
    bus.Write(NR11, 0xBF);
    bus.Write(NR12, 0xF3);
    bus.Write(NR13, 0xFF);
    bus.Write(NR14, 0xBF);
    bus.Write(NR21, 0x3F);
    bus.Write(NR22, 0x00);
    bus.Write(NR23, 0xFF);
    bus.Write(NR24, 0xBF);
    bus.Write(NR30, 0x7F);
    bus.Write(NR31, 0xFF);
    bus.Write(NR32, 0x9F);
    bus.Write(NR33, 0xFF);
    bus.Write(NR34, 0xBF);
    bus.Write(NR41, 0xFF);
    bus.Write(NR42, 0x00);
    bus.Write(NR43, 0x00);
    bus.Write(NR44, 0xBF);
    bus.Write(NR50, 0x77);
    bus.Write(NR51, 0xF3);
    bus.Write(NR52, 0xF1);
    bus.Write(LCDC, 0x91);
    bus.Write(STAT, 0x85);
    bus.Write(SCY, 0x00);
    bus.Write(SCX, 0x00);
    bus.Write(LY, 0x00);
    bus.Write(LYC, 0x00);
    bus.Write(DMA, 0xFF);
    bus.Write(BGP, 0xFC);
    bus.Write(OBP0, 0xFF);
    bus.Write(OBP1, 0xFF);
    bus.Write(WY, 0x00);
    bus.Write(WX, 0x00);
    bus.Write(KEY1, 0xFF);
    bus.Write(VBK, 0xFF);
    bus.Write(HDMA1, 0xFF);
    bus.Write(HDMA2, 0xFF);
    bus.Write(HDMA3, 0xFF);
    bus.Write(HDMA4, 0xFF);
    bus.Write(HDMA5, 0xFF);
    bus.Write(RP, 0xFF);
    bus.Write(BCPS, 0xFF);
    bus.Write(BCPD, 0xFF);
    bus.Write(OCPS, 0xFF);
    bus.Write(OCPD, 0xFF);
    bus.Write(SVBK, 0xFF);
    bus.Write(IE, 0x00);
  }

  void HardwareSGB(Bus& bus) {
    bus.Write(P1, 0xCF);
    bus.Write(SB, 0x00);
    bus.Write(SC, 0x7E);
    bus.Write(DIV, 0x0); // TODO
    bus.Write(TIMA, 0x00);
    bus.Write(TMA, 0x00);
    bus.Write(TAC, 0xF8);
    bus.Write(IF, 0xE1);
    bus.Write(NR10, 0x80);
    bus.Write(NR11, 0xBF);
    bus.Write(NR12, 0xF3);
    bus.Write(NR13, 0xFF);
    bus.Write(NR14, 0xBF);
    bus.Write(NR21, 0x3F);
    bus.Write(NR22, 0x00);
    bus.Write(NR23, 0xFF);
    bus.Write(NR24, 0xBF);
    bus.Write(NR30, 0x7F);
    bus.Write(NR31, 0xFF);
    bus.Write(NR32, 0x9F);
    bus.Write(NR33, 0xFF);
    bus.Write(NR34, 0xBF);
    bus.Write(NR41, 0xFF);
    bus.Write(NR42, 0x00);
    bus.Write(NR43, 0x00);
    bus.Write(NR44, 0xBF);
    bus.Write(NR50, 0x77);
    bus.Write(NR51, 0xF3);
    bus.Write(NR52, 0xF0);
    bus.Write(LCDC, 0x91);
    bus.Write(STAT, 0x0); // TODO
    bus.Write(SCY, 0x00);
    bus.Write(SCX, 0x00);
    bus.Write(LY, 0x5);
    bus.Write(LYC, 0x00);
    bus.Write(DMA, 0xFF);
    bus.Write(BGP, 0xFC);
    bus.Write(OBP0, 0xFF);
    bus.Write(OBP1, 0xFF);
    bus.Write(WY, 0x00);
    bus.Write(WX, 0x00);
    bus.Write(KEY1, 0xFF);
    bus.Write(VBK, 0xFF);
    bus.Write(HDMA1, 0xFF);
    bus.Write(HDMA2, 0xFF);
    bus.Write(HDMA3, 0xFF);
    bus.Write(HDMA4, 0xFF);
    bus.Write(HDMA5, 0xFF);
    bus.Write(RP, 0xFF);
    bus.Write(BCPS, 0xFF);
    bus.Write(BCPD, 0xFF);
    bus.Write(OCPS, 0xFF);
    bus.Write(OCPD, 0xFF);
    bus.Write(SVBK, 0xFF);
    bus.Write(IE, 0x00);
  }

  void HardwareCGB(Bus& bus) {
    bus.Write(P1, 0xCF);
    bus.Write(SB, 0x00);
    bus.Write(SC, 0x7F);
    bus.Write(DIV, 0x0); // TODO
    bus.Write(TIMA, 0x00);
    bus.Write(TMA, 0x00);
    bus.Write(TAC, 0xF8);
    bus.Write(IF, 0xE1);
    bus.Write(NR10, 0x80);
    bus.Write(NR11, 0xBF);
    bus.Write(NR12, 0xF3);
    bus.Write(NR13, 0xFF);
    bus.Write(NR14, 0xBF);
    bus.Write(NR21, 0x3F);
    bus.Write(NR22, 0x00);
    bus.Write(NR23, 0xFF);
    bus.Write(NR24, 0xBF);
    bus.Write(NR30, 0x7F);
    bus.Write(NR31, 0xFF);
    bus.Write(NR32, 0x9F);
    bus.Write(NR33, 0xFF);
    bus.Write(NR34, 0xBF);
    bus.Write(NR41, 0xFF);
    bus.Write(NR42, 0x00);
    bus.Write(NR43, 0x00);
    bus.Write(NR44, 0xBF);
    bus.Write(NR50, 0x77);
    bus.Write(NR51, 0xF3);
    bus.Write(NR52, 0xF1);
    bus.Write(LCDC, 0x91);
    bus.Write(STAT, 0x0); // TODO
    bus.Write(SCY, 0x00);
    bus.Write(SCX, 0x00);
    bus.Write(LY, 0x0); // TODO
    bus.Write(LYC, 0x00);
    bus.Write(DMA, 0x00);
    bus.Write(BGP, 0xFC);
    bus.Write(OBP0, 0xFF);
    bus.Write(OBP1, 0xFF);
    bus.Write(WY, 0x00);
    bus.Write(WX, 0x00);
    bus.Write(KEY1, 0xFF);
    bus.Write(VBK, 0xFF);
    bus.Write(HDMA1, 0xFF);
    bus.Write(HDMA2, 0xFF);
    bus.Write(HDMA3, 0xFF);
    bus.Write(HDMA4, 0xFF);
    bus.Write(HDMA5, 0xFF);
    bus.Write(RP, 0xFF);
    bus.Write(BCPS, 0xFF);
    bus.Write(BCPD, 0xFF);
    bus.Write(OCPS, 0xFF);
    bus.Write(OCPD, 0xFF);
    bus.Write(SVBK, 0xFF);
    bus.Write(IE, 0x00);
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