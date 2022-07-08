#include <gtest/gtest.h>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

class BlarggTest : public ::testing::TestWithParam<std::string>, protected ResourceHelper {
public:
    BlarggTest() : ResourceHelper() {
    }

    virtual ~BlarggTest() = default;

protected:
    gandalf::Gameboy gameboy_;
};

TEST_P(BlarggTest, cpu_instructions)
{
    std::string file_name = GetParam() + ".gb";
    std::cout << "Running test ROM: " << file_name << std::endl;

    auto rom_bytes = ReadFileBytes("/blargg/cpu_instrs/" + file_name);

    for (const std::string& bootrom : {"dmg"})
    {
        try {
            auto bootrom_bytes = ReadFileBytes("/bootroms/" + bootrom + ".bin");
            gandalf::Gameboy::BootROM bootrom;
            std::copy(bootrom_bytes.begin(), bootrom_bytes.end(), bootrom.begin());
            gandalf::Gameboy gb;
            gb.LoadBootROM(bootrom);
            gb.Load(rom_bytes);
        } catch(std::runtime_error& e)
        {
            FAIL() << "Could not find boot rom for: " << bootrom;
        }        
    }

}

INSTANTIATE_TEST_SUITE_P(
    BlarggTest,
    BlarggTest,
    ::testing::Values(
        "cpu_instrs", "01-special", "02-interrupts", "03-op sp,hl", "04-op r,imm",
        "05-op rp", "06-ld r,r", "07-jr,jp,call,ret,rst", "08-misc instrs",
        "09-op r,r", "10-bit ops", "11-op a,(hl)"
    )
);