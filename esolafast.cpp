#include "src/esola.cpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

using namespace std;
using namespace kfr;
namespace py = pybind11;

py::array_t<float> esola_main(py::array_t<float, py::array::c_style | py::array::forcecast> audio, float time_scaling_factor, float samplerate) {
    univector<f32> uni_audio = make_univector(audio.data(), audio.size());
    auto input_audio = std::make_shared<univector<f32>>(uni_audio);
    auto output_audio = std::make_shared<univector<f32>>();
    esola(input_audio, output_audio, float(1.0/time_scaling_factor), 3, samplerate);
    auto result        = py::array_t<float>(output_audio->size());
    auto result_buffer = result.request();
    int *result_ptr    = (int *) result_buffer.ptr;
    std::memcpy(result_ptr, output_audio->data(), output_audio->size()*sizeof(float));
    return result;
}

// wrap as Python module
PYBIND11_MODULE(esolafast, m)
{
    m.doc() = "Fast C++ implementation of ESOLA using KFRLib, can be used for online time-stretch augmentation during SpeechToText training.";
    m.def("time_stretch", &esola_main, py::arg("audio"), py::arg("time_scaling_factor"), py::arg("samplerate"), R"pbdoc(
        Takes 1d numpy array, time scaling factor & samplerate and outputs the generated audio.
        Algorithm is based on Epoch-Synchronous Overlap-Add.
    )pbdoc");
}
