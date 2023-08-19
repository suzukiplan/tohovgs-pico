#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "../../include/model.h"

int get_bgm_size(std::string mml)
{
    std::string path = "./rom/" + mml + ".bgm";
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    int result = (int)ftell(fp);
    fclose(fp);
    return result;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fprintf(stderr, "sldcnv /path/to/songlist.dat /path/to/songlist.bin\n");
        return -1;
    }
    FILE* fpR = fopen(argv[1], "rt");
    if (!fpR) {
        fprintf(stderr, "File open error\n");
        return -1;
    }
    FILE* fpW = fopen(argv[2], "wb");
    if (!fpW) {
        fprintf(stderr, "File open error\n");
        fclose(fpR);
        return -1;
    }

    char buf[1024];
    Album album;
    int songs = 0;
    int head = 0;
    while (fgets(buf, sizeof(buf), fpR)) {
        char* cp = strchr(buf, '\r');
        if (cp) {
            *cp = 0;
        } else {
            cp = strchr(buf, '\n');
            if (cp) {
                *cp = 0;
            }
        }
        if (0 == strncmp(buf, "name: ", 6)) {
            if (0 < songs) {
                fwrite(&album, sizeof(album), 1, fpW);
            }
            memset(&album, 0, sizeof(album));
            songs = 0;
            strncpy(album.name, &buf[6], sizeof(album.name) - 1);
        } else if (0 == strncmp(buf, "copyright: ", 11)) {
            strncpy(album.copyright, &buf[11], sizeof(album.copyright) - 1);
        } else if (0 == strncmp(buf, "color: ", 7)) {
            switch (atoi(&buf[7])) {
                case 7: album.color = 0b0000000000001110; break;
                case 35: album.color = 0b0000000110000000; break;
                case 196: album.color = 0b0100000100001000; break;
                default:
                    printf("detect invalid color code %d\n", atoi(&buf[7]));
                    exit(-1);
            }
        } else if (0 == strncmp(buf, "\tname: ", 7)) {
            strncpy(album.songs[songs].name, &buf[7], sizeof(album.songs[songs].name) - 1);
        } else if (0 == strncmp(buf, "\tmml: ", 6)) {
            album.songs[songs].no = atoi(strchr(buf, '-') + 1);
            album.songs[songs].bgmSize = get_bgm_size(&buf[6]);
            album.songs[songs].bgmHead = head;
            head += album.songs[songs].bgmSize;
        } else if (0 == strncmp(buf, "\tloop: ", 7)) {
            album.songs[songs].loop = atoi(&buf[7]);
            songs++;
        }
    }
    if (0 < songs) {
        fwrite(&album, sizeof(album), 1, fpW);
    }
    fclose(fpR);
    fclose(fpW);
    return 0;
}
