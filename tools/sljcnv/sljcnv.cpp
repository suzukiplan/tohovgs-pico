#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include "picojson.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "sljcnv /path/to/songlist.json\n");
        return -1;
    }
    std::ifstream ifs(argv[1]);
    auto json = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    picojson::value v;
    std::string err = picojson::parse(v, json);
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return -1;
    }
    auto obj = v.get<picojson::object>();
    auto albums = obj["albums"].get<picojson::array>();
    for (const auto& album : albums) {
        auto albumObj = album.get<picojson::object>();
        printf("name: %s\n", albumObj["formalName"].get<std::string>().c_str());
        printf("copyright: %s\n", albumObj["copyright"].get<std::string>().c_str());
        printf("color: %d\n", (int)albumObj["compatColor"].get<double>());
        auto songs = albumObj["songs"].get<picojson::array>();
        for (const auto& song : songs) {
            auto songObj = song.get<picojson::object>();
            printf("\tname: %s\n", songObj["name"].get<std::string>().c_str());
            printf("\tmml: %s\n", songObj["mml"].get<std::string>().c_str());
            printf("\tloop: %d\n", (int)songObj["loop"].get<double>());
        }
    }
    return 0;
}
