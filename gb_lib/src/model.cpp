#include <gandalf/model.h>

namespace gandalf
{
    std::string GetModelName(Model model)
    {
        switch (model)
        {
        case Model::DMG0:
            return "Game Boy - Revision 0";
        case Model::DMG:
            return "Game Boy";
        case Model::MGB:
            return "Game Boy Pocket";
        case Model::SGB:
            return "Super Game Boy";
        case Model::SGB2:
            return "Super Game Boy 2";
        case Model::CGB0:
            return "Game Boy Color - Revision 0";
        case Model::CGB:
            return "Game Boy Color";
        default:
            return "Unknown";
        }
    }
} // namespace gandalf