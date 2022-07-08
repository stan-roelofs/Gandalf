#ifndef __GANDALF_RESOURCE_HELPER_H
#define __GANDALF_RESOURCE_HELPER_H

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>

class ResourceHelper {
protected:
    ResourceHelper(const std::string& subfolder = "");
    virtual ~ResourceHelper();

    std::vector<std::uint8_t> ReadFileBytes(const std::string& filename);

private:
    std::string resource_path_;
    std::string subfolder_;
};

#endif // __GANDALF_RESOURCE_HELPER_H
