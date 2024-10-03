import numpy as np
import matplotlib.pyplot as plt
from scipy.io.wavfile import write

# Параметры звука
samplerate = 44100  # Частота дискретизации, Гц
duration = 2.0     # Продолжительность, сек

# Генерация временной оси
t = np.linspace(0., duration, int(samplerate * duration))

# Генерация синусоидальных волн с разными частотами
frequencies = [440.0, 880.0, 1760.0]  # Частоты, Гц
amplitude = np.iinfo(np.int16).max
wave = np.zeros_like(t)

for freq in frequencies:
    wave += amplitude * np.sin(2. * np.pi * freq * t)

# Добавление белого шума
noise = amplitude * np.random.normal(0, 0.05, wave.shape)
wave += noise

# Нормализация сигнала
wave = wave / np.max(np.abs(wave)) * amplitude

# Сохранение в файл
write('complex_wave.wav', samplerate, wave.astype(np.int16))

# Построение спектрограммы
plt.specgram(wave, NFFT=1024, Fs=samplerate, noverlap=512)
plt.title('Spectrogram of Complex Wave')
plt.xlabel('Time [s]')
plt.ylabel('Frequency [Hz]')
plt.colorbar(label='Intensity [dB]')

# Сохранение спектрограммы как изображение
plt.savefig('complex_spectrogram.png')

# Закрытие окна графика
plt.close()
