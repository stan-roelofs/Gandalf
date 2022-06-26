#include "bootrom.h"

void RegistersDMG0(Registers& registers)
{
    registers.a = 0x01;
    registers.f = 0x00;
    registers.b = 0xFF;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xC1;
    registers.h = 0x84;
    registers.l = 0x03;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void RegistersDMG(Registers& registers)
{
    registers.a = 0x01;
    registers.f = 0x80;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void RegistersMGB(Registers& registers)
{
    registers.a = 0xFF;
    registers.f = 0x80;
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void RegistersSGB(Registers& registers)
{
    registers.a = 0x01;
    registers.f = 0x00;
    registers.b = 0x00;
    registers.c = 0x14;
    registers.d = 0x00;
    registers.e = 0x00;
    registers.h = 0xC0;
    registers.l = 0x60;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void RegistersSGB2(Registers& registers)
{
    registers.a = 0xFF;
    registers.f = 0x00;
    registers.b = 0x00;
    registers.c = 0x14;
    registers.d = 0x00;
    registers.e = 0x00;
    registers.h = 0xC0;
    registers.l = 0x60;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void RegistersCGB(Registers& registers)
{
    registers.a = 0x11;
    registers.f = 0x80;
    registers.b = 0x00;
    registers.c = 0x00;
    registers.d = 0xFF;
    registers.e = 0x56;
    registers.h = 0x00;
    registers.l = 0x0D;
    registers.program_counter = 0x0100;
    registers.stack_pointer = 0xFFFE;
}

void HardwareDMG0(Memory& memory)
{
    memory.Write(P1, 0xCF);
    memory.Write(SB, 0x00);
    memory.Write(SC, 0x7E);
    memory.Write(DIV, 0x18);
    memory.Write(TIMA, 0x00);
    memory.Write(TMA, 0x00);
    memory.Write(TAC, 0xF8);
    memory.Write(IF, 0xE1);
    memory.Write(NR10, 0x80);
    memory.Write(NR11, 0xBF);
    memory.Write(NR12, 0xF3);
    memory.Write(NR13, 0xFF);
    memory.Write(NR14, 0xBF);
    memory.Write(NR21, 0x3F);
    memory.Write(NR22, 0x00);
    memory.Write(NR23, 0xFF);
    memory.Write(NR24, 0xBF);
    memory.Write(NR30, 0x7F);
    memory.Write(NR31, 0xFF);
    memory.Write(NR32, 0x9F);
    memory.Write(NR33, 0xFF);
    memory.Write(NR34, 0xBF);
    memory.Write(NR41, 0xFF);
    memory.Write(NR42, 0x00);
    memory.Write(NR43, 0x00);
    memory.Write(NR44, 0xBF);
    memory.Write(NR50, 0x77);
    memory.Write(NR51, 0xF3);
    memory.Write(NR52, 0xF1);
    memory.Write(LCDC, 0x91);
    memory.Write(STAT, 0x81);
    memory.Write(SCY, 0x00);
    memory.Write(SCX, 0x00);
    memory.Write(LY, 0x91);
    memory.Write(LYC, 0x00);
    memory.Write(DMA, 0xFF);
    memory.Write(BGP, 0xFC);
    memory.Write(OBP0, 0xFF);
    memory.Write(OBP1, 0xFF);
    memory.Write(WY, 0x00);
    memory.Write(WX, 0x00);
    memory.Write(KEY1, 0xFF);
    memory.Write(VBK, 0xFF);
    memory.Write(HDMA1, 0xFF);
    memory.Write(HDMA2, 0xFF);
    memory.Write(HDMA3, 0xFF);
    memory.Write(HDMA4, 0xFF);
    memory.Write(HDMA5, 0xFF);
    memory.Write(RP, 0xFF);
    memory.Write(BCPS, 0xFF);
    memory.Write(BCPD, 0xFF);
    memory.Write(OCPS, 0xFF);
    memory.Write(OCPD, 0xFF);
    memory.Write(SVBK, 0xFF);
    memory.Write(IE, 0x00);
}

void HardwareDMG(Memory& memory)
{
    memory.Write(P1, 0xCF);
    memory.Write(SB, 0x00);
    memory.Write(SC, 0x7E);
    memory.Write(DIV, 0xAB);
    memory.Write(TIMA, 0x00);
    memory.Write(TMA, 0x00);
    memory.Write(TAC, 0xF8);
    memory.Write(IF, 0xE1);
    memory.Write(NR10, 0x80);
    memory.Write(NR11, 0xBF);
    memory.Write(NR12, 0xF3);
    memory.Write(NR13, 0xFF);
    memory.Write(NR14, 0xBF);
    memory.Write(NR21, 0x3F);
    memory.Write(NR22, 0x00);
    memory.Write(NR23, 0xFF);
    memory.Write(NR24, 0xBF);
    memory.Write(NR30, 0x7F);
    memory.Write(NR31, 0xFF);
    memory.Write(NR32, 0x9F);
    memory.Write(NR33, 0xFF);
    memory.Write(NR34, 0xBF);
    memory.Write(NR41, 0xFF);
    memory.Write(NR42, 0x00);
    memory.Write(NR43, 0x00);
    memory.Write(NR44, 0xBF);
    memory.Write(NR50, 0x77);
    memory.Write(NR51, 0xF3);
    memory.Write(NR52, 0xF1);
    memory.Write(LCDC, 0x91);
    memory.Write(STAT, 0x85);
    memory.Write(SCY, 0x00);
    memory.Write(SCX, 0x00);
    memory.Write(LY, 0x00);
    memory.Write(LYC, 0x00);
    memory.Write(DMA, 0xFF);
    memory.Write(BGP, 0xFC);
    memory.Write(OBP0, 0xFF);
    memory.Write(OBP1, 0xFF);
    memory.Write(WY, 0x00);
    memory.Write(WX, 0x00);
    memory.Write(KEY1, 0xFF);
    memory.Write(VBK, 0xFF);
    memory.Write(HDMA1, 0xFF);
    memory.Write(HDMA2, 0xFF);
    memory.Write(HDMA3, 0xFF);
    memory.Write(HDMA4, 0xFF);
    memory.Write(HDMA5, 0xFF);
    memory.Write(RP, 0xFF);
    memory.Write(BCPS, 0xFF);
    memory.Write(BCPD, 0xFF);
    memory.Write(OCPS, 0xFF);
    memory.Write(OCPD, 0xFF);
    memory.Write(SVBK, 0xFF);
    memory.Write(IE, 0x00);
}

void HardwareSGB(Memory& memory)
{
    memory.Write(P1, 0xCF);
    memory.Write(SB, 0x00);
    memory.Write(SC, 0x7E);
    memory.Write(DIV, 0x0); // TODO
    memory.Write(TIMA, 0x00);
    memory.Write(TMA, 0x00);
    memory.Write(TAC, 0xF8);
    memory.Write(IF, 0xE1);
    memory.Write(NR10, 0x80);
    memory.Write(NR11, 0xBF);
    memory.Write(NR12, 0xF3);
    memory.Write(NR13, 0xFF);
    memory.Write(NR14, 0xBF);
    memory.Write(NR21, 0x3F);
    memory.Write(NR22, 0x00);
    memory.Write(NR23, 0xFF);
    memory.Write(NR24, 0xBF);
    memory.Write(NR30, 0x7F);
    memory.Write(NR31, 0xFF);
    memory.Write(NR32, 0x9F);
    memory.Write(NR33, 0xFF);
    memory.Write(NR34, 0xBF);
    memory.Write(NR41, 0xFF);
    memory.Write(NR42, 0x00);
    memory.Write(NR43, 0x00);
    memory.Write(NR44, 0xBF);
    memory.Write(NR50, 0x77);
    memory.Write(NR51, 0xF3);
    memory.Write(NR52, 0xF0);
    memory.Write(LCDC, 0x91);
    memory.Write(STAT, 0x0); // TODO
    memory.Write(SCY, 0x00);
    memory.Write(SCX, 0x00);
    memory.Write(LY, 0x5);
    memory.Write(LYC, 0x00);
    memory.Write(DMA, 0xFF);
    memory.Write(BGP, 0xFC);
    memory.Write(OBP0, 0xFF);
    memory.Write(OBP1, 0xFF);
    memory.Write(WY, 0x00);
    memory.Write(WX, 0x00);
    memory.Write(KEY1, 0xFF);
    memory.Write(VBK, 0xFF);
    memory.Write(HDMA1, 0xFF);
    memory.Write(HDMA2, 0xFF);
    memory.Write(HDMA3, 0xFF);
    memory.Write(HDMA4, 0xFF);
    memory.Write(HDMA5, 0xFF);
    memory.Write(RP, 0xFF);
    memory.Write(BCPS, 0xFF);
    memory.Write(BCPD, 0xFF);
    memory.Write(OCPS, 0xFF);
    memory.Write(OCPD, 0xFF);
    memory.Write(SVBK, 0xFF);
    memory.Write(IE, 0x00);
}

void HardwareCGB(Memory& memory)
{
    memory.Write(P1, 0xCF);
    memory.Write(SB, 0x00);
    memory.Write(SC, 0x7F);
    memory.Write(DIV, 0x0); // TODO
    memory.Write(TIMA, 0x00);
    memory.Write(TMA, 0x00);
    memory.Write(TAC, 0xF8);
    memory.Write(IF, 0xE1);
    memory.Write(NR10, 0x80);
    memory.Write(NR11, 0xBF);
    memory.Write(NR12, 0xF3);
    memory.Write(NR13, 0xFF);
    memory.Write(NR14, 0xBF);
    memory.Write(NR21, 0x3F);
    memory.Write(NR22, 0x00);
    memory.Write(NR23, 0xFF);
    memory.Write(NR24, 0xBF);
    memory.Write(NR30, 0x7F);
    memory.Write(NR31, 0xFF);
    memory.Write(NR32, 0x9F);
    memory.Write(NR33, 0xFF);
    memory.Write(NR34, 0xBF);
    memory.Write(NR41, 0xFF);
    memory.Write(NR42, 0x00);
    memory.Write(NR43, 0x00);
    memory.Write(NR44, 0xBF);
    memory.Write(NR50, 0x77);
    memory.Write(NR51, 0xF3);
    memory.Write(NR52, 0xF1);
    memory.Write(LCDC, 0x91);
    memory.Write(STAT, 0x0); // TODO
    memory.Write(SCY, 0x00);
    memory.Write(SCX, 0x00);
    memory.Write(LY, 0x0); // TODO
    memory.Write(LYC, 0x00);
    memory.Write(DMA, 0x00);
    memory.Write(BGP, 0xFC);
    memory.Write(OBP0, 0xFF);
    memory.Write(OBP1, 0xFF);
    memory.Write(WY, 0x00);
    memory.Write(WX, 0x00);
    memory.Write(KEY1, 0xFF);
    memory.Write(VBK, 0xFF);
    memory.Write(HDMA1, 0xFF);
    memory.Write(HDMA2, 0xFF);
    memory.Write(HDMA3, 0xFF);
    memory.Write(HDMA4, 0xFF);
    memory.Write(HDMA5, 0xFF);
    memory.Write(RP, 0xFF);
    memory.Write(BCPS, 0xFF);
    memory.Write(BCPD, 0xFF);
    memory.Write(OCPS, 0xFF);
    memory.Write(OCPD, 0xFF);
    memory.Write(SVBK, 0xFF);
    memory.Write(IE, 0x00);
}

void SimulateBootROM(Model model, Registers& registers, Memory& memory)
{
    // CPU registers
    switch (model)
    {
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
    switch (model)
    {
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
