# Epoch-Synchronous Overlap-Add (ESOLA) 
Fast C++ implementation of ESOLA using KFRLib, can be used for online time-stretch augmentation during SpeechToText training.

## C++ Rewrite
Mostly an C++ re-write of https://github.com/BaronVladziu/ESOLA-Implementation to be used in Online SpeechToText training.

## Build
```$xslt
    git clone https://github.com/viig99/esolafast.git
    cd esolafast
    git submodule update --init --recursive
    mkdir build && cd build
    cmake ..
    make -j`nproc`
```

## Run
```$xslt
    ./esolafast -i INPUT_PATH -o OUTPUT_PATH -t 1.5
    ./esolafast --help
```

## Performance
Right now faster than sox & sound-stretch, CPU usage is much higher.
```$xslt
$ time sox sample_file.wav sox_1_5.wav tempo 1.5
sox sox_1_5.wav tempo 2.0  0.01s user 0.01s system 17% cpu 0.022 total

$ time soundstretch sample_file.wav st_1_5.wav -tempo=1.5
soundstretch st_1_5.wav -tempo=1.5  0.01s user 0.00s system 44% cpu 0.022 total

$ time esolafast -i sample_file.wav -o es_1_5.wav -t 1.5
esolafast -i  -o  -t 1.5  0.01s user 0.00s system 92% cpu 0.017 total
```

## Reference
[Epoch-Synchronous Overlap-Add (ESOLA) for Time- and Pitch-Scale Modification of Speech Signals
](https://arxiv.org/abs/1801.06492)