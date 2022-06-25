#include "BootROM.h"

void RegistersDMG0(CPU::Registers& registers)
{
    registers.A = 0x01;
    registers.F = 0x00;
    registers.B = 0xFF;
    registers.C = 0x13;
    registers.D = 0x00;
    registers.E = 0xC1;
    registers.H = 0x84;
    registers.L = 0x03;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void RegistersDMG(CPU::Registers& registers)
{
    registers.A = 0x01;
    registers.F = 0x80;
    registers.B = 0x00;
    registers.C = 0x13;
    registers.D = 0x00;
    registers.E = 0xD8;
    registers.H = 0x01;
    registers.L = 0x4D;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void RegistersMGB(CPU::Registers& registers)
{
    registers.A = 0xFF;
    registers.F = 0x80;
    registers.B = 0x00;
    registers.C = 0x13;
    registers.D = 0x00;
    registers.E = 0xD8;
    registers.H = 0x01;
    registers.L = 0x4D;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void RegistersSGB(CPU::Registers& registers)
{
    registers.A = 0x01;
    registers.F = 0x00;
    registers.B = 0x00;
    registers.C = 0x14;
    registers.D = 0x00;
    registers.E = 0x00;
    registers.H = 0xC0;
    registers.L = 0x60;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void RegistersSGB2(CPU::Registers& registers)
{
    registers.A = 0xFF;
    registers.F = 0x00;
    registers.B = 0x00;
    registers.C = 0x14;
    registers.D = 0x00;
    registers.E = 0x00;
    registers.H = 0xC0;
    registers.L = 0x60;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void RegistersCGB(CPU::Registers& registers)
{
    registers.A = 0x11;
    registers.F = 0x80;
    registers.B = 0x00;
    registers.C = 0x00;
    registers.D = 0xFF;
    registers.E = 0x56;
    registers.H = 0x00;
    registers.L = 0x0D;
    registers.PC = 0x0100;
    registers.SP = 0xFFFE;
}

void HardwareDMG0(Memory& memory)
{
    memory[P1] = 0xCF;
    memory[SB] = 0x00;
    memory[SC] = 0x7E;
    memory[DIV] = 0x18;
    memory[TIMA] = 0x00;
    memory[TMA] = 0x00;
    memory[TAC] = 0xF8;
    memory[IF] = 0xE1;
    memory[NR10] = 0x80;
    memory[NR11] = 0xBF;
    memory[NR12] = 0xF3;
    memory[NR13] = 0xFF;
    memory[NR14] = 0xBF;
    memory[NR21] = 0x3F;
    memory[NR22] = 0x00;
    memory[NR23] = 0xFF;
    memory[NR24] = 0xBF;
    memory[NR30] = 0x7F;
    memory[NR31] = 0xFF;
    memory[NR32] = 0x9F;
    memory[NR33] = 0xFF;
    memory[NR34] = 0xBF;
    memory[NR41] = 0xFF;
    memory[NR42] = 0x00;
    memory[NR43] = 0x00;
    memory[NR44] = 0xBF;
    memory[NR50] = 0x77;
    memory[NR51] = 0xF3;
    memory[NR52] = 0xF1;
    memory[LCDC] = 0x91;
    memory[STAT] = 0x81;
    memory[SCY] = 0x00;
    memory[SCX] = 0x00;
    memory[LY] = 0x91;
    memory[LYC] = 0x00;
    memory[DMA] = 0xFF;
    memory[BGP] = 0xFC;
    memory[OBP0] = 0xFF;
    memory[OBP1] = 0xFF;
    memory[WY] = 0x00;
    memory[WX] = 0x00;
    memory[KEY1] = 0xFF;
    memory[VBK] = 0xFF;
    memory[HDMA1] = 0xFF;
    memory[HDMA2] = 0xFF;
    memory[HDMA3] = 0xFF;
    memory[HDMA4] = 0xFF;
    memory[HDMA5] = 0xFF;
    memory[RP] = 0xFF;
    memory[BCPS] = 0xFF;
    memory[BCPD] = 0xFF;
    memory[OCPS] = 0xFF;
    memory[OCPD] = 0xFF;
    memory[SVBK] = 0xFF;
    memory[IE] = 0x00;
}

void HardwareDMG(Memory& memory)
{
    memory[P1] = 0xCF;
    memory[SB] = 0x00;
    memory[SC] = 0x7E;
    memory[DIV] = 0xAB;
    memory[TIMA] = 0x00;
    memory[TMA] = 0x00;
    memory[TAC] = 0xF8;
    memory[IF] = 0xE1;
    memory[NR10] = 0x80;
    memory[NR11] = 0xBF;
    memory[NR12] = 0xF3;
    memory[NR13] = 0xFF;
    memory[NR14] = 0xBF;
    memory[NR21] = 0x3F;
    memory[NR22] = 0x00;
    memory[NR23] = 0xFF;
    memory[NR24] = 0xBF;
    memory[NR30] = 0x7F;
    memory[NR31] = 0xFF;
    memory[NR32] = 0x9F;
    memory[NR33] = 0xFF;
    memory[NR34] = 0xBF;
    memory[NR41] = 0xFF;
    memory[NR42] = 0x00;
    memory[NR43] = 0x00;
    memory[NR44] = 0xBF;
    memory[NR50] = 0x77;
    memory[NR51] = 0xF3;
    memory[NR52] = 0xF1;
    memory[LCDC] = 0x91;
    memory[STAT] = 0x85;
    memory[SCY] = 0x00;
    memory[SCX] = 0x00;
    memory[LY] = 0x00;
    memory[LYC] = 0x00;
    memory[DMA] = 0xFF;
    memory[BGP] = 0xFC;
    memory[OBP0] = 0xFF;
    memory[OBP1] = 0xFF;
    memory[WY] = 0x00;
    memory[WX] = 0x00;
    memory[KEY1] = 0xFF;
    memory[VBK] = 0xFF;
    memory[HDMA1] = 0xFF;
    memory[HDMA2] = 0xFF;
    memory[HDMA3] = 0xFF;
    memory[HDMA4] = 0xFF;
    memory[HDMA5] = 0xFF;
    memory[RP] = 0xFF;
    memory[BCPS] = 0xFF;
    memory[BCPD] = 0xFF;
    memory[OCPS] = 0xFF;
    memory[OCPD] = 0xFF;
    memory[SVBK] = 0xFF;
    memory[IE] = 0x00;
}

void HardwareSGB(Memory& memory)
{
    memory[P1] = 0xCF;
    memory[SB] = 0x00;
    memory[SC] = 0x7E;
    memory[DIV] = 0x0; // TODO
    memory[TIMA] = 0x00;
    memory[TMA] = 0x00;
    memory[TAC] = 0xF8;
    memory[IF] = 0xE1;
    memory[NR10] = 0x80;
    memory[NR11] = 0xBF;
    memory[NR12] = 0xF3;
    memory[NR13] = 0xFF;
    memory[NR14] = 0xBF;
    memory[NR21] = 0x3F;
    memory[NR22] = 0x00;
    memory[NR23] = 0xFF;
    memory[NR24] = 0xBF;
    memory[NR30] = 0x7F;
    memory[NR31] = 0xFF;
    memory[NR32] = 0x9F;
    memory[NR33] = 0xFF;
    memory[NR34] = 0xBF;
    memory[NR41] = 0xFF;
    memory[NR42] = 0x00;
    memory[NR43] = 0x00;
    memory[NR44] = 0xBF;
    memory[NR50] = 0x77;
    memory[NR51] = 0xF3;
    memory[NR52] = 0xF0;
    memory[LCDC] = 0x91;
    memory[STAT] = 0x0; // TODO
    memory[SCY] = 0x00;
    memory[SCX] = 0x00;
    memory[LY] = 0x5;
    memory[LYC] = 0x00;
    memory[DMA] = 0xFF;
    memory[BGP] = 0xFC;
    memory[OBP0] = 0xFF;
    memory[OBP1] = 0xFF;
    memory[WY] = 0x00;
    memory[WX] = 0x00;
    memory[KEY1] = 0xFF;
    memory[VBK] = 0xFF;
    memory[HDMA1] = 0xFF;
    memory[HDMA2] = 0xFF;
    memory[HDMA3] = 0xFF;
    memory[HDMA4] = 0xFF;
    memory[HDMA5] = 0xFF;
    memory[RP] = 0xFF;
    memory[BCPS] = 0xFF;
    memory[BCPD] = 0xFF;
    memory[OCPS] = 0xFF;
    memory[OCPD] = 0xFF;
    memory[SVBK] = 0xFF;
    memory[IE] = 0x00;
}

void HardwareCGB(Memory& memory)
{
    memory[P1] = 0xCF;
    memory[SB] = 0x00;
    memory[SC] = 0x7F;
    memory[DIV] = 0x0; // TODO
    memory[TIMA] = 0x00;
    memory[TMA] = 0x00;
    memory[TAC] = 0xF8;
    memory[IF] = 0xE1;
    memory[NR10] = 0x80;
    memory[NR11] = 0xBF;
    memory[NR12] = 0xF3;
    memory[NR13] = 0xFF;
    memory[NR14] = 0xBF;
    memory[NR21] = 0x3F;
    memory[NR22] = 0x00;
    memory[NR23] = 0xFF;
    memory[NR24] = 0xBF;
    memory[NR30] = 0x7F;
    memory[NR31] = 0xFF;
    memory[NR32] = 0x9F;
    memory[NR33] = 0xFF;
    memory[NR34] = 0xBF;
    memory[NR41] = 0xFF;
    memory[NR42] = 0x00;
    memory[NR43] = 0x00;
    memory[NR44] = 0xBF;
    memory[NR50] = 0x77;
    memory[NR51] = 0xF3;
    memory[NR52] = 0xF1;
    memory[LCDC] = 0x91;
    memory[STAT] = 0x0; // TODO
    memory[SCY] = 0x00;
    memory[SCX] = 0x00;
    memory[LY] = 0x0; // TODO
    memory[LYC] = 0x00;
    memory[DMA] = 0x00;
    memory[BGP] = 0xFC;
    memory[OBP0] = 0xFF;
    memory[OBP1] = 0xFF;
    memory[WY] = 0x00;
    memory[WX] = 0x00;
    memory[KEY1] = 0xFF;
    memory[VBK] = 0xFF;
    memory[HDMA1] = 0xFF;
    memory[HDMA2] = 0xFF;
    memory[HDMA3] = 0xFF;
    memory[HDMA4] = 0xFF;
    memory[HDMA5] = 0xFF;
    memory[RP] = 0xFF;
    memory[BCPS] = 0xFF;
    memory[BCPD] = 0xFF;
    memory[OCPS] = 0xFF;
    memory[OCPD] = 0xFF;
    memory[SVBK] = 0xFF;
    memory[IE] = 0x00;
}

void SimulateBootROM(Model model, CPU::Registers& registers, Memory& memory)
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
