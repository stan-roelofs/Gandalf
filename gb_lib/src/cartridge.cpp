#include <gandalf/cartridge.h>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <iostream>
#include <string>

namespace gandalf {
    class ROMOnly : public Cartridge::MBC {
    public:
        ROMOnly(const Cartridge::ROM& rom, word ram_banks) : Cartridge::MBC(rom, 2, ram_banks) {
            if (rom_.size() != 2)
                throw Exception("ROMOnly: Invalid ROM size");

            if (ram_.size() > 1)
                throw Exception("ROMOnly: Invalid RAM size");
        }
        byte Read(word address) const override {
            if (address <= 0x8000)
                return rom_[address / 0x4000][address % 0x4000];
            else if (ram_.size() > 0 && address > 0xA000 && address < 0xC000)
                return ram_[0][address % 0xA000];

            throw Exception("ROMOnly: Invalid read address");
        }

        void Write(word address, byte value) override {
            if (ram_.size() > 0 && address > 0xA000 && address < 0xC000) {
                ram_[0][address % 0xA000] = value;
                return;
            }

            throw Exception("ROMOnly: Invalid write address");
        }
    };

    std::string Cartridge::Header::GetTitle() const
    {
        return std::string(reinterpret_cast<const char*>(title), 0x10);
    }

    std::string Cartridge::Header::GetManufacturerCode() const
    {
        return std::string(reinterpret_cast<const char*>(manufacturer_code), 4);
    }

    std::string Cartridge::Header::GetDestination() const
    {
        return destination_code == 0x00 ? "Japan" : "Non-Japanese";
    }

