#include <gtest/gtest.h>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

class BlarggTest : public ::testing::TestWithParam<std::string>, protected ResourceHelper {
public:
    BlarggTest() : ResourceHelper() {
    }

    virtual ~BlarggTest() = default;

protected:
    std::unique_ptr<gandalf::Gameboy> gameboy_;
};

namespace gandalf {
    // Test will write its output to serial, this class overrides the default serial address handler and stores the output from the test in a string
    class SerialOutputReader : public Bus::AddressHandler
    {
    public:
        SerialOutputReader() : Bus::AddressHandler("BlarggTest - SerialOutputReader"), done_(false), last_character_(false) {}
        void Write(word address, byte value) override
        {
            if (address == kSC) {
                if (value == 0x81) {
                    output_ += last_character_;
                    if (output_.find("Pass") != std::string::npos || output_.find("Fail") != std::string::npos) {
                        done_ = true;
                    }
                }
            }
            else if (address == kSB)
                last_character_ = value;
            else
                FAIL() << "Unexpected write to serial address " << std::hex << address << " with value " << std::hex << value;
        }

        byte Read(word) const override
        {
            return 0xFF;
        }

        std::set<word> GetAddresses() const override
        {
            return { kSB, kSC };
        }

        std::string GetOutput() const { return output_; }

        bool Done() const { return done_; }
    private:
        byte last_character_;
        std::string output_;
        bool done_;
    };
}

TEST_P(BlarggTest, cpu_instructions)
{
    using namespace gandalf;

    std::string file_name = GetParam() + ".gb";
    std::cout << "Running test ROM: " << file_name << std::endl;

    ROM rom_bytes;
    ASSERT_TRUE(ReadFileBytes("/blargg/cpu_instrs/" + file_name, rom_bytes));

    ROM bootrom_bytes;
    ASSERT_TRUE(ReadFileBytes("/bootrom/boot.bin", bootrom_bytes));
    std::unique_ptr<Gameboy> gb = std::make_unique<Gameboy>(bootrom_bytes, rom_bytes, nullptr);

    ASSERT_TRUE(gb->Ready());

    Bus& bus = gb->GetBus();

    SerialOutputReader serial_output_reader;
    bus.Register(serial_output_reader);

    std::size_t ticks = 0;
    while (!serial_output_reader.Done() && ++ticks < 1e8)
        gb->Run();

    std::cout << "Test output: " << std::endl << serial_output_reader.GetOutput() << std::endl;

    std::string output = serial_output_reader.GetOutput();
    EXPECT_TRUE(serial_output_reader.Done());
    EXPECT_TRUE(output.find("Pass") != std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(
    BlarggTest,
    BlarggTest,
    ::testing::Values(
        "01-special", "02-interrupts", "03-op sp,hl", "04-op r,imm",
        "05-op rp", "06-ld r,r", "07-jr,jp,call,ret,rst", "08-misc instrs",
        "09-op r,r", "10-bit ops", "11-op a,(hl)", "cpu_instrs"
    )
);