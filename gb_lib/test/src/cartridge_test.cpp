#include <gtest/gtest.h>

#include <gandalf/cartridge.h>
#include <array>

namespace gandalf {
    class CartridgeTest : public ::testing::Test {
    public:
        CartridgeTest() {
            bytes_.resize(0x150);
        }

        virtual ~CartridgeTest() = default;

    protected:
        Cartridge cartridge_;
        std::vector<byte> bytes_;
    };

    TEST_F(CartridgeTest, read_logo)
    {
        std::array<byte, 0x30> logo_bytes;
        for (byte i = 0; i < 0x30; i++) {
            logo_bytes[i] = i;
        }

        std::copy(logo_bytes.begin(), logo_bytes.end(), bytes_.begin() + 0x104);

        EXPECT_TRUE(cartridge_.Load(bytes_));

        for (int i = 0; i < 0x30; i++) {
            EXPECT_EQ(logo_bytes[i], cartridge_.GetHeader()->logo[i]);
        }
    }

    TEST_F(CartridgeTest, read_title)
    {
        const std::string title("Gandalf");
        const std::vector<byte> title_bytes(title.begin(), title.end());
        std::copy(title_bytes.begin(), title_bytes.end(), bytes_.begin() + 0x134);

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_STREQ(title.c_str(), reinterpret_cast<const char*>(cartridge_.GetHeader().value().title));
    }

    TEST_F(CartridgeTest, read_manufacturer_code)
    {
        const std::string manufacturer_code("TEST");
        const std::vector<byte> manufacturer_code_bytes(manufacturer_code.begin(), manufacturer_code.end());
        std::copy(manufacturer_code_bytes.begin(), manufacturer_code_bytes.end(), bytes_.begin() + 0x13F);

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_STREQ(manufacturer_code.c_str(), reinterpret_cast<const char*>(cartridge_.GetHeader().value().manufacturer_code));
    }

    TEST_F(CartridgeTest, read_cgb_flag)
    {
        const byte cgb_flag = 0x80;
        bytes_.at(0x143) = cgb_flag;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(cgb_flag, cartridge_.GetHeader().value().cgb_flag);
    }

    TEST_F(CartridgeTest, read_new_licensee_code)
    {
        bytes_[0x144] = 'G';
        bytes_[0x145] = 'B';

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ('G', cartridge_.GetHeader()->new_licensee_code[0]);
        EXPECT_EQ('B', cartridge_.GetHeader()->new_licensee_code[1]);
    }

    TEST_F(CartridgeTest, read_cartridge_type)
    {
        bytes_.at(0x147) = 0x12;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0x12, cartridge_.GetHeader().value().cartridge_type);
    }

    TEST_F(CartridgeTest, read_rom_size)
    {
        bytes_.at(0x148) = 0x08;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0x08, cartridge_.GetHeader().value().rom_size);
    }

    TEST_F(CartridgeTest, read_ram_size)
    {
        bytes_.at(0x149) = 0x05;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0x05, cartridge_.GetHeader().value().ram_size);
    }

    TEST_F(CartridgeTest, read_destination_code)
    {
        bytes_.at(0x14A) = 0x01;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0x01, cartridge_.GetHeader().value().destination_code);
    }

    TEST_F(CartridgeTest, read_old_licensee_code)
    {
        bytes_.at(0x14B) = 0xFE;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0xFE, cartridge_.GetHeader()->old_licensee_code);
    }

    TEST_F(CartridgeTest, read_mask_rom_version)
    {
        bytes_.at(0x14C) = 0x01;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0x01, cartridge_.GetHeader()->mask_rom_version);
    }

    TEST_F(CartridgeTest, read_header_checksum)
    {
        bytes_.at(0x14D) = 0xC0;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0xC0, cartridge_.GetHeader()->header_checksum);
    }

    TEST_F(CartridgeTest, read_global_checksum)
    {
        bytes_.at(0x14E) = 0xC0;
        bytes_.at(0x14F) = 0xCC;

        EXPECT_TRUE(cartridge_.Load(bytes_));

        EXPECT_EQ(0xC0, cartridge_.GetHeader()->global_checksum[0]);
        EXPECT_EQ(0xCC, cartridge_.GetHeader()->global_checksum[1]);
    }

    TEST_F(CartridgeTest, load_fails_invalid_size)
    {
        bytes_.resize(0x149);
        EXPECT_FALSE(cartridge_.Load(bytes_));
    }

    TEST_F(CartridgeTest, header_null_when_load_fails)
    {
        bytes_.resize(0x149);
        EXPECT_FALSE(cartridge_.Load(bytes_));
        EXPECT_EQ(std::nullopt, cartridge_.GetHeader());
    }
};