#include "Constants.h"
#include "Entities.h"
#include "MatchBJ.h"
#include "MatchXD.h"
#include "Stats.h"

using namespace std;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    initRankTable(rankTable);
    initRankName(rankName);

    string base = "stats";
    string csv = ".csv";
    string filename = getFileName(base, csv);

    ofstream file(filename);

    int choice;

    cout << R"(
    Type your rules:
    1: BlackJack
    2: Xi Dach
    )";
    cin >> choice;
    if (choice == 1) {
        cout << R"(
        Chosen BlackJack
        Type your mode:
        1: Simulation
        2: Play
        )";
        cin >> choice;
        MatchBJ match;
        if (choice == 1) match.SimulationBJ(file);
        else if (choice == 2) match.PlayBJ();
        return 0;
    }
    else if (choice == 2) {
        cout << R"(
        Chosen Xi Dach
        Type your mode:
        1: Simulation
        2: Play
        )";
        cin >> choice;
        MatchXD match;
        if (choice == 1) match.SimulationXD(file);
        else if (choice == 2) {
            cout << R"(
            Chosen Play
            Type your role:
            1: You play as Players
            2: You play as Dealer
            )";
            cin >> choice;
            if (choice == 1) match.PlayPlayer();
            else match.PlayDealer();
        }
        return 0;
    }
    return 0;
}