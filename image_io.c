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

void free_image(Image* img) {
    if (img == NULL) {
        return;
    }

    if (img->bites != NULL) {
    stbi_image_free(img->bites); //удаление массива пикселей
    }

    free(img);
}