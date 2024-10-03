#include <iostream>
#include <vector>
#include <cmath>
#include <sndfile.h>

#define SAMPLE_RATE 44100  // Частота дискретизации
#define DURATION 5         // Продолжительность в секундах
#define AMPLITUDE 0.5      // Амплитуда сигнала

void generate_step_wave(std::vector<float>& buffer, int sample_rate, int duration) {
    int num_samples = sample_rate * duration;
    int step_duration = sample_rate / 10;  // Продолжительность каждой ступени в 0.1 секунда
    float freq_start = 200.0f;  // Начальная частота
    float freq_step = 200.0f;   // Шаг частоты

    for (int i = 0; i < num_samples; ++i) {
        int step = i / step_duration;
        float freq = freq_start + step * freq_step;
        buffer[i] = AMPLITUDE * sin(2.0 * M_PI * freq * i / sample_rate);
    }
}

int main() {
    int num_samples = SAMPLE_RATE * DURATION;
    std::vector<float> buffer(num_samples);

    // Генерация сигнала "лесенкой"
    generate_step_wave(buffer, SAMPLE_RATE, DURATION);

    // Параметры файла .wav
    SF_INFO sfinfo;
    sfinfo.frames = num_samples;
    sfinfo.samplerate = SAMPLE_RATE;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    // Создание и запись файла .wav
    SNDFILE* outfile = sf_open("step_wave.wav", SFM_WRITE, &sfinfo);
    if (!outfile) {
        std::cerr << "Error: could not open file for writing: " << sf_strerror(NULL) << std::endl;
        return 1;
    }

    sf_write_float(outfile, buffer.data(), num_samples);
    sf_close(outfile);

    std::cout << "Generated step_wave.wav" << std::endl;
    return 0;
}
