#include <iostream>
#include <vector>
#include <sndfile.h>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <complex>
#include <pthread.h>
#include <chrono>

#define WINDOW_SIZE 441 // Размер окна для FFT (10 миллисекунд для частоты дискретизации 44100 Гц)
#define STEP_SIZE 44    // Шаг окна (1 миллисекунда для частоты дискретизации 44100 Гц)

using Complex = std::complex<double>;
using ComplexArray = std::vector<Complex>;
using namespace std::chrono;

// Структура для хранения данных, передаваемых в потоки
struct ThreadData {
    const std::vector<float>* data; // Указатель на вектор данных аудио
    int samplerate; // Частота дискретизации аудио
    int start_sample; // Начальный образец для текущего сегмента
    int num_samples; // Общее количество образцов
    int segment_index; // Индекс текущего сегмента
    int num_segments; // Общее количество сегментов
    sf::Image* image; // Указатель на объект изображения спектрограммы
    double* max_magnitude; // Указатель на максимальную величину спектра
    pthread_mutex_t* max_magnitude_mutex; // Мьютекс для защиты доступа к max_magnitude
    double* segment_times; // Указатель на массив времени выполнения сегментов
};

// Функция для выполнения быстрого преобразования Фурье (FFT)
void fft(ComplexArray& x) {
    const size_t N = x.size();
    if (N <= 1) return; // Базовый случай рекурсии

    // Разделение на четные и нечетные элементы
    ComplexArray even(N / 2);
    ComplexArray odd(N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    // Рекурсивное выполнение FFT для четных и нечетных частей
    fft(even);
    fft(odd);

    // Объединение результатов
    for (size_t k = 0; k < N / 2; ++k) {
        Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }
}

// Функция для вычисления спектра и создания сегмента изображения спектрограммы
void* compute_segment(void* arg) {
    // Запуск замера времени выполнения сегмента
    auto start_time = high_resolution_clock::now();

    // Извлечение данных из структуры ThreadData
    ThreadData* thread_data = (ThreadData*)arg;
    const std::vector<float>& data = *(thread_data->data);
    int num_samples = thread_data->num_samples;
    int segment_index = thread_data->segment_index;
    int num_segments = thread_data->num_segments;
    sf::Image& image = *(thread_data->image);
    double* max_magnitude = thread_data->max_magnitude;
    pthread_mutex_t* max_magnitude_mutex = thread_data->max_magnitude_mutex;

    // Вычисление количества окон и окон в каждом сегменте
    int window_count = (num_samples - WINDOW_SIZE) / STEP_SIZE + 1;
    int windows_per_segment = window_count / num_segments;
    int start_window = segment_index * windows_per_segment;
    int end_window = (segment_index == num_segments - 1) ? window_count : start_window + windows_per_segment;

    const int height = WINDOW_SIZE / 2 + 1;

    // Первая итерация для нахождения максимального значения
    for (int i = start_window; i < end_window; ++i) {
        ComplexArray in(WINDOW_SIZE);
        int offset = i * STEP_SIZE;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
            in[j] = Complex(data[offset + j], 0.0);
        }
        fft(in);

        // Нахождение максимального значения величины спектра
        for (int k = 0; k <= WINDOW_SIZE / 2; ++k) {
            double magnitude = std::abs(in[k]);
            pthread_mutex_lock(max_magnitude_mutex);
            if (magnitude > *max_magnitude) {
                *max_magnitude = magnitude;
            }
            pthread_mutex_unlock(max_magnitude_mutex);
        }
    }

    // Вторая итерация для создания изображения
    for (int i = start_window; i < end_window; ++i) {
        ComplexArray in(WINDOW_SIZE);
        int offset = i * STEP_SIZE;
        for (int j = 0; j < WINDOW_SIZE; ++j) {
            in[j] = Complex(data[offset + j], 0.0);
        }
        fft(in);

        // Заполнение изображения пикселями на основе величины спектра
        for (int k = 0; k <= WINDOW_SIZE / 2; ++k) {
            double magnitude = std::abs(in[k]);
            int intensity = static_cast<int>((magnitude / *max_magnitude) * 255);
            sf::Color color(intensity, intensity, intensity);
            image.setPixel(i, height - 1 - k, color);
        }
    }

    // Завершение замера времени выполнения сегмента
    auto end_time = high_resolution_clock::now();
    duration<double> elapsed = end_time - start_time;
    thread_data->segment_times[segment_index] = elapsed.count();

    return nullptr;
}

// Функция для вычисления спектрограммы с использованием многопоточности
void compute_spectrogram(const std::vector<float>& data, int samplerate, int num_samples, const std::string& output_image, int num_threads) {
    int window_count = (num_samples - WINDOW_SIZE) / STEP_SIZE + 1;
    const int width = window_count;
    const int height = WINDOW_SIZE / 2 + 1;

    // Создание объекта изображения для спектрограммы
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    // Переменная для хранения максимальной величины спектра
    double max_magnitude = 0.0;
    pthread_mutex_t max_magnitude_mutex = PTHREAD_MUTEX_INITIALIZER;

    // Создание векторов потоков и данных для потоков
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadData> thread_data(num_threads);
    std::vector<double> segment_times(num_threads, 0.0);

    int segment_size = num_samples / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        // Инициализация данных для каждого потока
        thread_data[i] = {&data, samplerate, i * segment_size, num_samples, i, num_threads, &image, &max_magnitude, &max_magnitude_mutex, segment_times.data()};
        // Создание потока
        pthread_create(&threads[i], nullptr, compute_segment, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        // Ожидание завершения выполнения потока
        pthread_join(threads[i], nullptr);
    }

    // Сохранение изображения спектрограммы
    if (!image.saveToFile(output_image)) {
        throw std::runtime_error("Could not save image: " + output_image);
    }

    // Вывод времени выполнения каждого сегмента
    for (int i = 0; i < num_threads; ++i) {
        std::cout << "Time taken by segment " << i << ": " << segment_times[i] << " seconds." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_wav_file.wav> <output_image.png> <num_threads>" << std::endl;
        return 1;
    }

    int num_threads = std::stoi(argv[3]);

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
        // Запуск замера общего времени выполнения программы
        auto start_time = high_resolution_clock::now();

        // Построение и сохранение спектрограммы
        compute_spectrogram(data, sfinfo.samplerate, num_samples, argv[2], num_threads);

        // Завершение замера общего времени выполнения программы
        auto end_time = high_resolution_clock::now();
        duration<double> elapsed = end_time - start_time;

        // Вывод общего времени выполнения программы
        std::cout << "Total time taken: " << elapsed.count() << " seconds." << std::endl;
        std::cout << "Spectrogram saved to " << argv[2] << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
