#include "Options.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
using std::ifstream;
using std::ofstream;
using std::stoi;

#ifdef __APPLE__
#include <sys/stat.h>
#include <sys/types.h>
#include "ResourcePath.hpp"
#elif __WIN32
#include <shlobj.h>
#include <stdlib.h>
#include <windows.h>
#endif

namespace Options {

enum class TypeID { Null, Key, Bool, Time, Uint8, Short, String };

template <typename T>
TypeID getTypeID() {
    if (std::is_same<T, sf::Keyboard::Key>::value)
        return TypeID::Key;
    else if (std::is_same<T, bool>::value)
        return TypeID::Bool;
    else if (std::is_same<T, sf::Time>::value)
        return TypeID::Time;
    else if (std::is_same<T, uint8_t>::value)
        return TypeID::Uint8;
    else if (std::is_same<T, short>::value)
        return TypeID::Short;
    else if (std::is_same<T, std::string>::value)
        return TypeID::String;
    else
        return TypeID::Null;
}

std::map<std::string, std::pair<TypeID, void*>>& getMap() {
    static std::map<std::string, std::pair<TypeID, void*>> map;
    return map;
}

void toLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](char i) -> char { return std::tolower(i); });
}

template <typename T>
T& get(std::string name) {
    static auto& map = getMap();
    toLower(name);

#ifdef DEBUG
    if (map.find(name) == map.end()) std::cout << "Option::get with invalid name: " << name << std::endl;
#endif

    return *reinterpret_cast<T*>(map[name].second);
}

template bool& get<bool>(std::string name);
template uint8_t& get<uint8_t>(std::string name);
template std::string& get<std::string>(std::string name);
template sf::Time& get<sf::Time>(std::string name);
template sf::Keyboard::Key& get<sf::Keyboard::Key>(std::string name);
template short& get<short>(std::string name);

uint8_t& get_piece_rotation(int num) { return get_basepieces()[num].rotation; }

uint8_t& get_piece_color(int num) { return get_basepieces()[num].tile; }

std::array<basePieces, 7>& get_basepieces() {
    static std::array<basePieces, 7>& basepieces = get<std::array<basePieces, 7>>("BasePieces");
    return basepieces;
}

std::vector<sf::VideoMode>& get_videomodes() {
    static std::vector<sf::VideoMode>& videomodes = get<std::vector<sf::VideoMode>>("modes");
    return videomodes;
}

template <typename T>
T& add(std::string name) {
    toLower(name);
    auto& item = getMap()[name];
    if (item.first == TypeID::Null) {
        item.first = getTypeID<T>();
        item.second = reinterpret_cast<void*>(new T{});
    }

    return *reinterpret_cast<T*>(item.second);
}

template <typename T>
unsigned setOptions(const std::vector<std::pair<std::string, T>>& vec) {
    for (auto p : vec) add<T>(p.first) = p.second;

    return vec.size();
}

unsigned loadStandardOptions() {
    unsigned amount = 0;

    amount += setOptions<sf::Keyboard::Key>({{"down", sf::Keyboard::K},
                                             {"left", sf::Keyboard::J},
                                             {"right", sf::Keyboard::L},
                                             {"rcw", sf::Keyboard::F},
                                             {"rccw", sf::Keyboard::D},
                                             {"r180", sf::Keyboard::A},
                                             {"hd", sf::Keyboard::Space},
                                             {"menu", sf::Keyboard::LAlt},
                                             {"score", sf::Keyboard::Tab},
                                             {"away", sf::Keyboard::F4},
                                             {"ready", sf::Keyboard::F5}});

    amount += setOptions<bool>({{"ghostpiece", true},
                                {"fullscreen", false},
                                {"vsync", false},
                                {"performanceOutput", true},
                                {"animatedBackground", true},
                                {"sound", true},
                                {"rememberme", false},
                                {"noSound", false},
                                {"mouseMenu", true}});

    amount += setOptions<sf::Time>({{"repeatdelay", sf::milliseconds(150)},
                                    {"repeatspeed", sf::milliseconds(0)},
                                    {"repeatdelaydown", sf::milliseconds(20)},
                                    {"repeatspeeddown", sf::milliseconds(20)},
                                    {"framedelay", sf::milliseconds(10)},
                                    {"inputdelay", sf::milliseconds(4)}});

    amount += setOptions<uint8_t>({{"piece_0_rotation", 3},
                                   {"piece_1_rotation", 1},
                                   {"piece_2_rotation", 3},
                                   {"piece_3_rotation", 1},
                                   {"piece_4_rotation", 1},
                                   {"piece_5_rotation", 2},
                                   {"piece_6_rotation", 0},
                                   {"piece_0_color", 4},
                                   {"piece_1_color", 3},
                                   {"piece_2_color", 5},
                                   {"piece_3_color", 7},
                                   {"piece_4_color", 2},
                                   {"piece_5_color", 1},
                                   {"piece_6_color", 6},
                                   {"theme", 1},
                                   {"ghostPieceAlpha", 180},
                                   {"fieldBackground", 0}});

    amount += setOptions<short>({{"currentmode", 0}, {"musicvolume", 100}, {"effectvolume", 100}, {"chatvolume", 100}, {"pass", 0}});

    amount += setOptions<std::string>({{"name", "Player"}, {"hash", ""}, {"username", ""}});

    return amount;
}

