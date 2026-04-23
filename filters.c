#include "filters.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// ------------- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ -------------

// Безопасное получение пикселя с отражением на границах
static unsigned char get_pixel_reflect(const unsigned char *image,
                                        int width, int height,
                                        int x, int y) {
    // Отражение координат, если вышли за границу
    if (x < 0) x = -x - 1;
    if (x >= width) x = 2 * width - x - 1;
    if (y < 0) y = -y - 1;
    if (y >= height) y = 2 * height - y - 1;

    // Дополнительная защита на случай двойного отражения
    if (x < 0) x = 0;
    if (x >= width) x = width - 1;
    if (y < 0) y = 0;
    if (y >= height) y = height - 1;

    return image[y * width + x];
}

// Ограничение значения в диапазоне [0, 255]
static unsigned char clamp_to_byte(float value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)(value + 0.5f);  // округление до ближайшего
}

// Сравнение для qsort (медианный фильтр)
static int compare_uchar(const void *a, const void *b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}

// ------------- СВЁРТКА (НЕДЕЛЯ 1) -------------

FloatImage* convolve(const unsigned char *input, int width, int height,
                     const float *kernel, int kernel_size) {
    // Выделяем память под результат
    FloatImage *result = (FloatImage*)malloc(sizeof(FloatImage));
    if (!result) return NULL;

    result->width = width;
    result->height = height;
    result->data = (float*)malloc(width * height * sizeof(float));
    if (!result->data) {
        free(result);
        return NULL;
    }

    int half = kernel_size / 2;  // радиус ядра (1 для 3x3)

    // Проход по КАЖДОМУ пикселю выходного изображения
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0.0f;

            // Накладываем ядро: проход по всем элементам ядра
            for (int ky = 0; ky < kernel_size; ky++) {
                for (int kx = 0; kx < kernel_size; kx++) {
                    // Вычисляем координаты пикселя во входном изображении
                    int ix = x + kx - half;
                    int iy = y + ky - half;

                    // Получаем значение пикселя (с отражением на границах)
                    unsigned char pixel = get_pixel_reflect(input, width, height, ix, iy);

                    // Индекс в одномерном массиве ядра
                    float kernel_val = kernel[ky * kernel_size + kx];

                    sum += pixel * kernel_val;
                }
            }

            result->data[y * width + x] = sum;
        }
    }

    return result;
}

void free_float_image(FloatImage *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

// ------------- МЕДИАННЫЙ ФИЛЬТР (НЕДЕЛЯ 2) -------------

unsigned char* median_filter(const unsigned char *input, int width, int height,
                              int window_size) {
    unsigned char *output = (unsigned char*)malloc(width * height);
    if (!output) return NULL;

    int half = window_size / 2;
    int window_area = window_size * window_size;
    unsigned char *window = (unsigned char*)malloc(window_area);
    if (!window) {
        free(output);
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int count = 0;

            // Собираем все пиксели в окне
            for (int wy = -half; wy <= half; wy++) {
                for (int wx = -half; wx <= half; wx++) {
                    int ix = x + wx;
                    int iy = y + wy;

                    // Отражение на границах
                    unsigned char pixel = get_pixel_reflect(input, width, height, ix, iy);
                    window[count++] = pixel;
                }
            }

            // Сортируем и берём средний элемент
            qsort(window, window_area, sizeof(unsigned char), compare_uchar);
            output[y * width + x] = window[window_area / 2];
        }
    }

    free(window);
    return output;
}

// ------------- ФИЛЬТР ГАУССА (НЕДЕЛЯ 2) -------------

float* gaussian_kernel(int ksize, float sigma) {
    float *kernel = (float*)malloc(ksize * ksize * sizeof(float));
    if (!kernel) return NULL;

    int half = ksize / 2;
    float sum = 0.0f;

    // Формула Гаусса: G(x,y) = exp(-(x²+y²) / (2*sigma²))
    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {
            int idx = (y + half) * ksize + (x + half);
            float value = expf(-(float)(x*x + y*y) / (2.0f * sigma * sigma));
            kernel[idx] = value;
            sum += value;
        }
    }

    // Нормализация: сумма всех элементов ядра = 1
    for (int i = 0; i < ksize * ksize; i++) {
        kernel[i] /= sum;
    }

    return kernel;
}

unsigned char* gaussian_filter(const unsigned char *input, int width, int height,
                                int ksize, float sigma) {
    // Генерируем ядро Гаусса
    float *kernel = gaussian_kernel(ksize, sigma);
    if (!kernel) return NULL;

    // Выполняем свёртку
    FloatImage *temp = convolve(input, width, height, kernel, ksize);
    free(kernel);

    if (!temp) return NULL;

    // Преобразуем FloatImage обратно в unsigned char
    unsigned char *output = (unsigned char*)malloc(width * height);
    if (!output) {
        free_float_image(temp);
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        output[i] = clamp_to_byte(temp->data[i]);
    }

    free_float_image(temp);
    return output;
}

// ------------- ДЕТЕКТОР СОБЕЛЯ (НЕДЕЛЯ 2) -------------

unsigned char* sobel_edge_detection(const unsigned char *input, int width, int height,
                                     float threshold) {
    // Ядра Собеля
    float Gx[9] = {
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    };

    float Gy[9] = {
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    };

    // Свёртка с Gx
    FloatImage *grad_x = convolve(input, width, height, Gx, 3);
    if (!grad_x) return NULL;

    // Свёртка с Gy
    FloatImage *grad_y = convolve(input, width, height, Gy, 3);
    if (!grad_y) {
        free_float_image(grad_x);
        return NULL;
    }

    // Вычисляем магнитуду и применяем порог
    unsigned char *output = (unsigned char*)malloc(width * height);
    if (!output) {
        free_float_image(grad_x);
        free_float_image(grad_y);
        return NULL;
    }

    for (int i = 0; i < width * height; i++) {
        float magnitude = sqrtf(grad_x->data[i] * grad_x->data[i] +
                                grad_y->data[i] * grad_y->data[i]);

        // Пороговая обработка: если градиент > порога — белый (граница), иначе чёрный
        output[i] = (magnitude > threshold) ? 255 : 0;
    }

    free_float_image(grad_x);
    free_float_image(grad_y);
    return output;
}