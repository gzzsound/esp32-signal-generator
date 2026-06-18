#include "DAC_Module.h"

dac_channel_t DAC_Module::normalizeChannel(int channel) {
    if (channel == 2) {
        return DAC_CHAN_1;
    }
    return DAC_CHAN_0;
}

int DAC_Module::channelIndex(int channel) {
    dac_channel_t normalized = normalizeChannel(channel);
    return normalized == DAC_CHAN_1 ? 1 : 0;
}

dac_cosine_atten_t DAC_Module::attenuationFromScale(int scale) {
    switch (scale) {
        case 1: return DAC_COSINE_ATTEN_DB_6;
        case 2: return DAC_COSINE_ATTEN_DB_12;
        case 3: return DAC_COSINE_ATTEN_DB_18;
        default: return DAC_COSINE_ATTEN_DB_0;
    }
}

dac_cosine_phase_t DAC_Module::phaseFromInvert(int invert) {
    // The ESP-IDF 5 cosine driver supports phase inversion, not bit-pattern inversion.
    return (invert == 1 || invert == 3) ? DAC_COSINE_PHASE_180 : DAC_COSINE_PHASE_0;
}

void DAC_Module::Stop(int channel) {
    int index = channelIndex(channel);
    if (handles[index] == nullptr) {
        return;
    }

    dac_cosine_stop(handles[index]);
    dac_cosine_del_channel(handles[index]);
    handles[index] = nullptr;
}

void DAC_Module::Setup(int channel, int clk_div, int frequency, int scale, int phase, int invert) {
    dac_channel_t normalized = normalizeChannel(channel);
    int index = channelIndex(channel);

    Stop(channel);

    uint32_t output_frequency = max(130, (int)(frequency * (135.0 / (clk_div + 1.0)) + 0.5));
    int8_t offset = (int8_t)constrain(phase, 0, 127);

    dac_cosine_config_t config = {};
    config.chan_id = normalized;
    config.freq_hz = output_frequency;
    config.clk_src = DAC_COSINE_CLK_SRC_DEFAULT;
    config.atten = attenuationFromScale(scale);
    config.phase = phaseFromInvert(invert);
    config.offset = offset;
    config.flags.force_set_freq = true;

    esp_err_t err = dac_cosine_new_channel(&config, &handles[index]);
    if (err != ESP_OK) {
        Serial.printf("Failed to create DAC cosine channel: %s\n", esp_err_to_name(err));
        handles[index] = nullptr;
        return;
    }

    err = dac_cosine_start(handles[index]);
    if (err != ESP_OK) {
        Serial.printf("Failed to start DAC cosine channel: %s\n", esp_err_to_name(err));
        dac_cosine_del_channel(handles[index]);
        handles[index] = nullptr;
    }
}
