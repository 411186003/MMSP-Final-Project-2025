// decoder.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

static void die(const char* m) {
    fprintf(stderr, "ERROR: %s\n", m);
    exit(1);
}

int main(int argc, char** argv) {
    if (argc != 7) {
        fprintf(stderr, "Usage: decoder 0 out.bmp R.txt G.txt B.txt dim.txt\n");
        return 1;
    }

    int w, h;
    BITMAPINFOHEADER bih;

    FILE* fd = fopen(argv[6], "r");
    if (!fd) die("open dim.txt failed");

    fscanf(fd, "%d %d", &w, &h);
    fscanf(fd, "%d", &bih.biHeight);
    fscanf(fd, "%u", &bih.biSizeImage);
    fscanf(fd, "%d", &bih.biXPelsPerMeter);
    fscanf(fd, "%d", &bih.biYPelsPerMeter);
    fscanf(fd, "%u", &bih.biClrUsed);
    fscanf(fd, "%u", &bih.biClrImportant);
    fclose(fd);

    bih.biSize = 40;
    bih.biWidth = w;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;

    size_t row_bytes = w * 3;
    size_t stride = (row_bytes + 3) & ~3;

    uint8_t *R = malloc(w*h);
    uint8_t *G = malloc(w*h);
    uint8_t *B = malloc(w*h);
    if (!R || !G || !B) die("malloc failed");

    FILE *fr = fopen(argv[3], "r");
    FILE *fg = fopen(argv[4], "r");
    FILE *fb = fopen(argv[5], "r");
    if (!fr || !fg || !fb) die("open channel failed");

    for (int i = 0; i < w*h; i++) {
        fscanf(fr, "%hhu", &R[i]);
        fscanf(fg, "%hhu", &G[i]);
        fscanf(fb, "%hhu", &B[i]);
    }

    fclose(fr); fclose(fg); fclose(fb);

    FILE* fo = fopen(argv[2], "wb");
    if (!fo) die("open output bmp failed");

    BITMAPFILEHEADER bfh = {
        .bfType = 0x4D42,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
        .bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
                + stride * h,
        .bfReserved1 = 0,
        .bfReserved2 = 0
    };

    fwrite(&bfh, sizeof(bfh), 1, fo);
    fwrite(&bih, sizeof(bih), 1, fo);

    uint8_t* row = calloc(1, stride);
    int bottom_up = (bih.biHeight > 0);

    for (int i = 0; i < h; i++) {
        int y = bottom_up ? (h - 1 - i) : i;
        for (int x = 0; x < w; x++) {
            row[x*3+0] = B[y*w + x];
            row[x*3+1] = G[y*w + x];
            row[x*3+2] = R[y*w + x];
        }
        fwrite(row, 1, stride, fo);
    }

    fclose(fo);
    free(row);
    free(R); free(G); free(B);
    return 0;
}
