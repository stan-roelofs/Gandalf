#include <gandalf/joypad.h>

#include <cassert>

#include <gandalf/constants.h>

namespace
{
    struct ButtonProperties {
        gandalf::byte select_bit;
        gandalf::byte button_bit;
    };

    const ButtonProperties kButtonMatrix[8] = {
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {5, 0},
        {5, 1},
        {5, 2},
        {5, 3}
    };
}

namespace gandalf
{
    Joypad::Joypad(Memory& memory): Memory::AddressHandler("Joypad"), p1_(0xCF), memory_(memory)
    {
        pressed_buttons_.fill(false);
    }

    Joypad::~Joypad() = default;

    void Joypad::SetButtonState(Button button, bool pressed)
    {
        bool was_pressed = pressed_buttons_[button];
        pressed_buttons_[button] = pressed;
        Update();
        if (pressed && !was_pressed)
            memory_.Write(kIF, memory_.Read(kIF) | kJoypadInterruptMask);
    }

    byte Joypad::Read(word address) const
    {
        assert(address == kP1);
        (void)address;

        return p1_;
    }

    void Joypad::Write(word address, byte value)
    {
        assert(address == kP1);
        (void)address;

        p1_ = value & 0xF0;
        Update();
    }

    std::set<word> Joypad::GetAddresses() const
    {
        return { kP1 };
    }

    void Joypad::Update()
    {
        byte result = p1_ | 0xCF;
        const bool select_action_bit = p1_ & 0x20;
        const bool select_direction_bit = p1_ & 0x10;
        for (int i = 0; i < 8; ++i) {
            if (pressed_buttons_[i] && ((kButtonMatrix[i].select_bit == 4 && !select_direction_bit) || (kButtonMatrix[i].select_bit == 5 && !select_action_bit))) {
                result &= ~(1 << kButtonMatrix[i].button_bit);
            }
        }
        p1_ = result;
    }
} // namespace gandalf