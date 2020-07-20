//
// Created by Arjun Variar on 13/07/20.
//

#include <kfr/io/audiofile.hpp>
#include <kfr/io.hpp>
#include <kfr/dsp.hpp>

using namespace kfr;

univector<int> extract_epoch_indices(std::shared_ptr<univector<f32>> audio, double sample_frequency) {
    const int window_length = int(0.015 * sample_frequency);
    const int audio_size = audio->size();

    univector<double> y2(audio_size, 0);
    univector<double> y3(audio_size, 0);
    univector<double> y(audio_size, 0);
    univector<int> epochs;
    double mean_val;
    double running_sum;

    double _x_0 = audio->at(0);
    double _x_1 = audio->at(1) - _x_0;
    double _y1_0 = _x_0;
    double _y1_1 = _x_1 + (2.0 * _y1_0);
    double x_i = 0;
    double y1_i = 0;
    y2[0] = _y1_0;
    y2[1] = _y1_1 + (2.0 * y2[0]);
    for (int i = 2; i < audio_size; i++) {
        x_i = audio->at(i) - audio->at(i-1);
        y1_i = x_i + (2.0 * _y1_1) - _y1_0;
        y2[i] = y1_i + (2.0 * y2[i - 1]) - y2[i - 2];
        _y1_0 = _y1_1;
        _y1_1 = y1_i;
    }

    // Third stage
    running_sum = sum(y2.slice(0, 2 * window_length + 1));
    mean_val = 0;
    for (int i = 0; i < audio_size; ++i) {
        if ((i - window_length < 0) || (i + window_length >= audio_size)) {
            mean_val = y2[i];
        } else if (i - window_length == 0) {
            mean_val = running_sum / (2 * window_length + 1);
        } else {
            running_sum -= y2[i - window_length - 1] - y2[i + window_length];
            mean_val = running_sum / (2 * window_length + 1);
        }
        y3[i] = y2[i] - mean_val;
    }

    // Fourth Stage
    running_sum = sum(y3.slice(0, 2 * window_length + 1));
    mean_val = 0;
    for (int i = 0; i < audio_size; ++i) {
        if ((i - window_length < 0) || (i + window_length >= audio_size)) {
            mean_val = y3[i];
        } else if (i - window_length == 0) {
            mean_val = running_sum / (2 * window_length + 1);
        } else {
            running_sum -= y3[i - window_length - 1] - y3[i + window_length];
            mean_val = running_sum / (2 * window_length + 1);
        }
        y[i] = y3[i] - mean_val;
    }

    // Last stage
    double last = y[0];
    double act;
    epochs.push_back(0);
    for (int i = 0; i < audio_size; ++i) {
        act = y[i];
        if (last < 0 and act > 0) {
            epochs.push_back(i);
        }
        last = act;
    }
    epochs.push_back(audio_size - 1);

    return epochs;
}

void time_stretch(std::shared_ptr<univector<f32>> audio, std::shared_ptr<univector<f32>> synthesized_wav, univector<int> epoch_indices, float time_change_factor, int number_of_epochs_in_frame) {
    int target_length = 0;
    int last_epoch_index = epoch_indices[0];
    int hop = 0;
    int buffer_increase = 0;
    const int epoch_size = epoch_indices.size();
    univector<f32> window_wav;

    for (int i = 0; i < epoch_size - number_of_epochs_in_frame; ++i) {
        hop = epoch_indices[i+1] - epoch_indices[i];
        if (target_length >= synthesized_wav->size()) {
            int frame_length = epoch_indices[i+number_of_epochs_in_frame] - epoch_indices[i] - 1;
            auto window = window_blackman<f32>(frame_length);
            auto wav_frame_i = audio->slice(epoch_indices[i], frame_length) * window;
            buffer_increase = int(wav_frame_i.size() - synthesized_wav->size() + last_epoch_index);
            if (buffer_increase > 0) {
                synthesized_wav->resize(synthesized_wav->size() + buffer_increase, 0);
                window_wav.resize(window_wav.size() + buffer_increase, 0);
            }
            synthesized_wav->slice(last_epoch_index, frame_length) += wav_frame_i;
            window_wav.slice(last_epoch_index, frame_length) += window;

            last_epoch_index += hop;
        }
        target_length += int(hop * time_change_factor);
    }

    *synthesized_wav /= max(window_wav, univector<float>(window_wav.size(), 1e-4));
}

void esola(std::shared_ptr<univector<f32>> input_audio, std::shared_ptr<univector<f32>> output_audio, float time_change_factor, int number_of_epochs_in_frame, double sample_frequency) {
    auto epoch_indices = extract_epoch_indices(input_audio, sample_frequency);
    time_stretch(input_audio, output_audio, epoch_indices, time_change_factor, number_of_epochs_in_frame);
}

