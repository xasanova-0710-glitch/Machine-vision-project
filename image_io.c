#include "image_io.h"
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION //преобразует stb_image.h в проргаммный код из хэдера, только в одном сишном файле, иначе ошибка
#include "stb_image.h"

Image* load_image(const char* filename) {
    int w, h, chan;
    unsigned char* data = stbi_load(filename, &w, &h, &chan, 0); //загружаем нужные данные, 0 - использовать исходное количество каналов

    if (data == NULL) {
        return NULL;
    }

    Image* data_true = malloc(sizeof(Image));

    if (data_true == NULL) {
        stbi_image_free(data);
        return NULL;
    }

    data_true->bites = data;
    data_true->width = w;
    data_true->height = h;
    data_true->channels = chan;

    return data_true;
}

int save_image(const char* filename, Image* img) {
    if (img == NULL || img->bytes == NULL) {
        return 0; 
    }

    int result = stbi_write_png(filename, img->width, img->height, img->channels, img->bites, img->width * img->channels);
    
    return result; // если получится, то 1, если нет, то 0
}

Image* create_image(int w, int h, int chan) {
    Image* img = malloc(sizeof(Image));
    if (img == NULL) {
        return NULL;
    }

    img->width = w;
    img->height = h;
    img->channels = chan;

    img->bytes = malloc(w * h * chan);

    if (img->bytes == NULL) {
        free(img);
        return NULL;
    }

    return img;
}

Image* convert_to_grayscale(const Image* img) {
    if (img == NULL || img->bytes == NULL) return NULL;

    Image* gray_img = create_image(img->width, img->height, 1);
    if (gray_img == NULL) return NULL;

    for (int i = 0; i < img->width * img->height; i++) {
        int idx = i * img->channels;

        unsigned char r = img->bytes[idx];
        unsigned char g = img->bytes[idx + 1];
        unsigned char b = img->bytes[idx + 2];

        unsigned char gray_value = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);

        gray_img->bytes[i] = gray_value;
    }

    return gray_img;
}
void free_image(Image* img) {
    if (img == NULL) {
        return;
    }

    if (img->bites != NULL) {
    stbi_image_free(img->bites); //удаление массива пикселей
    }

    free(img);
}
