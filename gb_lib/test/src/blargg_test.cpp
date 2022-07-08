#include <gtest/gtest.h>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

class BlarggTest : public ::testing::TestWithParam<std::string>, protected ResourceHelper {
public:
    BlarggTest() : ResourceHelper("blargg/cpu_instrs") {
    }

    virtual ~BlarggTest() = default;

protected:
    gandalf::Gameboy gameboy_;
};

TEST_P(BlarggTest, cpu_instructions)
{
    std::string file_name = GetParam() + ".gb";
    std::cout << "Running test ROM: " << file_name << std::endl;

    auto bytes = ReadFileBytes(file_name);
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