    std::string Cartridge::Header::GetLicensee() const
    {
        if (old_licensee_code == 0x33) {
            std::string result = std::string(reinterpret_cast<const char*>(new_licensee_code), 2) + " - ";

            switch (((new_licensee_code[0] & 0xF) << 4) | (new_licensee_code[1] & 0xF)) {
            case 0x00: result += "None"; break;
            case 0x01: result += "Nintendo"; break;
            case 0x08: result += "Capcom"; break;
            case 0x09: result += "HOT-B"; break;
            case 0x0A: result += "Jaleco"; break;
            case 0x0B: result += "Coconuts"; break;
            case 0x0C: result += "Elite Systems"; break;
            case 0x13: result += "Electronic Arts"; break;
            case 0x18: result += "Hudson Soft"; break;
            case 0x19: result += "ITC Entertainment"; break;
            case 0x1A: result += "Yanoman"; break;
            case 0x1D: result += "Clary"; break;
            case 0x1F: result += "Virgin"; break;
            case 0x20: result += "KSS"; break;
            case 0x24: result += "PCM Complete"; break;
            case 0x25: result += "San-X"; break;
            case 0x28: result += "Kotobuki Systems"; break;
            case 0x29: result += "SETA"; break;
            case 0x30: result += "Infogrames"; break;
            case 0x31: result += "Nintendo"; break;
            case 0x32: result += "Bandai"; break;
            case 0x34: result += "Konami"; break;
            case 0x35: result += "Hector"; break;
            case 0x38: result += "Capcom"; break;
            case 0x39: result += "Banpresto"; break;
            case 0x3C: result += "Entertainment i(truncated)"; break;
            case 0x3E: result += "Gremlin"; break;
            case 0x41: result += "Ubisoft"; break;
            case 0x42: result += "Atlus"; break;
            case 0x44: result += "Malibu"; break;
            case 0x46: result += "Angel"; break;
            case 0x47: result += "Spectrum Holoby"; break;
            case 0x49: result += "Irem"; break;
            case 0x4A: result += "Virgin"; break;
            case 0x4D: result += "Malibu"; break;
            case 0x4F: result += "U.S. Gold"; break;
            case 0x50: result += "Absolute"; break;
            case 0x51: result += "Acclaim"; break;
            case 0x52: result += "Activision"; break;
            case 0x53: result += "American Sammy"; break;
            case 0x54: result += "GameTek"; break;
            case 0x55: result += "Park Place"; break;
            case 0x56: result += "LJN"; break;
            case 0x57: result += "Matchbox"; break;
            case 0x59: result += "Milton Bradley"; break;
            case 0x5A: result += "Mindscape"; break;
            case 0x5B: result += "Romstar"; break;
            case 0x5C: result += "Naxat Soft"; break;
            case 0x5D: result += "Tradewest"; break;
            case 0x60: result += "Titus"; break;
            case 0x61: result += "Virgin"; break;
            case 0x67: result += "Ocean"; break;
            case 0x69: result += "Electronic Arts"; break;
            case 0x6E: result += "Elite Systems"; break;
            case 0x6F: result += "Electro Brain"; break;
            case 0x70: result += "Infogrammes"; break;
            case 0x71: result += "Interplay"; break;
            case 0x72: result += "Brøderbund"; break;
            case 0x73: result += "Sculptered Soft"; break;
            case 0x75: result += "The Sales Curve"; break;
            case 0x78: result += "THQ"; break;
            case 0x79: result += "Accolade"; break;
            case 0x7A: result += "Triffix Entertainment"; break;
            case 0x7C: result += "Microprose"; break;
            case 0x7F: result += "Kemco"; break;
            case 0x80: result += "Misawa Entertainment"; break;
            case 0x83: result += "LOZC"; break;
            case 0x86: result += "Tokuma Shoten Intermedia"; break;
            case 0x8B: result += "Bullet-Proof Software"; break;
            case 0x8C: result += "Vic Tokai"; break;
            case 0x8E: result += "Ape"; break;
            case 0x8F: result += "I'Max"; break;
            case 0x91: result += "Chun Soft"; break;
            case 0x92: result += "Video System"; break;
            case 0x93: result += "Tsuburava"; break;
            case 0x95: result += "Varie"; break;
            case 0x96: result += "Yonezawa/S'Pal"; break;
            case 0x97: result += "Kaneko"; break;
            case 0x99: result += "Arc"; break;
            case 0x9A: result += "Nihon Bussan"; break;
            case 0x9B: result += "Tecmo"; break;
            case 0x9C: result += "Imagineer"; break;
            case 0x9D: result += "Banpresto"; break;
            case 0x9F: result += "Nova"; break;
            case 0xA1: result += "Hori Electric"; break;
            case 0xA2: result += "Bandai"; break;
            case 0xA4: result += "Konami"; break;
            case 0xA6: result += "Kawada"; break;
            case 0xA7: result += "Takara"; break;
            case 0xA9: result += "Technos Japan"; break;
            case 0xAA: result += "Brøderbund"; break;
            case 0xAC: result += "Toei Animation"; break;
            case 0xAD: result += "Toho"; break;
            case 0xAF: result += "Namco"; break;
            case 0xB0: result += "Acclaim"; break;
            case 0xB1: result += "Ascii/Nexoft"; break;
            case 0xB2: result += "Bandai"; break;
            case 0xB4: result += "Enix"; break;
            case 0xB6: result += "HAL"; break;
            case 0xB7: result += "SNK"; break;
            case 0xB9: result += "pony Canyon"; break;
            case 0xBA: result += "culture brain o(truncated)"; break;
            case 0xBB: result += "Sunsoft"; break;
            case 0xBD: result += "Sony Imagesoft"; break;
            case 0xBF: result += "Sammy"; break;
            case 0xC0: result += "Taito"; break;
            case 0xC2: result += "Kemco"; break;
            case 0xC3: result += "Squaresoft"; break;
            case 0xC4: result += "Tokuma Shoten Intermedia"; break;
            case 0xC5: result += "Data East"; break;
            case 0xC6: result += "Tonkin House"; break;
            case 0xC8: result += "Koei"; break;
            case 0xC9: result += "UFL"; break;
            case 0xCA: result += "Eltra"; break;
            case 0xCB: result += "VAP"; break;
            case 0xCC: result += "USE"; break;
            case 0xCD: result += "Meldac"; break;
            case 0xCE: result += "Pony Canyon"; break;
            case 0xCF: result += "Angel"; break;
            case 0xD0: result += "Taito"; break;
            case 0xD1: result += "Sofel"; break;
            case 0xD2: result += "Quest"; break;
            case 0xD3: result += "Sigma Enterprises"; break;
            case 0xD4: result += "Ask Kodansha"; break;
            case 0xD6: result += "Naxat Soft"; break;
            case 0xD7: result += "Copya Systems"; break;
            case 0xD9: result += "Banpresto"; break;
            case 0xDA: result += "Tomy"; break;
            case 0xDB: result += "LJN"; break;
            case 0xDD: result += "NCS"; break;
            case 0xDE: result += "Human"; break;
            case 0xDF: result += "Altron"; break;
            case 0xE0: result += "Jaleco"; break;
            case 0xE1: result += "Towachiki"; break;
            case 0xE2: result += "Uutaka"; break;
            case 0xE3: result += "Varie"; break;
            case 0xE5: result += "Epoch"; break;
            case 0xE7: result += "Athena"; break;
            case 0xE8: result += "Asmik"; break;
            case 0xE9: result += "Natsume"; break;
            case 0xEA: result += "King Records"; break;
            case 0xEB: result += "Atlus"; break;
            case 0xEC: result += "Epic/Sony Secords"; break;
            case 0xEE: result += "IGS"; break;
            case 0xF0: result += "A Wave"; break;
            case 0xF3: result += "Extreme Entertainment"; break;
            case 0xFF: result += "LJN"; break;
            default: result += "Unknown"; break;
            }
            return result;
        }

        switch (old_licensee_code)
        {
        case 0x00: return "None";
        case 0x01: return "Nintendo";
        case 0x04: return "Konami";
        case 0x08: return "Capcom";
        case 0x13: return "Electronic arts";
        case 0x18: return "Hudsonsoft";
        case 0x19: return "B-ai";
        case 0x20: return "Kss";
        case 0x22: return "Pow";
        case 0x24: return "Pcm complete";
        case 0x25: return "San-x";
        case 0x28: return "Kemco japan";
        case 0x29: return "Seta";
        case 0x30: return "Viacom";
        case 0x31: return "Nintendo";
        case 0x32: return "Bandia";
        case 0x33: return "Ocean/acclaim";
        case 0x34: return "Konami";
        case 0x35: return "Hector";
        case 0x37: return "Taito";
        case 0x38: return "Hudson";
        case 0x39: return "banpresto";
        case 0x41: return "Ubi soft";
        case 0x42: return "Atlus";
        case 0x44: return "Malibu";
        case 0x46: return "Angel";
        case 0x47: return "Bullet-Proof";
        case 0x49: return "Irem";
        case 0x50: return "Absolute";
        case 0x51: return "Acclaim";
        case 0x52: return "Activision";
        case 0x53: return "American sammy";
        case 0x54: return "Konami";
        case 0x55: return "Hi tech entertainment";
        case 0x56: return "Ljn";
        case 0x57: return "Matchbox";
        case 0x58: return "Mattel";
        case 0x59: return "Milton bradley";
        case 0x60: return "Titus";
        case 0x61: return "Virgin";
        case 0x64: return "Lucasarts";
        case 0x67: return "Ocean";
        case 0x69: return "Electronic arts";
        case 0x70: return "Infogrames";
        case 0x71: return "Interplay";
        case 0x72: return "Broderbund";
        case 0x73: return "Sculptured";
        case 0x75: return "Sci";
        case 0x78: return "T*hq";
        case 0x79: return "Accolade";
        case 0x80: return "Misawa";
        case 0x83: return "Lozc";
        case 0x86: return "Tokuma shoten i*";
        case 0x87: return "Tsukuda ori*";
        case 0x91: return "Chun soft";
        case 0x92: return "Video system";
        case 0x93: return "Ocean/acclaim";
        case 0x95: return "Varie";
        case 0x96: return "Yonezawa/s'pal";
        case 0x97: return "Kaneko";
        case 0x99: return "Pack in soft";
        }
        return "Unknown";
    }

