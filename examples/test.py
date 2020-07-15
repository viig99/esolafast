import sys
sys.path += ["build/lib.macosx-10.15-x86_64-3.8"]
from esolafast import time_stretch
import soundfile as sf

audio, samplerate = sf.read(INPUT_PATH, dtype='float32')
output_audio = time_stretch(audio, 1.2, samplerate)

sf.write(OUTPUT_PATH, output_audio, samplerate)