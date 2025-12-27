// encoder.c
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
        fprintf(stderr, "Usage: encoder 0 in.bmp R.txt G.txt B.txt dim.txt\n");
        return 1;
    }

    FILE* f = fopen(argv[2], "rb");
    if (!f) die("open bmp failed");

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    fread(&bfh, sizeof(bfh), 1, f);
    fread(&bih, sizeof(bih), 1, f);

    if (bfh.bfType != 0x4D42 || bih.biBitCount != 24)
        die("only support 24-bit bmp");

    int w = bih.biWidth;
    int h = abs(bih.biHeight);

    size_t row_bytes = w * 3;
    size_t stride = (row_bytes + 3) & ~3;

    uint8_t *R = malloc(w * h);
    uint8_t *G = malloc(w * h);
    uint8_t *B = malloc(w * h);
    uint8_t *row = malloc(stride);
    if (!R || !G || !B || !row) die("malloc failed");

    fseek(f, bfh.bfOffBits, SEEK_SET);

    int bottom_up = (bih.biHeight > 0);

    for (int i = 0; i < h; i++) {
        int y = bottom_up ? (h - 1 - i) : i;
        fread(row, 1, stride, f);
        for (int x = 0; x < w; x++) {
            B[y*w + x] = row[x*3 + 0];
            G[y*w + x] = row[x*3 + 1];
            R[y*w + x] = row[x*3 + 2];
        }
    }

    fclose(f);
    free(row);

    /* write channels */
    FILE *fr = fopen(argv[3], "w");
    FILE *fg = fopen(argv[4], "w");
    FILE *fb = fopen(argv[5], "w");
    if (!fr || !fg || !fb) die("open channel output failed");

    for (int i = 0; i < w*h; i++) {
        fprintf(fr, "%u%s", R[i], (i%w==w-1)?"\n":" ");
        fprintf(fg, "%u%s", G[i], (i%w==w-1)?"\n":" ");
        fprintf(fb, "%u%s", B[i], (i%w==w-1)?"\n":" ");
    }

    fclose(fr); fclose(fg); fclose(fb);

    /* write dim.txt (ALL header info) */
    FILE* fd = fopen(argv[6], "w");
    if (!fd) die("open dim.txt failed");

    fprintf(fd,
        "%d %d\n"
        "%d\n"
        "%u\n"
        "%d\n"
        "%d\n"
        "%u\n"
        "%u\n",
        w, h,
        bih.biHeight,
        bih.biSizeImage,
        bih.biXPelsPerMeter,
        bih.biYPelsPerMeter,
        bih.biClrUsed,
        bih.biClrImportant
    );

    fclose(fd);

    free(R); free(G); free(B);
    return 0;
}
