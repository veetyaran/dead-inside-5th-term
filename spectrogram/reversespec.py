import numpy as np
import matplotlib.pyplot as plt

def read_spectrogram_data(file_path):
    # Чтение данных из файла
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    # Пропустить заголовок
    lines = lines[1:]
    
    # Разбор данных
    data = []
    for line in lines:
        parts = line.strip().split()
        time_window = int(parts[0])
        frequency_bin = int(parts[1])
        magnitude = float(parts[2])
        data.append((time_window, frequency_bin, magnitude))
    
    # Определение размера спектрограммы
    max_time_window = max(d[0] for d in data) + 1
    max_frequency_bin = max(d[1] for d in data) + 1
    
    # Создание массива спектрограммы
    spectrogram = np.zeros((max_time_window, max_frequency_bin))
    
    for time_window, frequency_bin, magnitude in data:
        spectrogram[time_window, frequency_bin] = magnitude
    
    return spectrogram

def plot_spectrogram(spectrogram, output_image):
    plt.figure(figsize=(10, 6))
    plt.imshow(spectrogram.T, aspect='auto', origin='lower', cmap='viridis')
    plt.colorbar(label='Magnitude')
    plt.xlabel('Time Window')
    plt.ylabel('Frequency Bin')
    plt.title('Spectrogram')
    plt.savefig(output_image)
    plt.show()

# Чтение данных из файла result.txt
spectrogram = read_spectrogram_data('result.txt')

# Построение спектрограммы и сохранение как изображение
plot_spectrogram(spectrogram, 'reversespectrogram.png')
