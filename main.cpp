#include "src/esola.cpp"
#include <chrono>

using namespace std;
using namespace kfr;

univector<float> convert_audio_to_float(univector<int16_t> audio) {
    univector<float> float_audio;
    for (int i = 0; i < audio.size(); ++i) {
        float_audio.push_back(float(audio[i]) / 32768.0);
    }
    return float_audio;
}

int main() {
    std::string filename("/Users/vigi99/AirtelDEV/VoiceDataForAirtel/CommonCorpusTest/sample-010955.wav");
    audio_reader_wav<int16_t> reader(open_file_for_reading(filename));
    univector2d<int16_t> audio = reader.read_channels();
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto input_audio = convert_audio_to_float(audio[0]);
    auto output_audio = esola(input_audio, float(1.0/0.75), 3, reader.format().samplerate);
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    spdlog::info("Final Length = {}, ratio = {}", output_audio.size(),  float(output_audio.size()) / float(reader.format().length));

    audio_writer_wav<f32> writer(open_file_for_writing(KFR_FILEPATH("/Users/vigi99/Downloads/1_5_scope.wav")),
                                   audio_format{ 1, audio_sample_type::i16, reader.format().samplerate, false });
    writer.write(output_audio.data(), output_audio.size());
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    return 0;
}
