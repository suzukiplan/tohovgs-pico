#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "sldmak /path/to/songlist.dat\n");
        return -1;
    }
    FILE* fpR = fopen(argv[1], "rt");
    if (!fpR) {
        fprintf(stderr, "File open error\n");
        return -1;
    }

    char buf[1024];
    std::vector<std::string> mml;
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
        if (0 == strncmp(buf, "\tmml: ", 6)) {
            mml.push_back(&buf[6]);
        }
    }
    fclose(fpR);
    if (mml.empty()) {
        return 0;
    }

    const char* compiler = "tools/vgsmml/vgsmml";
    printf("all: %s", compiler);
    for (auto bgm : mml) {
        printf(" rom/%s.bgm", bgm.c_str());
    }
    printf("\n");
    for (auto bgm : mml) {
        std::string dist = "rom/" + bgm + ".bgm";
        std::string src = "mml/" + bgm + ".mml";
        printf("\n%s: %s\n", dist.c_str(), src.c_str());
        printf("\t%s %s %s\n", compiler, src.c_str(), dist.c_str());
    }

    return 0;
}
