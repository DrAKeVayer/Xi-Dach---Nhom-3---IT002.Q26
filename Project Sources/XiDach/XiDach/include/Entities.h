#pragma once
#include "Constants.h"
using namespace std;
class Player;

class Hand {
public:
    vector<int> hand;
    Player* owner;
    int score = 0;
    bool isBust = false;
    bool hasAce = false;
    bool isSoft = false;
    bool isSplitable = false;
    bool SplitWon = false; //both split and won
    bool SplitLost = false; //both split and lost
    int softScore = 0;
    int first2SoftScore = 0;
    int first2HardScore = 0;
    int first2Score = 0;
    bool resolved = false;
    bool isDoubled = false;
    bool wasSoft = false;
    bool splited = false;
    int Pos;
    double profit = 0;
    bool surrendered = false;
    bool stood = false;
    bool toBeChecked = false;

    int HitHard[3][3] = {
            {0, 0, 0}, // 16 điểm (2, 3, 4 lá)
            {0, 0, 0}, // 17 điểm
            {0, 0, 0} // 18 điểm
    };
    int HitSoft[3][3] = {
            {0, 0, 0}, // 16 điểm (2, 3, 4 lá)
            {0, 0, 0}, // 17 điểm
            {0, 0, 0} // 18 điểm
    };
    int StandHard[3][3] = {
            {0, 0, 0}, // 16 điểm (2, 3, 4 lá)
            {0, 0, 0}, // 17 điểm
            {0, 0, 0} // 18 điểm
    };
    int StandSoft[3][3] = {
            {0, 0, 0}, // 16 điểm (2, 3, 4 lá)
            {0, 0, 0}, // 17 điểm
            {0, 0, 0} // 18 điểm
    };


    Hand(Player* _p, int _C);

    void reset();
    void calculateScore();
    void receiveCard(int x);
    void setSolved();
    int getFaceup();
    void setWon();
    void setLost();
    void setDouble();
    HandType getHandType();
    int getScore() const;
    int getSoftScore() const;
    void upProfit(double x);
    void printNewCard();
    void printHand() const;
    void printJustHand() const;

};

class Player {
public:
    vector<Hand> hands;
    int splitCount = 0;
    double profit = 0;
    int Pos = 0;

    Player() = default;
    Player(int _p);

    virtual ~Player();
    void resetPlayer();
    void splitHand(int x);
    void printHand();
    void printProfit();
    virtual void printOwner();
    virtual void printForWho(Hand& h);
    void upProfit(double x);
};

class Dealer : public Player {
public:
    using Player::Player;

    bool stood = false;
    void printDealerFirstHand() const;
    void printDealerHand() const;
    void printOwner() override;
    void printForWho(Hand& h) override;
};