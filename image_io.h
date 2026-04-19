#ifndef IMAGE_IO_H //чтобы не было повторного включения, если файл не определен - определи его
#define IMAGE_IO_H

typedef struct Image { //структура для хранения изображения, typedef, чтобы каждый раз не прописывать struct

    unsigned char* bites; //указатель на массив байтов, пиксели
    int width; //ширина
    int height; //высота
    int channels; //количество каналов
} Image;

Image* load_image(const char* filename);
int save_image(const Image* img, const char* filename); //int чтобы проверить на успех операции, 1 - да, 0 - нет
Image* convert_to_grayscale(const Image* img);
void free_image(Image* img); //не const, так как const - обещание функции, что она не будет изменять данные

#endif