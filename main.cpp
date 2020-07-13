#include "src/esola.cpp"
#include "src/argparse.h"

using namespace std;
using namespace kfr;
using namespace argparse;

int parse(ArgumentParser& parser, int argc, const char* argv[]) {
    parser.add_argument()
            .names({"-t", "--time_scale"})
            .required(true);
    parser.add_argument()
            .names({"-i", "--input_file"})
            .required(true);
    parser.add_argument()
            .names({"-o", "--output_file"})
            .required(true);
    parser.enable_help();
    auto err = parser.parse(argc, argv);
    if (err) {
        std::cout << err << std::endl;
        return -1;
    }
    if (parser.exists("help")) {
        parser.print_help();
        return 0;
    }
    return 1;
}

int main(int argc, const char* argv[]) {
    ArgumentParser parser("esola", "ESOLA C++");
    int code = parse(parser, argc, argv);
    if (code != 1) {
        return code;
    }
    auto input_filepath = parser.get<std::string>("input_file");
    auto output_filepath = parser.get<std::string>("output_file");
    auto time_scaling_factor = parser.get<float>("time_scale");

    audio_reader_wav<float> reader(open_file_for_reading(input_filepath));
    univector2d<float> audio = reader.read_channels();
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto input_audio = audio[0];
    auto output_audio = esola(input_audio, float(1.0/time_scaling_factor), 3, reader.format().samplerate);
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    audio_writer_wav<f32> writer(open_file_for_writing(KFR_FILEPATH(output_filepath)),
                                   audio_format{ 1, audio_sample_type::i16, reader.format().samplerate, false });
    writer.write(output_audio.data(), output_audio.size());

    std::cout << "File with " << time_scaling_factor << "x time-stretch created at " << output_filepath << std::endl;

    return 0;
}
