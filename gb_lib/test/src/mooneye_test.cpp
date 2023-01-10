#include <gtest/gtest.h>

#include <memory>
#include <set>

#include <gandalf/gameboy.h>

#include "resource_helper.h"

namespace {
    constexpr std::uint64_t kMaxCycles = static_cast<std::uint64_t>(1e7);

    struct TestProperties
    {
        TestProperties(std::string rom_path): rom_path(rom_path)
        {
            for (int i = 0; i < static_cast<int>(gandalf::Model::LAST); ++i) {
                models.insert(static_cast<gandalf::Model>(i));
            }
        }

        TestProperties(std::string rom_path, std::set<gandalf::Model> models): rom_path(rom_path), models(models) {}

        std::string rom_path;
        std::set<gandalf::Model> models;
    };

    std::ostream& operator<<(std::ostream& os, const TestProperties& p)
    {
        os << p.rom_path;
        return os;
    }

    class MooneyeTest: public ::testing::TestWithParam<TestProperties>, protected ResourceHelper {
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
        SerialOutputReader(): Serial(GameboyMode::DMG), sc_(0), done_(false) {}

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

        TestProperties properties = GetParam();
        std::cout << "Running test ROM: " << properties.rom_path << std::endl;

        ROM rom_bytes;
        ASSERT_TRUE(ReadFileBytes("/mooneye/" + properties.rom_path, rom_bytes));

        for (Model model : properties.models)
        {
            std::unique_ptr<Gameboy> gb = std::make_unique<Gameboy>(model);

            ASSERT_TRUE(gb->LoadROM(rom_bytes));
            auto& memory = gb->GetMemory();
            SerialOutputReader p;
            gb->RegisterAddressHandler(p);

            std::size_t ticks = 0;
            while (!p.Done() && ++ticks < kMaxCycles) {
                gb->Run();
            }

            EXPECT_TRUE(p.Passed()) << "Test failed on model: " << gandalf::GetModelName(model) << std::endl;
        }
    }

#define GROUP_G Model::DMG0, Model::DMG, Model::MGB
#define GROUP_S Model::SGB, Model::SGB2
#define GROUP_C Model::CGB0, Model::CGB

