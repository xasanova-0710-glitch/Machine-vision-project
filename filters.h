#ifndef FILTERS_H
#define FILTERS_H

// Структура для дробного изображения (результат свёртки)
typedef struct {
    float *data;
    int width;
    int height;
} FloatImage;

// Универсальная свёртка (Неделя 1)
// Принимает: входной массив байтов, ширину, высоту,
//            ядро (одномерный массив), размер ядра (например 3 для 3x3)
// Возвращает: FloatImage с дробными значениями
FloatImage* convolve(const unsigned char *input, int width, int height,
                     const float *kernel, int kernel_size);

// Медианный фильтр (Неделя 2)
// Принимает: входной массив байтов, ширину, высоту, размер окна
// Возвращает: новый массив байтов (выделен malloc)
unsigned char* median_filter(const unsigned char *input, int width, int height,
                              int window_size);

// Генерация ядра Гаусса (вспомогательная)
// Принимает: размер ядра, сигму
// Возвращает: одномерный массив размера ksize*ksize (выделен malloc, нормализован)
float* gaussian_kernel(int ksize, float sigma);

// Фильтр Гаусса (Неделя 2)
// Принимает: входной массив байтов, ширину, высоту, размер ядра, сигму
// Возвращает: новый массив байтов (выделен malloc)
unsigned char* gaussian_filter(const unsigned char *input, int width, int height,
                                int ksize, float sigma);

// Детектор границ Собеля (Неделя 2)
// Принимает: входной массив байтов, ширину, высоту, порог
// Возвращает: новый массив байтов (выделен malloc, 255 — граница, 0 — не граница)
unsigned char* sobel_edge_detection(const unsigned char *input, int width, int height,
                                     float threshold);

// Освобождение FloatImage
void free_float_image(FloatImage *img);

#endif // FILTERS_H