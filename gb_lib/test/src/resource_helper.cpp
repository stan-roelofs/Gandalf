#include "resource_helper.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

ResourceHelper::ResourceHelper(const std::string& subfolder) : subfolder_(subfolder) {
#ifdef RESOURCE_PATH
    resource_path_ = RESOURCE_PATH;
#else 
    throw std::runtime_error("Test needs resources, but RESOURCE_PATH is not defined");
#endif
}

ResourceHelper::~ResourceHelper() = default;

std::vector<std::uint8_t> ResourceHelper::ReadFileBytes(const std::string& filename)
{
    const std::string file_path = resource_path_ + "/" + subfolder_ + "/" + filename;
    std::ifstream stream(file_path, std::ios::binary);
    if (stream.fail())
    {
        throw std::runtime_error("Could not open file " + file_path);
    }

    auto buffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>());
    return buffer;
}