    INSTANTIATE_TEST_SUITE_P(
        acceptance_timer,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/timer/div_write.gb"),
            TestProperties("acceptance/timer/rapid_toggle.gb"),
            TestProperties("acceptance/timer/tim00_div_trigger.gb"),
            TestProperties("acceptance/timer/tim00.gb"),
            TestProperties("acceptance/timer/tim00_div_trigger.gb"),
            TestProperties("acceptance/timer/tim01.gb"),
            TestProperties("acceptance/timer/tim01_div_trigger.gb"),
            TestProperties("acceptance/timer/tim10.gb"),
            TestProperties("acceptance/timer/tim10_div_trigger.gb"),
            TestProperties("acceptance/timer/tim11.gb"),
            TestProperties("acceptance/timer/tim11_div_trigger.gb"),
            TestProperties("acceptance/timer/tima_reload.gb"),
            TestProperties("acceptance/timer/tima_write_reloading.gb"),
            TestProperties("acceptance/timer/tma_write_reloading.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_bits,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/bits/mem_oam.gb"),
            TestProperties("acceptance/bits/reg_f.gb"),
            TestProperties("acceptance/bits/unused_hwio-GS.gb", { GROUP_G, GROUP_S })
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_instr,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/instr/daa.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_interrupts,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/interrupts/ie_push.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_oem_dma,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/oam_dma/basic.gb"),
            TestProperties("acceptance/oam_dma/reg_read.gb"),
            TestProperties("acceptance/oam_dma/sources-GS.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_serial,
        MooneyeTest,
        ::testing::Values(
            //TestProperties("acceptance/interrupts/boot_sclk_align-dmgABCmgb.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance_ppu,
        MooneyeTest,
        ::testing::Values(
            //TestProperties("acceptance/ppu/hblank_ly_scx_timing-GS.gb", { GROUP_G, GROUP_S }),
            //TestProperties("acceptance/ppu/intr_1_2_timing-GS.gb", { GROUP_G, GROUP_S }),
            //TestProperties("acceptance/ppu/intr_2_0_timing.gb"),
            TestProperties("acceptance/ppu/intr_2_mode0_timing.gb"),
            //TestProperties("acceptance/ppu/intr_2_mode0_timing_sprites.gb"),
            TestProperties("acceptance/ppu/intr_2_mode3_timing.gb")
            //TestProperties("acceptance/ppu/intr_2_oam_ok_timing.gb"),
            //TestProperties("acceptance/ppu/lcdon_timing-GS.gb", { GROUP_G, GROUP_S }),
            //TestProperties("acceptance/ppu/lcdon_write_timing-GS.gb", { GROUP_G, GROUP_S }),
            //TestProperties("acceptance/ppu/stat_irq_blocking.gb"),
            //TestProperties("acceptance/ppu/stat_lyc_onoff.gb"),
            //TestProperties("acceptance/ppu/vblank_stat_intr-GS.gb", { GROUP_G, GROUP_S })
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        acceptance,
        MooneyeTest,
        ::testing::Values(
            TestProperties("acceptance/add_sp_e_timing.gb"),

            // TODO These tests fail because executing the boot rom takes too long - probably because of PPU.
            // Because of this the value of DIV register is incorrect when the boot ROM finishes. 

            //TestProperties("acceptance/boot_div2-S.gb"),
            //TestProperties("acceptance/boot_div-dmg0.gb", {Model::DMG0}),
            //TestProperties("acceptance/boot_div-dmgABCmgb.gb", {Model::DMG, Model::MGB}),
            //TestProperties("acceptance/boot_div-S.gb", {Model::SGB, Model::SGB2}),
            //TestProperties("acceptance/boot_hwio-dmg0.gb", {Model::DMG0}}),
            //TestProperties("acceptance/boot_hwio-dmgABCmgb.gb", {Model::DMG, Model::MGB}}),
            //TestProperties("acceptance/boot_hwio-S.gb", {Model::SGB, Model::SGB2}}),
            TestProperties("acceptance/boot_regs-dmg0.gb", { Model::DMG0 }),
            TestProperties("acceptance/boot_regs-dmgABC.gb", { Model::DMG }),
            TestProperties("acceptance/boot_regs-mgb.gb", { Model::MGB }),
            TestProperties("acceptance/boot_regs-sgb.gb", { Model::SGB }),
            TestProperties("acceptance/boot_regs-sgb2.gb", { Model::SGB2 }),
            TestProperties("acceptance/call_cc_timing.gb"),
            TestProperties("acceptance/call_cc_timing2.gb"),
            TestProperties("acceptance/call_timing.gb"),
            TestProperties("acceptance/call_timing2.gb"),
            //TestProperties("acceptance/di-timing-GS.gb", {GROUP_G, GROUP_S}), // Requires accurate VBlank timing
            TestProperties("acceptance/div_timing.gb"),
            TestProperties("acceptance/ei_sequence.gb"),
            TestProperties("acceptance/ei_timing.gb"),
            TestProperties("acceptance/halt_ime0_ei.gb"),
            //TestProperties("acceptance/halt_ime0_nointr_timing.gb"),
            TestProperties("acceptance/halt_ime1_timing.gb"),
            //TestProperties("acceptance/halt_ime1_timing2-GS.gb", {GROUP_G, GROUP_S}),
            TestProperties("acceptance/if_ie_registers.gb"),
            TestProperties("acceptance/intr_timing.gb"),
            TestProperties("acceptance/jp_cc_timing.gb"),
            TestProperties("acceptance/jp_timing.gb"),
            TestProperties("acceptance/ld_hl_sp_e_timing.gb"),
            TestProperties("acceptance/oam_dma_start.gb"),
            TestProperties("acceptance/oam_dma_timing.gb"),
            TestProperties("acceptance/oam_dma_restart.gb"),
            TestProperties("acceptance/pop_timing.gb"),
            TestProperties("acceptance/push_timing.gb"),
            TestProperties("acceptance/rapid_di_ei.gb"),
            TestProperties("acceptance/ret_cc_timing.gb"),
            TestProperties("acceptance/ret_timing.gb"),
            TestProperties("acceptance/reti_intr_timing.gb"),
            TestProperties("acceptance/reti_timing.gb"),
            TestProperties("acceptance/rst_timing.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        emulator_only_mbc1,
        MooneyeTest,
        ::testing::Values(
            TestProperties("emulator-only/mbc1/bits_bank1.gb"),
            TestProperties("emulator-only/mbc1/bits_bank2.gb"),
            TestProperties("emulator-only/mbc1/bits_mode.gb"),
            TestProperties("emulator-only/mbc1/bits_ramg.gb"),
            //TestProperties("emulator-only/mbc1/multicart_rom_8Mb.gb", // This test requires a way of detecting multicart ROMs which is not implemente)d
            TestProperties("emulator-only/mbc1/ram_64kb.gb"),
            TestProperties("emulator-only/mbc1/ram_256kb.gb"),
            TestProperties("emulator-only/mbc1/rom_1Mb.gb"),
            TestProperties("emulator-only/mbc1/rom_2Mb.gb"),
            TestProperties("emulator-only/mbc1/rom_4Mb.gb"),
            TestProperties("emulator-only/mbc1/rom_8Mb.gb"),
            TestProperties("emulator-only/mbc1/rom_16Mb.gb"),
            TestProperties("emulator-only/mbc1/rom_512kb.gb")
        )
    );

    INSTANTIATE_TEST_SUITE_P(
        emulator_only_mbc5,
        MooneyeTest,
        ::testing::Values(
            TestProperties("emulator-only/mbc5/rom_1Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_2Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_4Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_8Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_16Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_32Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_64Mb.gb"),
            TestProperties("emulator-only/mbc5/rom_512kb.gb")
        )
    );
}