void save() {
#ifdef __WIN32
    PWSTR appdataW;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdataW);
    char appdata[500];
    wcstombs(appdata, appdataW, 500);
    std::string appdataFolder(appdata);
    appdataFolder += "\\SpeedBlocks\\";
    CreateDirectory(appdataFolder.c_str(), NULL);
    CoTaskMemFree(appdataW);

    ofstream file(appdataFolder + "options.cfg");
#elif __APPLE__
    mkdir(applicationSupportFolder().c_str(), 0755);
    ofstream file(applicationSupportFolder() + "options.cfg");
#else
    ofstream file("options.cfg");
#endif

    if (!file.is_open()) {
#ifdef DEBUG
        std::cout << "Failed to open options.cfg" << std::endl;
#endif
        return;
    }

    auto& basepiece = get_basepieces();
    for (int i = 0; i < 7; ++i) {
        get<uint8_t>("piece_" + std::to_string(i) + "_rotation") = basepiece[i].rotation;
        get<uint8_t>("piece_" + std::to_string(i) + "_color") = basepiece[i].tile;
    }

    for (auto& item : getMap()) {
        switch (item.second.first) {
            case TypeID::Key:
                file << item.first << "=" << *reinterpret_cast<sf::Keyboard::Key*>(item.second.second) << '\n';
                break;
            case TypeID::Bool:
                file << item.first << "=" << *reinterpret_cast<bool*>(item.second.second) << '\n';
                break;
            case TypeID::Time:
                if (item.first == "framedelay" || item.first == "inputdelay")
                    file << item.first << "=" << reinterpret_cast<sf::Time*>(item.second.second)->asMicroseconds() << '\n';
                else
                    file << item.first << "=" << reinterpret_cast<sf::Time*>(item.second.second)->asMilliseconds() << '\n';
                break;
            case TypeID::Uint8:
                file << item.first << "=" << static_cast<int>(*reinterpret_cast<uint8_t*>(item.second.second)) << '\n';
                break;
            case TypeID::Short:
                file << item.first << "=" << *reinterpret_cast<short*>(item.second.second) << '\n';
                break;
            case TypeID::String:
                file << item.first << "=" << *reinterpret_cast<std::string*>(item.second.second) << '\n';
                break;
            default:
                break;
        }
    }
}

void initBasePieces() {
    std::vector<short> value = pieceArray();

    auto& basepiece = add<std::array<basePieces, 7>>("BasePieces");

    short vc = 0;

    for (int p = 0; p < 7; p++) {
        basepiece[p].posX = 0;
        basepiece[p].posY = 0;
        basepiece[p].lpiece = false;
        basepiece[p].rotation = get<uint8_t>("piece_" + std::to_string(p) + "_rotation");
        basepiece[p].current_rotation = 0;
        basepiece[p].tile = get<uint8_t>("piece_" + std::to_string(p) + "_color");
        basepiece[p].piece = p;
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++) {
                basepiece[p].grid[y][x] = value[vc];
                vc++;
            }
        basepiece[p].setColor(basepiece[p].tile);
        while (basepiece[p].rotation != basepiece[p].current_rotation) basepiece[p].rcw();
    }
    basepiece[4].lpiece = true;
    basepiece[6].lpiece = true;
}

