#include "Constants.h"
int rankTable[53];
string rankName[53];

void initRankTable(int rankTable[]) {
    for (int i = 1; i <= 52; i++) {
        int r = (i - 1) % 13 + 1; //Return ranks
        rankTable[i] = (r > 10) ? 10 : r; //Return points
    }
}

string getSuit(int x) {
    int suit = (x - 1) / 13;
    if (suit == 0) return "♠";
    else if (suit == 1) return "♣";
    else if (suit == 2) return "♢";
    else if (suit == 3) return "♡";
    return " of Default";
}

void initRankName(string rankName[]) {
    for (int i = 1; i <= 52; i++) {
        int r = (i - 1) % 13 + 1; //Return number
        if (r == 13) rankName[i] = "K";
        else if (r == 12) rankName[i] = "Q";
        else if (r == 11) rankName[i] = "J";
        else if (r == 1) rankName[i] = "A";
        else rankName[i] = to_string(r);
    }
}

string getRankName(int x) {
    int rank = (x - 1) % 13 + 1;

    if (rank == 1) return "A";
    if (rank == 11) return "J";
    if (rank == 12) return "Q";
    if (rank == 13) return "K";

    return to_string(rank);
}

bool Chance(double x) {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(gen) < x;
}

string getFileName(const string& base, const string& csv) {
    string filename = base + csv;
    int counter = 1;

    while (true) {
        ifstream file(filename);
        if (!file.good()) {
            return filename; // file does NOT exist
        }

        file.close();
        filename = base + "_(" + std::to_string(counter) + ")" + csv;
        counter++;
    }
}