    std::string Cartridge::Header::GetType() const
    {
        switch (cartridge_type)
        {
        case 0x00: return "ROM only";
        case 0x01: return "MBC1";
        case 0x02: return "MBC1 + RAM";
        case 0x03: return "MBC1 + RAM + BATTERY";
        case 0x05: return "MBC2";
        case 0x06: return "MBC2 + BATTERY";
        case 0x08: return "ROM + RAM";
        case 0x09: return "ROM + RAM + BATTERY";
        case 0x0B: return "MMM01";
        case 0x0C: return "MMM01 + RAM";
        case 0x0D: return "MMM01 + RAM + BATTERY";
        case 0x0F: return "MBC3 + TIMER + BATTERY";
        case 0x10: return "MBC3 + TIMER + RAM + BATTERY";
        case 0x11: return "MBC3";
        case 0x12: return "MBC3 + RAM";
        case 0x13: return "MBC3 + RAM + BATTERY";
        case 0x15: return "MBC4";
        case 0x16: return "MBC4 + RAM";
        case 0x17: return "MBC4 + RAM + BATTERY";
        case 0x19: return "MBC5";
        case 0x1A: return "MBC5 + RAM";
        case 0x1B: return "MBC5 + RAM + BATTERY";
        case 0x1C: return "MBC5 + RUMBLE";
        case 0x1D: return "MBC5 + RUMBLE + RAM";
        case 0x1E: return "MBC5 + RUMBLE + RAM + BATTERY";
        case 0x22: return "MBC7";
        case 0xFC: return "POCKET CAMERA";
        case 0xFD: return "BANDAI TAMA5";
        case 0xFE: return "HuC3";
        case 0xFF: return "HuC1 + RAM + BATTERY";
        };

        return "Unknown";
    }

