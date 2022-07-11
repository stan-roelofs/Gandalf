#ifndef __GANDALF_RESOURCE_HELPER_H
#define __GANDALF_RESOURCE_HELPER_H

#include <string>
#include <iostream>
#include <cstdint>
#include <vector>

class ResourceHelper {
protected:
    ResourceHelper();
    virtual ~ResourceHelper();

    /**
     * Reads an entire file into a vector of bytes.
     *
     * @param path The path including the filename (relative to the resources folder).
     * @param buffer [out] the vector to store the bytes in. The buffer will always be cleared.
     * @return true if the file was read successfully, false otherwise.
    */
    bool ReadFileBytes(const std::string& path, std::vector<std::uint8_t>& buffer);

private:
    std::string resource_path_;
};

#endif // __GANDALF_RESOURCE_HELPER_H
