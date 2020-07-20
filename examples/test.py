import sys
sys.path += ["build/lib.macosx-10.15-x86_64-3.8"]
from esolafast import time_stretch
import soundfile as sf
from time import process_time
import sox

rate = 1.5
input_path = ""
output_path = ""

audio, samplerate = sf.read(input_path, dtype='float32')
start = process_time()
output_audio = time_stretch(audio, rate, samplerate)
print(f"Time taken for esolafast was {(process_time()-start)*1000:.2f}ms")
sf.write(output_path, output_audio, samplerate)

tfm = sox.Transformer()
tfm.stretch(rate)
start = process_time()
array_out = tfm.build_array(input_filepath=input_path)
print(f"Time taken for sox was {(process_time()-start)*1000:.2f}ms")