    std::string Cartridge::Header::GetRAMSize() const
    {
        switch (ram_size)
        {
        case 0x00: return "None";
        case 0x01: return "2 KB, no banking";
        case 0x02: return "8 KB, no banking";
        case 0x03: return "32 KB, 4 banks";
        case 0x04: return "128 KB, 16 banks";
        }

        return "Unknown";
    }

    std::string Cartridge::Header::GetROMSize() const
    {
        switch (ram_size)
        {
        case 0x00: return "32 KB, no banking";
        case 0x01: return "64 KB, 4 banks";
        case 0x02: return "128 KB, 8 banks";
        case 0x03: return "256 KB, 16 banks";
        case 0x04: return "512 KB, 32 banks";
        case 0x05: return "1 MB, 64 banks";
        case 0x06: return "2 MB, 128 banks";
        case 0x07: return "4 MB, 256 banks";
        case 0x52: return "1.1 MB, 72 banks";
        case 0x53: return "1.2 MB, 80 banks";
        case 0x54: return "1.5 MB, 96 banks";
        }

        return "Unknown";
    }

    std::string Cartridge::Header::GetCGBFlag() const
    {
        if (cgb_flag == 0x80)
            return "CGB + GB";
        else if (cgb_flag == 0xC0)
            return "CGB only";
        else
            return "None";
    }

    std::string Cartridge::Header::GetSGBFlag() const
    {
        if (sgb_flag == 0x03)
            return "SGB supported";
        else
            return "None";
    }

    Cartridge::Cartridge() : Bus::AddressHandler("Cartridge") {}

    Cartridge::~Cartridge() = default;

    bool Cartridge::Load(const ROM& bytes)
    {
        if (bytes.size() < 0x150) // The header is located at 0x100-0x14F, so bytes must be at least 0x150 bytes long.
            return false;

        Header result;
        std::copy(bytes.begin() + 0x104, bytes.begin() + 0x134, result.logo);
        std::copy(bytes.begin() + 0x134, bytes.begin() + 0x144, result.title);
        std::copy(bytes.begin() + 0x13F, bytes.begin() + 0x143, result.manufacturer_code);
        result.cgb_flag = bytes.at(0x143);
        std::copy(bytes.begin() + 0x144, bytes.begin() + 0x146, result.new_licensee_code);
        result.sgb_flag = bytes.at(0x146);
        result.cartridge_type = bytes.at(0x147);
        result.rom_size = bytes.at(0x148);
        result.ram_size = bytes.at(0x149);
        result.destination_code = bytes.at(0x14A);
        result.old_licensee_code = bytes.at(0x14B);
        result.mask_rom_version = bytes.at(0x14C);
        result.header_checksum = bytes.at(0x14D);
        std::copy(bytes.begin() + 0x14E, bytes.begin() + 0x150, result.global_checksum);
        header_ = result;

        switch (result.cartridge_type)
        {
        case 0x00: mbc_ = std::make_unique<ROMOnly>(bytes, result.ram_size); break;
        default: throw Exception("Cartridge type not implemented: " + result.GetType());
        }

        return true;
    }

    std::optional<Cartridge::Header> Cartridge::GetHeader() const
    {
        return header_;
    }

    void Cartridge::Write(word address, byte value)
    {
        if (!mbc_)
            throw Exception("No cartridge loaded");

        mbc_->Write(address, value);
    }

    byte Cartridge::Read(word address) const
    {
        if (!mbc_)
            throw Exception("No cartridge loaded");

        return mbc_->Read(address);
    }

    std::set<word> Cartridge::GetAddresses() const
    {
        std::set<word> result;
        for (word i = 0; i < 0x8000; ++i)
            result.insert(i);

        for (word i = 0xA000; i < 0xC000; ++i)
            result.insert(i);

        return result;
    }

    Cartridge::MBC::MBC(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks) {
        rom_.resize(rom_banks);
        ram_.resize(ram_banks);

        if (rom.size() != rom_banks * 0x4000)
            throw Exception("ROM is incorrect");

        for (size_t bank = 0; bank < rom_banks; ++bank)
        {
            std::copy(rom.begin() + bank * 0x4000, rom.begin() + (bank + 1) * 0x4000, rom_[bank].begin());
        }
    }
    Cartridge::MBC::~MBC() = default;
}