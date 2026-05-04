#pragma once
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <windows.h> //for printing unicode symbols
#include <array>

using namespace std;

extern int rankTable[53];
extern string rankName[53];

void initRankTable(int rankTable[]);

string getSuit(int x);

void initRankName(string rankName[]);

string getRankName(int x);

bool Chance(double x);

using Table3x3 = array<array<double, 3>, 3>;

enum HandType {
    THUONG = 0,
    NGULINH = 1,
    XIDACH = 2,
    XIBANG = 3
};

enum Action {
    HIT = 1,
    XET = 1,
    XETALL = 0,
    STAND = 0,
    XET3 = 2,
    XET4 = 3,
    SPLIT = 4,
    DDOUBLE = 5,
    SURRENDER = 6
};

enum TIME {
    INSURE = 0,
    ALL = 1,
    SPLITDOUBLE = 3,
    SECONDSPLIT = 4,
    FIRSTDOUBLE = 5,
    HITSTAND = 6,
    SECONDDOUBLE = 7
};

string getFileName(const string& base, const string& csv);