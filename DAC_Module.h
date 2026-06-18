#ifndef DAC_Module_h
#define DAC_Module_h

#include <Arduino.h>
#include "driver/dac_cosine.h"
#include "esp_err.h"

class DAC_Module {
    public:
        void Stop(int channel);
        void Setup(int channel, int clk_div, int frequency, int scale, int phase, int invert);
    private:
        dac_cosine_handle_t handles[2] = {nullptr, nullptr};
        int channelIndex(int channel);
        dac_channel_t normalizeChannel(int channel);
        dac_cosine_atten_t attenuationFromScale(int scale);
        dac_cosine_phase_t phaseFromInvert(int invert);
};

#endif

