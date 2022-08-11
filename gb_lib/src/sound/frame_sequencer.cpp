#include <gandalf/sound/frame_sequencer.h>

#include <gandalf/constants.h>

namespace
{
    const int kDivider = gandalf::kCPUFrequency / 512;
}

namespace gandalf
{
    // Values are initialized such that the first tick triggers a step to 0
    FrameSequencer::FrameSequencer() : counter_(kDivider), current_step_(7)
    {
    }

    FrameSequencer::~FrameSequencer() = default;


    void FrameSequencer::AddListener(std::shared_ptr<Listener> listener)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (listener->GetSteps()[i])
                listeners_[i].push_back(listener);
        }
    }

    void FrameSequencer::Tick()
    {
        ++counter_;
        if (counter_ < kDivider)
            return;

        counter_ = 0;
        current_step_ = (current_step_ + 1) % 8;

        for (auto it = listeners_[current_step_].begin(); it != listeners_[current_step_].end(); ++it) {
            if (it->expired())
                it = listeners_[current_step_].erase(it);
            else
                it->lock()->OnFrameSequencerStep();
        }
    }

} // namespace gandalf