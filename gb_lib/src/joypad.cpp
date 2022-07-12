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
    Joypad::Joypad() : Bus::AddressHandler("Joypad"), p1_(0xCF)
    {
        pressed_buttons_.fill(false);
    }

    Joypad::~Joypad() = default;

    void Joypad::ButtonPressed(Button button)
    {
        pressed_buttons_[button] = true;
        Update();
    }

    void Joypad::ButtonReleased(Button button)
    {
        pressed_buttons_[button] = false;
        Update();
    }

    byte Joypad::Read(word address) const
    {
        assert(address == kP1);
        (void)address;

        return p1_ | 0xC0;
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
        const bool select_action = p1_ & (1 << 5);
        const bool select_direction = p1_ & (1 << 4);
        for (int i = 0; i < 8; ++i) {
            if (pressed_buttons_[i] && ((kButtonMatrix[i].select_bit == 4 && select_direction) || (kButtonMatrix[i].select_bit == 5 && select_action))) {
                result &= ~(1 << kButtonMatrix[i].button_bit);
            }
        }
        p1_ = result;
    }
} // namespace gandalf