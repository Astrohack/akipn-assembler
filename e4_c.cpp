#include <dos.h>
#include <stdio.h>
#include <iostream.h>;
#include <sys/types.h>
#include <string.h>;
#include <conio.h>;
#include <math.h>;

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define BUFFER_LEN (UINT)64000

typedef unsigned char  BYTE;
typedef unsigned int  WORD;
typedef unsigned int  UINT;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;

#define DOWN_ARROW 80
#define UP_ARROW 72
#define ESCAPE 27

struct BITMAPFILEHEADER
{
    UINT bfType; //Opis formatu pliku. Musi być ‘BM’.
    DWORD bfSize; //Rozmiar pliku BMP w bajtach.
    UINT bfReserved1; //Zarezerwowane. Musi być równe 0.
    UINT bfReserved2; //Zarezerwowane. Musi być równe 0.
    DWORD bfOffBits; //Przesunięcie w bajtach początku danych
};
struct BITMAPINFOHEADER
{
    DWORD biSize; //Rozmiar struktury BITMAPINFOHEADER.
    LONG biWidth; //Szerokość bitmapy w pikselach.
    LONG biHeight; //Wysokość bitmapy w pikselach.
    WORD biPlanes; //Ilość płaszczyzn. Musi być 1.
    WORD biBitCount; //Głębia kolorów w bitach na piksel.
    DWORD biCompression; //Rodzaj kompresji (0 – brak).
    DWORD biSizeImage; //Rozmiar obrazu w bajtach. Uwaga może być 0.
    LONG biXPelsPerMeter;//Rozdzielczość pozioma w pikselach na metr.
    LONG biYPelsPerMeter;//Rozdzielczość pionowa w pikselach na metr.
    DWORD biClrUsed; //Ilość używanych kolorów z palety.
    DWORD biClrImportant; //Ilość kolorów z palety niezbędnych do
};

struct RGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
};

struct IMAGE
{
    BYTE *p_buffer;
    BYTE *p_original_image;
    BYTE  p_treshold;
    float b_current_contrast;
};


BYTE *VGA = (BYTE *) 0xA0000000L;
RGBQUAD palette[256];

BYTE *original_image = new BYTE[BUFFER_LEN];

LONG average = 0;

void set_pixel(int x, int y, BYTE color) {    
    VGA[y * 320 + x] = color;
}

/*void set_mode(unsigned char mode)
{
	union REGS regs;	
	regs.h.ah = 0x00;
	regs.h.al = mode;
	int86(0x10, &regs, &regs);
}*/

void set_palette(BITMAPINFOHEADER *bmih) {
    outportb(0x03C8, 0); //rozpocznij ustawianie palety od koloru nr 0
    for (int a = 0; a < bmih->biClrUsed; a++) //ilość kolorów w palecie 8-bitowej
    {
        outp(0x03C9, palette[a].rgbRed * 0.24705); //skalowana składowa R
        outp(0x03C9, palette[a].rgbGreen * 0.24705); //skalowana składowa
        outp(0x03C9, palette[a].rgbBlue * 0.24705); //skalowana składowa B
    }
}

void init_image(IMAGE &img) {
    img.p_original_image = new BYTE[BUFFER_LEN];
    img.p_buffer = new BYTE[BUFFER_LEN];
    img.b_current_contrast = -1;
    img.p_treshold = 0;
}

void free_image_memory(IMAGE &img) {
    delete[] img.p_buffer;
    delete[] img.p_original_image;
}

