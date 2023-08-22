/**
 * VGS Fixed NOTE data to Variable NOTE data
 * 東方BGM on VGS for RasberryPi Pico
 * License under GPLv3: https://github.com/suzukiplan/tohovgs-pico/blob/master/LICENSE.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTYPE_ENV1 1
#define NTYPE_ENV2 2
#define NTYPE_VOL 3
#define NTYPE_MVOL 4
#define NTYPE_KEYON 5
#define NTYPE_KEYOFF 6
#define NTYPE_WAIT 7
#define NTYPE_PDOWN 8
#define NTYPE_JUMP 9
#define NTYPE_LABEL 10
#define NTYPE_WAIT_8 11
#define NTYPE_WAIT_16 12
#define NTYPE_WAIT_32 13

#define MAX_NOTES 65536

typedef struct _NOTE {
    unsigned char type;
    unsigned char op1;
    unsigned char op2;
    unsigned char op3;
    unsigned int val;
} NOTE;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        puts("vgsftv input output");
        return 1;
    }
    FILE* fpR = fopen(argv[1], "rb");
    FILE* fpW = fopen(argv[2], "wb");
    if (!fpR || !fpW) {
        puts("file open error");
        if (fpR) fclose(fpR);
        if (fpW) fclose(fpW);
        return -1;
    }

    NOTE note;
    while (sizeof(NOTE) == fread(&note, 1, sizeof(NOTE), fpR)) {
        if (note.type == NTYPE_WAIT) {
            if (note.val < 1) {
                // ignore 0 or minus
            } else if (note.val < 256) {
                unsigned char op1 = NTYPE_WAIT_8 << 4;
                fwrite(&op1, 1, 1, fpW);
                unsigned char v8 = note.val & 0xFF;
                fwrite(&v8, 1, 1, fpW);
            } else if (note.val < 65536) {
                unsigned char op1 = NTYPE_WAIT_16 << 4;
                fwrite(&op1, 1, 1, fpW);
                unsigned short v16 = note.val & 0xFFFF;
                fwrite(&v16, 2, 1, fpW);
            } else {
                unsigned char op1 = NTYPE_WAIT_32 << 4;
                fwrite(&op1, 1, 1, fpW);
                fwrite(&note.val, 4, 1, fpW);
            }
            continue;
        }
        unsigned char op1 = note.type & 0x0F;
        op1 <<= 4;
        op1 |= note.op1 & 0x0F;
        fwrite(&op1, 1, 1, fpW);
        switch (note.type) {
            case NTYPE_JUMP: {
                fwrite(&note.val, 4, 1, fpW);
                break;
            }
            case NTYPE_ENV1:
            case NTYPE_ENV2:
            case NTYPE_PDOWN: {
                short v16 = (short)note.val;
                fwrite(&v16, 2, 1, fpW);
                break;
            }
            case NTYPE_VOL:
            case NTYPE_MVOL: {
                unsigned char v8 = note.val & 0xFF;
                fwrite(&v8, 1, 1, fpW);
                break;
            }
            case NTYPE_KEYON: {
                fwrite(&note.op2, 1, 1, fpW);
                fwrite(&note.op3, 1, 1, fpW);
                break;
            }
            case NTYPE_KEYOFF:
            case NTYPE_LABEL: {
                break;
            }
            default:
                printf("detect invalid type (%d)\n", note.type);
                exit(-1);
        }
    }

    fclose(fpR);
    fclose(fpW);
    return 0;
}