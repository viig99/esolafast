//
// Created by Arjun Variar on 13/07/20.
//

#include <kfr/io/audiofile.hpp>
#include <kfr/io.hpp>
#include <kfr/dsp.hpp>

using namespace kfr;

std::vector<int> extract_epoch_indices(std::shared_ptr<univector<f32>> audio, double sample_frequency) {
    const int window_length = int(0.015 * sample_frequency);
    const int audio_size = audio->size();

    std::vector<double> y2(audio_size);
    std::vector<double> y3(audio_size);
    std::vector<double> y(audio_size);
    std::vector<int> epochs;
    double mean_val;
    double running_sum;

    const double _x_0 = audio->at(0);
    const double _x_1 = audio->at(1) - _x_0;
    double _y1_0 = _x_0;
    double _y1_1 = _x_1 + (2.0 * _y1_0);
    double x_i = 0;
    double y1_i = 0;
    y2[0] = _y1_0;
    y2[1] = _y1_1 + (2.0 * y2[0]);
    for (int i = 2; i < audio_size; i++) {
        x_i = audio->at(i) - audio->at(i - 1);
        y1_i = x_i + (2.0 * _y1_1) - _y1_0;
        y2[i] = y1_i + (2.0 * y2[i - 1]) - y2[i - 2];
        _y1_0 = _y1_1;
        _y1_1 = y1_i;
    }

    // Third stage
    running_sum = std::accumulate(y2.begin(), y2.begin() + 2 * window_length + 2 , 0.0);
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
    running_sum = std::accumulate(y3.begin(), y3.begin() + 2 * window_length + 2 , 0.0);
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

    return std::move(epochs);
}

void time_stretch(std::shared_ptr<univector<f32>> audio, std::shared_ptr<univector<f32>> synthesized_wav, std::vector<int> epoch_indices, float time_change_factor, int number_of_epochs_in_frame) {
    int target_length = 0;
    int last_epoch_index = epoch_indices[0];
    const int epoch_size = epoch_indices.size();
    univector<f32> window_wav;

    for (int i = 0; i < epoch_size - number_of_epochs_in_frame; ++i) {
        const int hop = epoch_indices[i + 1] - epoch_indices[i];
        if (target_length >= synthesized_wav->size()) {
            const int frame_length = epoch_indices[i + number_of_epochs_in_frame] - epoch_indices[i] - 1;
            univector<f32> window = window_blackman<f32>(frame_length);
            univector<f32> wav_frame_i = audio->slice(epoch_indices[i], frame_length) * window;
            const int buffer_increase = int(wav_frame_i.size() - synthesized_wav->size() + last_epoch_index);
            if (buffer_increase > 0) {
                synthesized_wav->insert(synthesized_wav->end(), wav_frame_i.end() - buffer_increase,
                                        wav_frame_i.end());
                window_wav.insert(window_wav.end(), window.end() - buffer_increase, window.end());
            }
            synthesized_wav->slice(last_epoch_index, frame_length - buffer_increase) += wav_frame_i.slice(0,
                                                                                                          wav_frame_i.size() -
                                                                                                          buffer_increase);
            window_wav.slice(last_epoch_index, frame_length - buffer_increase) += window.slice(0, window.size() -
                                                                                                  buffer_increase);
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

