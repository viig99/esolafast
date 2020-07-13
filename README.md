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


## Reference
[Epoch-Synchronous Overlap-Add (ESOLA) for Time- and Pitch-Scale Modification of Speech Signals
](https://arxiv.org/abs/1801.06492)