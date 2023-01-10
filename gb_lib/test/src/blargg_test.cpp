#include <gtest/gtest.h>

#include <memory>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

namespace {
    constexpr std::uint64_t kMaxCycles = static_cast<std::uint64_t>(1e7);

    class TestRunner
    {
    public:
        virtual ~TestRunner() = default;

        virtual bool RunTestROM(gandalf::Gameboy& gb) = 0;
        virtual const std::string& GetOutput() const = 0;
    };

    enum class TestOutput
    {
        Serial,
        Memory
    };

    struct TestProperties
    {
        TestOutput validator;
        std::string rom_path;
    };

    std::ostream& operator<<(std::ostream& os, const TestProperties& p)
    {
        os << p.rom_path;
        return os;
    }

    class BlarggTest: public ::testing::TestWithParam<TestProperties>, protected ResourceHelper {
    public:
        BlarggTest(): ResourceHelper() {
        }

        virtual ~BlarggTest() = default;

    protected:
        std::unique_ptr<gandalf::Gameboy> gameboy_;
    };
}

namespace blargg {
    using namespace gandalf;
    // Test will write its output to serial, this class overrides the default serial address handler and stores the output from the test in a string
    class SerialOutputValidator: public Memory::AddressHandler, public TestRunner
    {
    public:
        SerialOutputValidator(): Memory::AddressHandler("BlarggTest - SerialOutputReader"), done_(false), last_character_(0) {}
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

        bool RunTestROM(Gameboy& gb) override
        {
            gb.RegisterAddressHandler(*this);

            std::size_t ticks = 0;
            while (!done_ && ++ticks < kMaxCycles)
                gb.Run();

            return output_.find("Pass") != std::string::npos;
        }

        const std::string& GetOutput() const override { return output_; }

        bool Done() const { return done_; }
    private:
        byte last_character_;
        std::string output_;
        bool done_;
    };

    // Test will write output to memory. Address 0xA000 holds the overall status, 0x80 means the test is still running.
    class MemoryValidator: public TestRunner
    {
        bool CheckResultValid(const Memory& memory)
        {
            bool result_valid = true;
            result_valid &= memory.Read(0xA001, false) == 0xDE;
            result_valid &= memory.Read(0xA002, false) == 0xB0;
            result_valid &= memory.Read(0xA003, false) == 0x61;
            return result_valid;
        }
        
        bool RunTestROM(Gameboy& gb)
        {
            std::uint64_t i = 0;
            auto& memory = gb.GetMemory();
            while (i < kMaxCycles)
            {
                gb.Run();

                // 0xA000 holds the status code, check it every once in a while to detect if the test has finished.
                if (i % 10000 == 0)
                {
                    if (memory.Read(0xA000, false) != 0x80 && CheckResultValid(memory))
                        break;
                }

                ++i;
            }

            const bool result_valid = CheckResultValid(memory);
            const byte result_code = memory.Read(0xA000, false);

            word address = 0xA004;
            if (result_valid) {
                while (memory.Read(address, false) != 0x00)
                    output_ += memory.Read(address++, false);
            }

            return result_valid && result_code == 0;
        }

        const std::string& GetOutput() const override
        {
            return output_;
        }

    private:
        std::string output_;
    };

    TEST_P(BlarggTest, test_rom)
    {
        using namespace gandalf;

        TestProperties p = GetParam();

        std::string file_name = p.rom_path;
        std::cout << "Running test ROM: " << file_name << std::endl;

        ROM rom_bytes;
        ASSERT_TRUE(ReadFileBytes("/blargg/" + file_name, rom_bytes));

        for (int model = 0; model < static_cast<int>(gandalf::Model::LAST); ++model) {
            std::unique_ptr<TestRunner> runner;
            switch (p.validator)
            {
            case TestOutput::Serial:
                runner = std::make_unique<SerialOutputValidator>();
                break;
            case TestOutput::Memory:
                runner = std::make_unique<MemoryValidator>();
                break;
            }

            gameboy_ = std::make_unique<gandalf::Gameboy>(static_cast<gandalf::Model>(model));
            ASSERT_TRUE(gameboy_->LoadROM(rom_bytes));

            EXPECT_TRUE(runner->RunTestROM(*gameboy_)) << "Test failed on model " << gandalf::GetModelName(static_cast<gandalf::Model>(model)) << std::endl << runner->GetOutput();
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        cpu_instrs,
        BlarggTest,
        ::testing::Values(
            TestProperties{ TestOutput::Serial, "cpu_instrs/01-special.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/02-interrupts.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/03-op sp,hl.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/04-op r,imm.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/05-op rp.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/06-ld r,r.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/07-jr,jp,call,ret,rst.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/08-misc instrs.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/09-op r,r.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/10-bit ops.gb" },
            TestProperties{ TestOutput::Serial, "cpu_instrs/11-op a,(hl).gb" }
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        mem_timing,
        BlarggTest,
        ::testing::Values(
            TestProperties{ TestOutput::Serial, "mem_timing/01-read_timing.gb" },
            TestProperties{ TestOutput::Serial, "mem_timing/02-write_timing.gb" },
            TestProperties{ TestOutput::Serial, "mem_timing/03-modify_timing.gb" }
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        mem_timing2,
        BlarggTest,
        ::testing::Values(
            TestProperties{ TestOutput::Memory, "mem_timing-2/01-read_timing.gb" },
            TestProperties{ TestOutput::Memory, "mem_timing-2/02-write_timing.gb" },
            TestProperties{ TestOutput::Memory, "mem_timing-2/03-modify_timing.gb" }

        )
    );

    INSTANTIATE_TEST_SUITE_P(
        instr_timing,
        BlarggTest,
        ::testing::Values(
            TestProperties{ TestOutput::Serial, "instr_timing/instr_timing.gb" }
        )
    );
}