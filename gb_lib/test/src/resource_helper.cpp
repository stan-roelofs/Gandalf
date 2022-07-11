#include "resource_helper.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

ResourceHelper::ResourceHelper() {
#ifdef RESOURCE_PATH
    resource_path_ = RESOURCE_PATH;
#endif
}

ResourceHelper::~ResourceHelper() = default;

bool ResourceHelper::ReadFileBytes(const std::string& path, std::vector<std::uint8_t>& buffer)
{
    buffer.clear();

    if (resource_path_.empty()) {
        std::cerr << "Resource path is not set!" << std::endl;
        return false;
    }
    const std::string file_path = resource_path_ + "/" + path;
    std::ifstream stream(file_path, std::ios::binary);
    if (stream.fail())
        return false;

    std::vector<std::uint8_t> file = std::vector<std::uint8_t>(std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>());

    buffer = file;
    return true;
}
