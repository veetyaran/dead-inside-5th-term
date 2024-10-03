import numpy as np
import matplotlib.pyplot as plt
from scipy.io.wavfile import write

# Параметры звука
samplerate = 44100  # Частота дискретизации, Гц
duration = 2.0     # Продолжительность, сек

# Генерация синусоидальной волны
frequency = 440.0  # Частота, Гц (A4 нота)
t = np.linspace(0., duration, int(samplerate * duration))
amplitude = np.iinfo(np.int16).max
wave = amplitude * np.sin(2. * np.pi * frequency * t)

# Сохранение в файл
write('test_sine_wave.wav', samplerate, wave.astype(np.int16))

# Построение спектрограммы
plt.specgram(wave, NFFT=1024, Fs=samplerate, noverlap=512)
plt.title('Spectrogram of Test Sine Wave')
plt.xlabel('Time [s]')
plt.ylabel('Frequency [Hz]')
plt.colorbar(label='Intensity [dB]')

# Сохранение спектрограммы как изображение
plt.savefig('spectrogram.png')

# Закрытие окна графика
plt.close()