void open_file(IMAGE &img) {
    FILE *bitmap_file; //Plik bitmapy
    BITMAPFILEHEADER bmfh; //nagłówek nr 1 bitmapy
    BITMAPINFOHEADER bmih; //nagłówek nr 2 bitmapy
    bitmap_file = fopen("lena.bmp", "rb");
    fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, bitmap_file);
    fread(&bmih, sizeof(BITMAPINFOHEADER), 1, bitmap_file);
    fread(&palette[0],  bmih.biClrUsed * sizeof(RGBQUAD), 1, bitmap_file);
    
    set_palette(&bmih);

    UINT len = bmih.biWidth * bmih.biHeight;
    BYTE *buff = new BYTE[len];


    fread(&buff[0], sizeof(BYTE), len, bitmap_file);
    for (UINT i = 0; i < len; i++) 
        memcpy(img.p_buffer + len - 1 - i, buff + i, sizeof(BYTE));
    delete [] buff;
    fclose(bitmap_file);
}

void calculate_avg_global(IMAGE &img) {
    for (UINT i = 0; i < BUFFER_LEN; i++)
        average += img.p_buffer[i];
    average /= BUFFER_LEN;
}

void apply_contrast(IMAGE &img) {
    if (img.b_current_contrast < 0)
        for (UINT j = 0; j < BUFFER_LEN; j++)
            img.p_buffer[j] = max(min(img.p_original_image[j] / -img.b_current_contrast, 255), 0);
    else
        for (UINT j = 0; j < BUFFER_LEN; j++)
            img.p_buffer[j] = max(min(255, img.p_original_image[j] * img.b_current_contrast), 0);
}

void contrast_up(IMAGE &img) {
    img.b_current_contrast += 0.2;
    apply_contrast(img);
}

void contrast_down(IMAGE &img) {
    img.b_current_contrast -= 0.2;
    apply_contrast(img);
}

void negate(IMAGE &img) {
    for (UINT i = 0; i < BUFFER_LEN; i++)
        img.p_buffer[i] = ~img.p_buffer[i];
}

void threshold(IMAGE &img, BYTE edge) {
    img.p_treshold += edge;
    for (UINT i = 0; i < BUFFER_LEN; i++) {
        if (img.p_buffer[i] < img.p_treshold)
            img.p_buffer[i] = 0; 
    }
}

void brightnes(IMAGE &img, int value) {
    for (UINT i = 0; i < BUFFER_LEN; i++)
        img.p_buffer[i] = max(min(255, img.p_buffer[i] + value), 0);
}

void execute_operation(BYTE mode, BYTE arrow, IMAGE &img) {
    if (mode == 'c') {
        if (arrow == UP_ARROW) contrast_up(img);
        if (arrow == DOWN_ARROW) contrast_down(img);
    }
    else if (mode == 'b') {
        if (arrow == UP_ARROW) brightnes(img, -10);
        if (arrow == DOWN_ARROW) brightnes(img, 10);
    }
    else if (mode == 'n') {
        negate(img);
    }
    else if (mode == 't') {
        if (arrow == UP_ARROW) threshold(img, 10);
        if (arrow == DOWN_ARROW) threshold(img, -10);
    }
}

int main() {
    // Enter 0x13h video mode
    REGPACK regs;
    regs.r_ax = 0x13;
    intr(0x10, &regs);

    BYTE current_mode = 'n';

    IMAGE img;

    init_image(img);

    open_file(img);

    /// Save original version of loaded image
    memcpy(&img.p_original_image[0], &img.p_buffer[0], BUFFER_LEN); 

    calculate_avg_global(img);

    char pressed_key = 0;

    // main loop
    while (1) {
        if(!kbhit()) continue;
        pressed_key = getch();

        if (pressed_key == ESCAPE) break;
        
        if (pressed_key == 'r') {
            free_image_memory(img);
            init_image(img);
            open_file(img);
        }

        else if (pressed_key == UP_ARROW || pressed_key == DOWN_ARROW)
            execute_operation(current_mode, pressed_key, img);

        else if (pressed_key > 90)
            current_mode = (char)pressed_key;
        
        // Update graphics buffer
        memcpy(&VGA[0], &img.p_buffer[0], BUFFER_LEN);
    }

    free_image_memory(img);

    // Exit 0x13h video mode
    REGPACK regs;
    regs.r_ax = 0x03;
    intr(0x10, &regs);
    return 0;
}