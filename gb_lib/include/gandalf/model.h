#ifndef __GANDALF_MODEL_H
#define __GANDALF_MODEL_H

#include <string>

namespace gandalf {
    enum class Model { DMG, MGB, SGB, SGB2, CGB, LAST };

    std::string GetModelName(Model model);
} // namespace gandalf
#endif