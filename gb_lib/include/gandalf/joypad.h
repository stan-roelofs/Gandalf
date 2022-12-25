#ifndef __GANDALF_JOYPAD_H
#define __GANDALF_JOYPAD_H

#include <set>

#include <gandalf/memory.h>

namespace gandalf
{
    class Joypad : public Memory::AddressHandler
    {
    public:
        enum Button {
            kRight,
            kLeft,
            kUp,
            kDown,
            kA,
            kB,
            kSelect,
            kStart,
        };

        Joypad(Memory& memory);
        virtual ~Joypad();

        void SetButtonState(Button button, bool pressed);

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

    private:
        void Update();

        std::array<bool, 8> pressed_buttons_;
        byte p1_;
        Memory& memory_;
    };
}

#endif