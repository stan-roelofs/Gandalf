#ifndef __GANDALF_ADDRESSRANGE_H
#define __GANDALF_ADDRESSRANGE_H

#include <set>
#include <string>

#include <gandalf/types.h>

namespace gandalf {
    class AddressRange
    {
    public:
        /**
         * Writes the value to the specified address.
         *
         * @param address address that will be written
         * @param value value that will be written
         */
        virtual void Write(word address, byte value) = 0;

        /**
         * Reads the value at the specified address.
         *
         * @param address the address that will be read.
         * @return Value
         */
        virtual byte Read(word address) const = 0;


        /// @return the addresses that are handled by this address range.         
        virtual std::set<word> GetAddresses() const = 0;

    protected:
        AddressRange(const std::string& name) : name_(name) {}

    private:
        std::string name_;
        std::set<word> addresses_;
    };
}

#endif