
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char R, G, B;
} RGBTriple;

typedef struct {
    int size;
    RGBTriple* table;
} RGBPalette;

typedef struct {
    int width, height;
    RGBTriple* pixels;
} RGBImage;

typedef struct {
    int width, height;
    unsigned char* pixels;
} PalettizedImage;

// #define compute_disperse(channel)                                              \
//     error = ((int)(currentPixel->channel)) - palette.table[index].channel;     \
//     image.pixels[(x + 1) + (y + 0) * image.width].channel += (error * 7) >> 4; \
//     image.pixels[(x - 1) + (y + 1) * image.width].channel += (error * 3) >> 4; \
//     image.pixels[(x + 0) + (y + 1) * image.width].channel += (error * 5) >> 4; \
//     image.pixels[(x + 1) + (y + 1) * image.width].channel += (error * 1) >> 4;

#define plus_truncate_uchar(a, b)                                              \
    if (((int)(a)) + (b) < 0)                                                  \
        (a) = 0;                                                               \
    else if (((int)(a)) + (b) > 255)                                           \
        (a) = 255;                                                             \
    else                                                                       \
        (a) += (b);

#define compute_disperse(channel)                                              \
    error = ((int)(currentPixel->channel)) - palette.table[index].channel;     \
    if (x + 1 < image.width) {                                                 \
        plus_truncate_uchar(                                                   \
            image.pixels[(x + 1) + (y + 0) * image.width].channel,             \
            (error * 7) >> 4                                                   \
        );                                                                     \
    }                                                                          \
    if (y + 1 < image.height) {                                                \
        if (x - 1 > 0) {                                                       \
            plus_truncate_uchar(                                               \
                image.pixels[(x - 1) + (y + 1) * image.width].channel,         \
                (error * 3) >> 4                                               \
            );                                                                 \
        }                                                                      \
        plus_truncate_uchar(                                                   \
            image.pixels[(x + 0) + (y + 1) * image.width].channel,             \
            (error * 5) >> 4                                                   \
        );                                                                     \
        if (x + 1 < image.width) {                                             \
            plus_truncate_uchar(                                               \
                image.pixels[(x + 1) + (y + 1) * image.width].channel,         \
                (error * 1) >> 4                                               \
            );                                                                 \
        }                                                                      \
    }

unsigned char FindNearestColor(RGBTriple color, RGBPalette palette) {

    int i, distanceSquared, minDistanceSquared, bestIndex = 0;

    minDistanceSquared = 255 * 255 + 255 * 255 + 255 * 255 + 1;

    for (i = 0; i < palette.size; i++) {

        int Rdiff = ((int)color.R) - palette.table[i].R;
        int Gdiff = ((int)color.G) - palette.table[i].G;
        int Bdiff = ((int)color.B) - palette.table[i].B;

        distanceSquared = Rdiff * Rdiff + Gdiff * Gdiff + Bdiff * Bdiff;

        if (distanceSquared < minDistanceSquared) {

            minDistanceSquared = distanceSquared;
            bestIndex          = i;
        }
    }
    return bestIndex;
}

PalettizedImage FloydSteinbergDither(RGBImage image, RGBPalette palette) {

    PalettizedImage result;
    result.width  = image.width;
    result.height = image.height;
    result.pixels = malloc(sizeof(unsigned char) * result.width * result.height);
    int x, y;
    for (y = 0; y < image.height; y++) {

        for (x = 0; x < image.width; x++) {

            RGBTriple* currentPixel = &(image.pixels[x + y * image.width]);

            unsigned char index = FindNearestColor(*currentPixel, palette);

            result.pixels[x + y * result.width] = index;

            {
                int error;
                compute_disperse(R);
                compute_disperse(G);
                compute_disperse(B);
            }
        }
    }

    return result;
}



int main(int argc, char* argv[]) {

    int             x;
    int             y;
    RGBImage        image;
    RGBPalette      palette;
    PalettizedImage result;
    FILE*           raw_in;
    FILE*           raw_out;

    image.width  = 100;
    image.height = 145;
    image.pixels = malloc(sizeof(RGBTriple) * image.width * image.height);

    raw_in = fopen(argv[1], "rb");

    for (y = 0; y < image.height; y++) {

        for (x = 0; x < image.width; x++) {

            fread(&image.pixels[x + y * image.width], 3, 1, raw_in);
        }
    }

    fclose(raw_in);

    palette.size        = 16;
    palette.table       = malloc(sizeof(RGBTriple) * palette.size);
    palette.table[0].R  = 149;
    palette.table[0].G  = 91;
    palette.table[0].B  = 110;
    palette.table[1].R  = 176;
    palette.table[1].G  = 116;
    palette.table[1].B  = 137;
    palette.table[2].R  = 17;
    palette.table[2].G  = 11;
    palette.table[2].B  = 15;
    palette.table[3].R  = 63;
    palette.table[3].G  = 47;
    palette.table[3].B  = 69;
    palette.table[4].R  = 93;
    palette.table[4].G  = 75;
    palette.table[4].B  = 112;
    palette.table[5].R  = 47;
    palette.table[5].G  = 62;
    palette.table[5].B  = 24;
    palette.table[6].R  = 76;
    palette.table[6].G  = 90;
    palette.table[6].B  = 55;
    palette.table[7].R  = 190;
    palette.table[7].G  = 212;
    palette.table[7].B  = 115;
    palette.table[8].R  = 160;
    palette.table[8].G  = 176;
    palette.table[8].B  = 87;
    palette.table[9].R  = 116;
    palette.table[9].G  = 120;
    palette.table[9].B  = 87;
    palette.table[10].R = 245;
    palette.table[10].G = 246;
    palette.table[10].B = 225;
    palette.table[11].R = 148;
    palette.table[11].G = 146;
    palette.table[11].B = 130;
    palette.table[12].R = 200;
    palette.table[12].G = 195;
    palette.table[12].B = 180;
    palette.table[13].R = 36;
    palette.table[13].G = 32;
    palette.table[13].B = 27;
    palette.table[14].R = 87;
    palette.table[14].G = 54;
    palette.table[14].B = 45;
    palette.table[15].R = 121;
    palette.table[15].G = 72;
    palette.table[15].B = 72;

    result = FloydSteinbergDither(image, palette);

    raw_out = fopen(argv[2], "wb");

    for(y = 0; y < result.height; y++) {

        for(x = 0; x < result.width; x++) {

            fwrite(&palette.table[result.pixels[x + y*image.width]], 3, 1, raw_out);
        }
    }

    fclose(raw_out);

    return 0;
}
