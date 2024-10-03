#include <iostream>
#include <vector>
#include <sndfile.h>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <complex>
#include <chrono>

#define WINDOW_SIZE 441 // Размер окна для FFT (10 миллисекунд для частоты дискретизации 44100 Гц)
#define STEP_SIZE 44    // Шаг окна (1 миллисекунда для частоты дискретизации 44100 Гц)

using Complex = std::complex<double>;
using ComplexArray = std::vector<Complex>;
using namespace std::chrono;

void fft(ComplexArray& x) {
    const size_t N = x.size();
    if (N <= 1) return;

    // Разделение на четные и нечетные элементы
    ComplexArray even(N / 2);
    ComplexArray odd(N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    // Рекурсивное вычисление FFT
    fft(even);
    fft(odd);

    // Объединение результатов
    for (size_t k = 0; k < N / 2; ++k) {
        Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

void compute_spectrogram(const std::vector<float>& data, int samplerate, int num_samples, const std::string& output_image) {
    int window_count = (num_samples - WINDOW_SIZE) / STEP_SIZE + 1;

    // Определение размеров спектрограммы
    const int width = window_count;
    const int height = WINDOW_SIZE / 2 + 1;
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    // Нормализация величин
    double max_magnitude = 0.0;

    // Первая итерация для нахождения максимального значения
    for (int i = 0; i < window_count; ++i) {
        ComplexArray in(WINDOW_SIZE);
        int offset = i * STEP_SIZE;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
            in[j] = Complex(data[offset + j], 0.0);
        }
        fft(in);

        for (int k = 0; k <= WINDOW_SIZE / 2; ++k) {
            double magnitude = std::abs(in[k]);
            if (magnitude > max_magnitude) {
                max_magnitude = magnitude;
            }
        }
    }

    // Вторая итерация для создания изображения
    for (int i = 0; i < window_count; ++i) {
        ComplexArray in(WINDOW_SIZE);
        int offset = i * STEP_SIZE;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
            in[j] = Complex(data[offset + j], 0.0);
        }
        fft(in);

        for (int k = 0; k <= WINDOW_SIZE / 2; ++k) {
            double magnitude = std::abs(in[k]);
            int intensity = static_cast<int>((magnitude / max_magnitude) * 255);
            sf::Color color(intensity, intensity, intensity);
            image.setPixel(i, height - 1 - k, color);
        }
    }

    // Сохранение изображения
    if (!image.saveToFile(output_image)) {
        throw std::runtime_error("Could not save image: " + output_image);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_wav_file> <output_image>" << std::endl;
        return 1;
    }

    // Загрузка аудиофайла
    SF_INFO sfinfo;
    SNDFILE *infile = sf_open(argv[1], SFM_READ, &sfinfo);
    if (!infile) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    int num_samples = sfinfo.frames * sfinfo.channels;
    std::vector<float> data(num_samples);
    sf_read_float(infile, data.data(), num_samples);
    sf_close(infile);

    try {
        auto start_time = high_resolution_clock::now();

        // Построение и сохранение спектрограммы
        compute_spectrogram(data, sfinfo.samplerate, num_samples, argv[2]);

        auto end_time = high_resolution_clock::now();
        duration<double> elapsed = end_time - start_time;

        std::cout << "Total time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "Spectrogram saved to " << argv[2] << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
