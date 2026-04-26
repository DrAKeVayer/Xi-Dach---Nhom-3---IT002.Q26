#pragma once
#include "constants.h"
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

class Match {
protected:
    vector<int> deck;
    int playerCount = 0;
    int cardIdx = 0;
    bool Has3 = false;
    bool Has4 = false;
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
    void splitTo(Player& p, int x);
    void setPlayerCount();
    void deductPlayerCount();
    int getPlayerCount() const;
    void set3();
    void set4();
    void no3();
    void no4();
    bool get3() const;
    bool get4() const;

};

class Stat {
protected:
    int PWin = 0;
    int PLose = 0;
    int DWin = 0;
    int Draw = 0;
    int PBJWin = 0;
    int DBJWin = 0;
    int BJDraw = 0;
    int PWin16 = 0;
    int PLose16 = 0;
    int PWinDBust = 0;

    int PBust = 0;
    int DLoseAgainstPScore[32] = { 0 };
    int DWinAgainstPScore[32] = { 0 };
    int DLoseAgainstPFirst2Score[22] = { 0 };
    int DWinAgainstPFirst2Score[22] = { 0 };

    int PWinSoft[22] = { 0 }; //if player wins, track if player wins with soft or hard score
    int PWinHard[22] = { 0 };
    int PLoseFirst2Soft[22] = { 0 }; //if player loses, track the score of first 2 cards (soft or hard)
    int PLoseFirst2Hard[22] = { 0 };
    int PWinFirst2Soft[22] = { 0 };  //if player wins, track the score of first 2 cards (soft or hard)
    int PWinFirst2Hard[22] = { 0 };
    int PTotalHit = 0;
    int PHitWin = 0;
    int PStandedWin = 0;
    int PStandSoftWin = 0; //if player loses, but would have won if they stand at the last soft hand
    int PWinAfterHitTime[12] = { 0 };  //track how many hits if player wins
    int PLoseAfterHitTime[12] = { 0 }; //track how many hits if player loses

    int DBustCount = 0;
    int BustDraw = 0;
    double TotalProfit = 0;
    int TotalRound = 0;

public:
    void upPWinDBust();
    void upPLose();
    void upPLose16();
    void upPWin();
    void upDWin();
    void upPWin16();
    void upDraw();
    void upPBJWin();
    void upDBJWin();
    void upBJDraw();
    void upPTotalHit(int x);
    void upPHitWin();
    void upPStandedWin();
    void upPBust();

    void upDLoseAgainstPScore(int i);
    void upDWinAgainstPScore(int i);
    void upDLoseAgainstPFirst2Score(int i);
    void upDWinAgainstPFirst2Score(int i);

    void upPWinSoft(int i);
    void upPWinHard(int i);
    void upPLoseFirst2Soft(int i);
    void upPLoseFirst2Hard(int i);
    void upPWinFirst2Soft(int i);
    void upPWinFirst2Hard(int i);

    void upPWinAfterHitTime(int i);
    void upPLoseAfterHitTime(int i);

    void upDBustCount();
    void upBustDraw();

    void upTotalProfit(double x);
    void setTotalRound(int x);

    virtual void printStat();

    virtual void exportStat(ofstream& file);
};

class StatBJ : public Stat {
protected:
    int PWinVSup[11] = { 0 }; //if player wins, track the up card of dealer
    int PLoseVSup[11] = { 0 }; //if player loses, track the up card of dealer
    int TotalSplit = 0;
    int SplitWin = 0;
    int SplitLose = 0;
    int PSplitWinRank[11] = { 0 };
    int PSplitLoseRank[11] = { 0 };
    int PSplitWinVSup[11] = { 0 };
    int PSplitLoseVSup[11] = { 0 };

    int InsureCount = 0;
    int InsureWinCount = 0;
    int InsureLoseCount = 0;

