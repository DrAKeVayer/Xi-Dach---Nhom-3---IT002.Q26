#pragma once
#include "Constants.h"
#include "Entities.h"

class Match {
protected:
    vector<int> deck;
    int playerCount = 0;
    int cardIdx = 0;
public:
    vector<Player> players;
    Dealer dealer;

    Match();

    void addPlayer(int i);
    void deckInit();
    void shuffle();
    void deckReset();
    int drawCard();
    void dealCardToPlayer(Player& p, int x);
    void dealCardToHand(Hand& h);
    void deal2Card();
    
    int compareHands(Hand& a, Hand& b);

    void HitPlay(Hand& h);
    void PrintState(Player& p, Dealer& d);
    void StandPlay(Player& p, Hand& h, Dealer& d);
    
    void PrintResult(Hand& h, Dealer& d, int res);
    void PrintResultXD(Player& p, Dealer& d, int res);
    void PrintPBust(Hand& h);

};