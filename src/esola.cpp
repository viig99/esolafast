//
// Created by Arjun Variar on 13/07/20.
//

#include <iostream>
#include <kfr/io/audiofile.hpp>
#include <kfr/io.hpp>
#include <kfr/dsp.hpp>
#include <kfr/math.hpp>

using namespace kfr;


univector<int> extract_epoch_indices(univector<f32> audio, double sample_frequency) {
    const int window_length = int(0.005 * sample_frequency);

    univector<double> x(audio.size(), 0);
    univector<double> y1(x.size(), 0);
    univector<double> y2(y1.size(), 0);
    univector<double> y3(y2.size(), 0);
    univector<double> y(y3.size(), 0);
    univector<int> epochs;
    double mean_val;

    // Preprocess
    x[0] = audio[0];
    for (int i = 1; i < audio.size(); ++i) {
        x[i] = audio[i] - audio[i-1];
    }
    x /= absmaxof(x);

    // First stage
    y1[0] = x[0];
    y1[1] = x[1] + (2.0*y1[0]);
    for (int i = 2; i < x.size(); ++i) {
        y1[i] = x[i] + (2.0*y1[i-1]) - y1[i-2];
    }
    y1 /= absmaxof(y1);

    // Second Stage
    y2[0] = y1[0];
    y2[1] = y1[1] + (2.0*y2[0]);
    for (int i = 2; i < y1.size(); ++i) {
        y2[i] = y1[i] + (2.0*y2[i-1]) - y2[i-2];
    }
    y2 /= absmaxof(y2);

    // Third stage
    mean_val = 0;
    for (int i = 0; i < y2.size(); ++i) {
        if ((i-window_length < 0) || (i+window_length >= y2.size())) {
            mean_val = y2[i];
        } else {
            mean_val = mean(y2.slice(i - window_length, 2 * window_length + 1));
        }
        y3[i] = y2[i] - mean_val;
    }
    y3 /= absmaxof(y3);

    mean_val = 0;
    for (int i = 0; i < y3.size(); ++i) {
        if ((i-window_length < 0) || (i+window_length >= y3.size())) {
            mean_val = y3[i];
        } else {
            mean_val = mean(y3.slice(i - window_length, 2 * window_length + 1));
        }
        y[i] = y3[i] - mean_val;
    }
    y /= absmaxof(y);

    double last = y[0];
    double act;
    epochs.push_back(0);
    for (int i = 0; i < y.size(); ++i) {
        act = y[i];
        if (last < 0 and act > 0) {
            epochs.push_back(i);
        }
        last = act;
    }
    epochs.push_back(audio.size() - 1);

    return epochs;
}

univector<f32> time_stretch(univector<f32> audio, univector<int> epoch_indices, float time_change_factor, int number_of_epochs_in_frame) {

    int target_length = 0;
    int last_epoch_index = epoch_indices[0];
    int hop = 0;
    int buffer_increase = 0;
    univector<f32> synthesized_wav;
    univector<f32> window_wav;

    for (int i = 0; i < epoch_indices.size() - number_of_epochs_in_frame; ++i) {
        hop = epoch_indices[i+1] - epoch_indices[i];
        while (target_length >= synthesized_wav.size()) {
            int frame_length = epoch_indices[i+number_of_epochs_in_frame] - epoch_indices[i] - 1;
            auto window = window_blackman<f32>(frame_length);
            auto wav_frame_i = audio.slice(epoch_indices[i], frame_length) * window;
            buffer_increase = int(wav_frame_i.size() - synthesized_wav.size() + last_epoch_index);
            if (buffer_increase > 0) {
                synthesized_wav.resize(synthesized_wav.size() + buffer_increase, 0);
                window_wav.resize(window_wav.size() + buffer_increase, 0);
            }
            synthesized_wav.slice(last_epoch_index, frame_length) += wav_frame_i;
            window_wav.slice(last_epoch_index, frame_length) += window;

            last_epoch_index += hop;
        }
        target_length += int(hop * time_change_factor);
    }

    synthesized_wav /= max(window_wav, univector<float>(window_wav.size(), 1e-4));
    return synthesized_wav;
}

univector<f32> esola(univector<f32> audio, float time_change_factor, int number_of_epochs_in_frame, double sample_frequency) {
    auto epoch_indices = extract_epoch_indices(audio, sample_frequency);
    auto stretched_wave = time_stretch(audio, epoch_indices, time_change_factor, number_of_epochs_in_frame);
    return stretched_wave;
}

