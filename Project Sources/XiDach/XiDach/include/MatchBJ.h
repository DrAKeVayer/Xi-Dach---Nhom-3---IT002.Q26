#pragma once
#include "Constants.h"
#include "Entities.h"
#include "Engine.h"
#include "Stats.h"

class MatchBJ : public Match {
protected:
    

public:

    void splitTo(Player& p, int x);
    void InsurePlay(Player& p, Hand& h, Dealer& d);
    void SplitPlay(Player& p, Hand& h, Dealer& d);
    void SurrenderPlay(Player& p, Hand& h, Dealer& d);
    void DoublePlay(Player& p, Hand& h, Dealer& d);

    void doResultBJ(Hand& p, Hand& d, int& result, StatBJ& stat);

    int playerWantsBJ(Hand& p, Hand& d);

    bool playerInsure(Hand& p, Hand& d);

    void ProcessSimulationBJ(Dealer& dealer, StatBJ& stat);

    void SimulationBJ(ofstream& file);

    bool DealerWantsBJ(Dealer& d);

    void AnotherRound();
    int GetSit(Player& p, Hand& h, Dealer& d);
    void Options(Player& p, Hand& h, Dealer& d, int x);
    void PrintState(Player& p, Dealer& d);

    void AISuggest(Player& p, Hand& h, Dealer& d);


    void ProcessPlayBJ(Dealer& d);

    void PlayBJ();

};
