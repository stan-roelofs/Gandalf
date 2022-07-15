#ifndef __GANDALF_APU_H
#define __GANDALF_APU_H

#include "bus.h"

namespace gandalf
{
    class APU : public Bus::AddressHandler
    {
    public:
        APU();
        virtual ~APU();

        void Write(word address, byte value) override;
        byte Read(word address) const override;
        std::set<word> GetAddresses() const override;

        void Tick();

        class OutputHandler
        {
        public:
            virtual ~OutputHandler() = default;

            /** Play an audio sample
             * @param left the left channel sample
             * @param right the right channel sample
             */
            virtual void Play(byte left, byte right) = 0;
        };

        void SetOutputHandler(std::shared_ptr<OutputHandler> handler);

    private:
        std::shared_ptr<OutputHandler> output_handler_;
    };
} // namespace gandalf

#endif