void getVideoModes() {
    std::vector<sf::VideoMode> allmodes = sf::VideoMode::getFullscreenModes();
    auto& modes = add<std::vector<sf::VideoMode>>("modes");
    for (auto& new_mode : allmodes) {
        if (std::any_of(modes.begin(), modes.end(),
                        [&](sf::VideoMode& existing_mode) { return new_mode.width == existing_mode.width && new_mode.height == existing_mode.height; }))
            continue;

        modes.push_back(new_mode);
    }
}

void load() {
    getVideoModes();

    std::string line, keyword;

    unsigned amount = loadStandardOptions(), countset = 0;

#ifdef __WIN32
    PWSTR appdataW;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appdataW);
    char appdata[500];
    wcstombs(appdata, appdataW, 500);
    std::string appdataFolder(appdata);
    appdataFolder += "\\SpeedBlocks\\";
    CoTaskMemFree(appdataW);

    ifstream file(appdataFolder + "options.cfg");
#elif __APPLE__
    ifstream file(applicationSupportFolder() + "options.cfg");
#else
    ifstream file("options.cfg");
#endif

    if (!file.is_open()) {
#ifdef DEBUG
        std::cout << "Failed to open options.cfg" << std::endl;
#endif
        initBasePieces();
        return;
    }

    auto map = getMap();

    while (getline(file, line)) {
        auto delim = line.find('=');
        if (delim == std::string::npos) continue;

        keyword = line.substr(0, delim);
        toLower(keyword);
        line = line.substr(delim + 1);

        if (keyword.empty() || line.empty()) continue;

        auto& item = map[keyword];
        if (item.first != TypeID::String) {
            try {
                stoi(line);
            } catch (...) {
                continue;
            }
        }

        switch (item.first) {
            case TypeID::Key:
                *reinterpret_cast<sf::Keyboard::Key*>(item.second) = sf::Keyboard::Key(stoi(line));
                break;
            case TypeID::Bool:
                *reinterpret_cast<bool*>(item.second) = stoi(line);
                break;
            case TypeID::Time:
                if (keyword == "framedelay" || keyword == "inputdelay")
                    *reinterpret_cast<sf::Time*>(item.second) = sf::microseconds(stoi(line));
                else
                    *reinterpret_cast<sf::Time*>(item.second) = sf::milliseconds(stoi(line));
                break;
            case TypeID::Uint8:
                *reinterpret_cast<uint8_t*>(item.second) = stoi(line);
                break;
            case TypeID::Short:
                *reinterpret_cast<short*>(item.second) = stoi(line);
                break;
            case TypeID::String:
                *reinterpret_cast<std::string*>(item.second) = line;
                break;
            default:
#ifdef DEBUG
                std::cout << keyword << " = Nuller?" << std::endl;
#endif
                continue;
                break;
        }

        countset++;
    }

    if (countset != amount) {
#ifdef DEBUG
        std::cout << "Incomplete options.cfg, " << countset << " of " << amount << "set\n"
                  << "Missing options set to default" << std::endl;
#endif
    }

    initBasePieces();
}

void setDelay(short i, std::string string) {
    short value;
    if (string.size() > 0)
        value = std::stoi(string);
    else
        value = 0;
    if (i == 1)
        get<sf::Time>("repeatdelay") = sf::milliseconds(value);
    else if (i == 2)
        get<sf::Time>("repeatspeed") = sf::milliseconds(value);
    else if (i == 3)
        get<sf::Time>("repeatdelaydown") = sf::milliseconds(value);
    else if (i == 4)
        get<sf::Time>("repeatspeeddown") = sf::milliseconds(value);
}

std::vector<short> pieceArray() {
    return {0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0,

            0, 3, 0, 0, 0, 3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0,

            0, 5, 0, 0, 0, 5, 5, 0, 0, 0, 5, 0, 0, 0, 0, 0,

            0, 7, 0, 0, 7, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0,

            0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0,

            0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,

            0, 0, 0, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 0, 0, 0};
}

}  // Namespace Options
