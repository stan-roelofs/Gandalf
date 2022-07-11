#ifndef __GANDALF_CARTRIDGE_ROMONLY_H
#define __GANDALF_CARTRIDGE_ROMONLY_H

#include <gandalf/types.h>
#include <gandalf/mbc.h>

namespace gandalf {
    class ROMOnly : public MBC {
    public:
        ROMOnly(const ROM& rom, std::size_t ram_banks);
        virtual ~ROMOnly();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
    };
}

#endif