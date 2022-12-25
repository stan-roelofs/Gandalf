#include <gtest/gtest.h>

#include <memory>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

namespace {
    constexpr std::uint64_t kMaxCycles = static_cast<std::uint64_t>(1e8);

    class MooneyeTest: public ::testing::TestWithParam<std::string>, protected ResourceHelper {
    public:
        MooneyeTest(): ResourceHelper() {
        }

        virtual ~MooneyeTest() = default;

    protected:
        std::unique_ptr<gandalf::Gameboy> gameboy_;
    };
}

namespace mooneye {
    using namespace gandalf;

    class SerialOutputReader: public Serial
    {
    public:
        SerialOutputReader(): Serial(GameboyMode::DMG), done_(false) {}

        void Write(word address, byte value) override
        {
            if (address == kSB) {
                serial_bytes_.push_back(value);
                done_ = serial_bytes_.size() == 6;
            }

            Serial::Write(address, value);
        }

        byte Read(word address) const override
        {
            return Serial::Read(address);
        }

        bool Done() const {
            return done_;
        }

        bool Passed() const
        {
            return serial_bytes_.size() == 6 && serial_bytes_[0] == 3 && serial_bytes_[1] == 5
                && serial_bytes_[2] == 8 && serial_bytes_[3] == 13 && serial_bytes_[4] == 21 && serial_bytes_[5] == 34;
        }

    private:
        byte sc_;
        bool done_;
        std::vector<byte> serial_bytes_;
    };

    TEST_P(MooneyeTest, test_rom)
    {
        using namespace gandalf;

        std::string file_name = GetParam();
        std::cout << "Running test ROM: " << file_name << std::endl;

        ROM rom_bytes;
        ASSERT_TRUE(ReadFileBytes("/mooneye/" + file_name, rom_bytes));

        ROM bootrom_bytes;
        ASSERT_TRUE(ReadFileBytes("/bootrom/boot.bin", bootrom_bytes));
        std::unique_ptr<Gameboy> gb = std::make_unique<Gameboy>(bootrom_bytes, rom_bytes, nullptr);

        ASSERT_TRUE(gb->Ready());
        auto& memory = gb->GetMemory();
        SerialOutputReader p;
        memory.Register(p);

        std::size_t ticks = 0;
        while (!p.Done() && ++ticks < kMaxCycles) {
            gb->Run();
        }

        EXPECT_TRUE(p.Passed());
    }

    INSTANTIATE_TEST_SUITE_P(
        acceptance_timer,
        MooneyeTest,
        ::testing::Values(
            "acceptance/timer/div_write.gb",
            "acceptance/timer/rapid_toggle.gb",
            "acceptance/timer/tim00_div_trigger.gb",
            "acceptance/timer/tim00.gb",
            "acceptance/timer/tim00_div_trigger.gb",
            "acceptance/timer/tim01.gb",
            "acceptance/timer/tim01_div_trigger.gb",
            "acceptance/timer/tim10.gb",
            "acceptance/timer/tim10_div_trigger.gb",
            "acceptance/timer/tim11.gb",
            "acceptance/timer/tim11_div_trigger.gb",
            "acceptance/timer/tima_reload.gb",
            "acceptance/timer/tima_write_reloading.gb",
            "acceptance/timer/tma_write_reloading.gb"
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_bits,
        MooneyeTest,
        ::testing::Values(
            "acceptance/bits/mem_oam.gb",
            "acceptance/bits/reg_f.gb",
            "acceptance/bits/unused_hwio-GS.gb"
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_instr,
        MooneyeTest,
        ::testing::Values(
            "acceptance/instr/daa.gb"
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_interrupts,
        MooneyeTest,
        ::testing::Values(
            "acceptance/interrupts/ie_push.gb"
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_oem_dma,
        MooneyeTest,
        ::testing::Values(
            "acceptance/oam_dma/basic.gb",
            "acceptance/oam_dma/reg_read.gb",
            "acceptance/oam_dma/sources-GS.gb"
        )
    );
    
    INSTANTIATE_TEST_SUITE_P(
        acceptance,
        MooneyeTest,
        ::testing::Values(
            "acceptance/boot_regs-dmgABC.gb",
			"acceptance/div_timing.gb",
            "acceptance/ei_timing.gb",
            "acceptance/ei_sequence.gb",
            "acceptance/halt_ime0_ei.gb",
            "acceptance/halt_ime1_timing.gb",
            "acceptance/if_ie_registers.gb",
            "acceptance/pop_timing.gb",
            "acceptance/rapid_di_ei.gb",
            "acceptance/reti_intr_timing.gb"
        )
    );
}