    int DoubleCount = 0;
    int DoubleWinCount = 0;
    int DoubleLoseCount = 0;
    int DoubleWinHardScore[22] = { 0 };
    int DoubleWinSoftScore[22] = { 0 };
    int DoubleLoseHardScore[22] = { 0 };
    int DoubleLoseSoftScore[22] = { 0 };
    int DoubleVSup[11] = { 0 };
    int DoubleWinVSup[11] = { 0 };
    int DoubleLoseVSup[11] = { 0 };

    int SurrenderCount = 0;

    double EVBJ;

public:
    void upPWinVSup(int i);
    void upPLoseVSup(int i);

    void upTotalSplit(int x);
    void upSplitWin();
    void upSplitLose();
    void upPSplitWinRank(int i);
    void upPSplitLoseRank(int i);
    void upPSplitWinVSup(int i);
    void upPSplitLoseVSup(int i);

    void downPBJWin();
    void setEVBJ();
    void upInsureCount();
    void upInsureWinCount();
    void upInsureLoseCount();

    void upDoubleCount();
    void upDoubleWinCount();
    void upDoubleLoseCount();
    void upDoubleWinHardScore(int i);
    void upDoubleWinSoftScore(int i);
    void upDoubleLoseHardScore(int i);
    void upDoubleLoseSoftScore(int i);
    void upDoubleVSup(int i);
    void upDoubleWinVSup(int i);
    void upDoubleLoseVSup(int i);
    void upSurrenderCount();

    void printStat() override;

    void exportStat(ofstream& file) override;
};

class StatXD : public Stat {
protected:
    int TotalCheck = 0;
    int DCheckWin = 0; //track all wins excluding XIDACH or XIBANG
    int DCheckLose = 0;
    int DCheckWinPreTurn = 0; //track all wins when checking pre-dealer-turn
    int DCheckLosePreTurn = 0;
    int DCheckWinPreTurnScore[22] = { 0 }; //track dealer's score for all wins when checking pre-dealer-turn
    int DCheckLosePreTurnScore[22] = { 0 };
    int DCheckWinPostTurn = 0; //exclude XETALL
    int DCheckLosePostTurn = 0; //exclude Dealer Bust
    int DCheckWinPostTurnScore[22] = { 0 }; //track dealer's score for all wins when checking at dealer's turn
    int DCheckLosePostTurnScore[22] = { 0 };
    int DLastWin = 0; //track wins at XETALL
    int DLastLose = 0;
    int DCheckWinAgainstSizePreTurn[6] = { 0 };
    int DCheckLoseAgainstSizePreTurn[6] = { 0 };
    int DCheckWinAgainstSizePostTurn[6] = { 0 };
    int DCheckLoseAgainstSizePostTurn[6] = { 0 };

    int XBCount = 0;
    int PNLCount = 0;
    int DNLCount = 0;
    int XBDrawCount = 0;
    int PNLWinCount = 0;
    int DNLWinCount = 0;
    int NLDrawCount = 0;

    double EVXD = 0;
public:
    void upTotalCheck();
    void upDCheckWin();
    void upDCheckLose();
    void upDCheckWinPreTurn();
    void upDCheckLosePreTurn();
    void upDCheckWinPreTurnScore(int i);
    void upDCheckLosePreTurnScore(int i);
    void upDCheckWinPostTurn();
    void upDCheckLosePostTurn();
    void upDCheckWinPostTurnScore(int i);
    void upDCheckLosePostTurnScore(int i);
    void upDLastWin();
    void upDLastLose();
    void upDCheckWinAgainstSizePreTurn(int i);
    void upDCheckLoseAgainstSizePreTurn(int i);
    void upDCheckWinAgainstSizePostTurn(int i);
    void upDCheckLoseAgainstSizePostTurn(int i);

    void upXBCount();
    void upPNLCount();
    void upDNLCount();
    void upXBDrawCount();
    void upPNLWinCount();
    void upDNLWinCount();
    void upNLDrawCount();
    void setEVXD();

    void printStat() override;

    void exportStat(ofstream& file) override;
};

