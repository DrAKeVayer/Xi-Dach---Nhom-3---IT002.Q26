#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <windows.h> //for printing unicode symbols
using namespace std;

void PlayBJ();

int rankTable[53];
string rankName[53];

void initRankTable(int rankTable[]) {
    for (int i = 1; i <= 52; i++) {
        int r = (i - 1) % 13 + 1; //Return ranks
        rankTable[i] = (r > 10) ? 10 : r; //Return points
    }
}
//Deck will be A of Spades (1) to K of Spades (13). Then Clubs, Diamonds and Hearts
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
};

class Hand {
public:
    vector<int> hand;
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
    bool resolved = false;

    Hand() : score(0) {}

    void reset() {
        hand.clear();
        isBust = false;
        hasAce = false;
        score = 0;
    }

    void calculateScore() {
        isBust = false;
        hasAce = false;
        isSoft = false;
        isSplitable = false;
        score = 0;
        softScore = 0;
        for (auto c : hand) {
            int r = rankTable[c];
            if (r == 1) hasAce = true;
            score += r;
        }

        if (hasAce && (score + 10) < 22) {
            isSoft = true;
            score += 10;
            softScore = score;
            if (hand.size() == 2) first2SoftScore = score;
        }

        if (!isSoft && hand.size() == 2) first2HardScore = score;

        if (hand.size() == 2) {
            if (getRankName(hand[0]) == getRankName(hand[1])) isSplitable = true;
        }

        if (score > 21) {
            isBust = true;
        }
    }

    void receiveCard(int x) {
        hand.push_back(x);
        calculateScore();
    }

    void setSolved() {
        resolved = true;
    }

    int getFaceup() {
        int r = rankTable[hand[0]];
        return r;
    }

    void setWon() {
        SplitWon = true;
    }

    void setLost() {
        SplitLost = true;
    }

    HandType getHandType() {
        if (hand.size() == 2) {
            int r1 = rankTable[hand[0]];
            int r2 = rankTable[hand[1]];
            if (r1 == 1 && r2 == 1) {
                return XIBANG;
            }
        }

        if (hand.size() == 2 && score == 21) {
            return XIDACH;
        }

        if (hand.size() == 5 && score <= 21) {
            return NGULINH;
        }

        return THUONG;
    }

    int getScore() const {
        return score;
    }

    int getSoftScore() const {
        return softScore;
    }

    void printNewCard() const {
        cout << getRankName(hand.back());
            cout << getSuit(hand.back());
    }

    void printHand() const {
        for (int c : hand) {
            cout << getRankName(c);
            cout << getSuit(c) << ", ";
        }
        cout << "(";
        if (isSoft) cout << " Soft ";
        cout << getScore() << ")";
    }
};

class Player {  
public:
    vector<Hand> hands;

    Player() {
        hands.reserve(10);
        hands.push_back(Hand()); 
    }

    virtual ~Player() {}

    void resetPlayer() {
        hands.clear();
        hands.push_back(Hand()); 
    }

    void splitHand(int x) {
        hands.push_back(Hand());
        hands.back().hand.push_back(hands[x].hand.back());
        hands[x].hand.pop_back();
    }

    void printHand() const {
        for (int i = 0; i < hands.size(); i++) {
            cout << "Hand " << i + 1 << " of this player: ";
            for (int c : hands[i].hand) {
                cout << getRankName(c);
                cout << getSuit(c) << ", ";
            }
            cout << "(";
            if (hands[i].isSoft) cout << " Soft ";
            cout << hands[i].getScore() << ")";
        }
    }

    
};

class Dealer : public Player {
public:
    void printDealerFirstHand() const {
        cout << getRankName(hands[0].hand[0]);
        cout << getSuit(hands[0].hand[0]) << ", " << "?";
    }

    void printDealerHand() const {
        for (int c : hands[0].hand) {
            cout << getRankName(c);
            cout << getSuit(c) << ", ";
        }
    }
};

class Match {
protected:
    vector<int> deck;
    int playerCount;
    int cardIdx = 0;
    bool Has3 = false;
    bool Has4 = false;
public:
    vector<Player> players;
    Dealer dealer;

    Match() {
        deckInit();
        shuffle();
    }

    void addPlayer(Player p) {
        players.push_back(p);
    }

    void deckInit() {
        deck.clear();
        for (int i = 1; i < 53; i++) {
            deck.push_back(i);
        }
    }

    void shuffle() {
        static unsigned seed = static_cast<unsigned>(
            chrono::high_resolution_clock::now().time_since_epoch().count()
        );
        static mt19937 gen(seed);

        std::shuffle(deck.begin(), deck.end(), gen);
    }

    void deckReset() {
        cardIdx = 0;
    }

    int drawCard() {
        int card = deck[cardIdx++];
        if (cardIdx > 51) {
            deckInit();
            shuffle();
            cardIdx = 0;
        }
        return card;
    }

    void dealCardToPlayer(Player& p, int x) {
        p.hands[x].receiveCard(drawCard());
        p.hands[x].calculateScore();
    }

    void dealCardToHand(Hand& h) {
        h.receiveCard(drawCard());
        h.calculateScore();
    }

    void deal2Card() {
        for (Player& p : players) {
            p.hands[0].hand.push_back(drawCard());
            p.hands[0].hand.push_back(drawCard());
            p.hands[0].calculateScore();
        }

        dealer.hands[0].hand.push_back(drawCard());
        dealer.hands[0].hand.push_back(drawCard());
        dealer.hands[0].calculateScore();
    }

    void splitTo(Player& p, int x) {
        p.splitHand(x);
        dealCardToPlayer(p, x);
        dealCardToPlayer(p, p.hands.size() - 1);
        p.hands[x].calculateScore();
        p.hands.back().calculateScore();
    }

    void setPlayerCount() {
        playerCount = players.size();
    }

    void deductPlayerCount() {
        playerCount--;
    }

    int getPlayerCount() const {
        return playerCount;
    }

    void set3() {
        Has3 = true;
    }

    void set4() {
        Has4 = true;
    }

    void no3() {
        Has3 = false;
    }

    void no4() {
        Has4 = false;
    }

    bool get3() const{
        return Has3;
    }

    bool get4() const{
        return Has4;
    }
};

#include <iostream>
using namespace std;
class Stat {
protected:
    int PWin = 0;
    int DWin = 0;
    int Draw = 0;
    int PBJWin = 0;
    int DBJWin = 0;
    int BJDraw = 0;
    int PWin16 = 0;
    int PLose16 = 0;
    int PWinDBust = 0;
    
    int PWinSoft [22] = {0}; //if player wins, track if player wins with soft or hard score
    int PWinHard [22] = {0};
    int PLoseFirst2Soft [22] = {0}; //if player loses, track the score of first 2 cards (soft or hard)
    int PLoseFirst2Hard [22] = {0};
    int PWinFirst2Soft [22] = {0};  //if player wins, track the score of first 2 cards (soft or hard)
    int PWinFirst2Hard [22] = {0};
    int PTotalHit = 0;
    int PHitWin = 0;
    int PStandedWin = 0;
    int PStandSoftWin = 0; //if player loses, but would have won if they stand at the last soft hand
    int PWinAfterHitTime[12] = {0};  //track how many hits if player wins
    int PLoseAfterHitTime[12] = {0}; //track how many hits if player loses

    
    int DBustCount = 0;
    int BustDraw = 0;

public:
    void upPWinDBust() { PWinDBust++; }
    void upPLose16() { PLose16++; }
    void upPWin() { PWin++; }
    void upDWin() { DWin++; }
    void upPWin16() { PWin16++; }
    void upDraw() { Draw++; }
    void upPBJWin() { PBJWin++; }
    void upDBJWin() { DBJWin++; }
    void upBJDraw() { BJDraw++; }
    void upPTotalHit(int x) { PTotalHit += x; }
    void upPHitWin() { PHitWin++; }
    void upPStandedWin() { PStandedWin++; }

    void upPWinSoft(int i) { PWinSoft[i]++; }
    void upPWinHard(int i) { PWinHard[i]++; }
    void upPLoseFirst2Soft(int i) { PLoseFirst2Soft[i]++; }
    void upPLoseFirst2Hard(int i) { PLoseFirst2Hard[i]++; }
    void upPWinFirst2Soft(int i) { PWinFirst2Soft[i]++; }
    void upPWinFirst2Hard(int i) { PWinFirst2Hard[i]++; }
    
    void upPWinAfterHitTime(int i) { PWinAfterHitTime[i]++; }
    void upPLoseAfterHitTime(int i) { PLoseAfterHitTime[i]++; }

    void upDBustCount() { DBustCount++; }
    void upBustDraw() { BustDraw++; }

    virtual void printStat() {
        cout << "GENERIC STATS: " << '\n';
        cout << "Player wins " << PWin << " hands" << '\n';
        cout << "Dealer wins " << DWin << " hands" << '\n';
        cout << "Draw " << Draw << " hands" << '\n';
        cout << "Player wins with BlackJack " << PBJWin << " hands" << '\n';
        cout << "Dealer wins with BlackJack " << DBJWin << " hands" << '\n';
        cout << "Draw with BlackJack " << BJDraw << " hands" << '\n';
        cout << "Player wins on 16 " << PWin16 << " hands" << '\n';
        cout << "Player loses on 16 " << PLose16 << " hands" << '\n';
        cout << "Player wins and Dealer busts " << PWinDBust << " hands" << '\n';
        cout << "Player wins with soft score from 16 to 21: " << '\n';
        for (int i = 16; i < 22; i++) cout << PWinSoft[i] << " - ";
        cout << '\n' << "Player wins with hard score from 16 to 21: " << '\n';
        for (int i = 16; i < 22; i++) cout << PWinHard[i] << " - ";
        cout << '\n' << "Player wins with first 2 cards being soft 12 to 21: " << '\n';
        for (int i = 12; i < 22; i++) cout << PWinFirst2Soft[i] << " - ";
        cout << '\n' << "Player wins with first 2 cards being hard 4 to 21: " << '\n';
        for (int i = 4; i < 22; i++) cout << PWinFirst2Hard[i] << " - ";
        cout << '\n' << "Player loses with first 2 cards being soft 12 to 21: " << '\n';
        for (int i = 12; i < 22; i++) cout << PLoseFirst2Soft[i] << " - ";
        cout << '\n' << "Player loses with first 2 cards being hard 4 to 21: " << '\n';
        for (int i = 4; i < 22; i++) cout << PLoseFirst2Hard[i] << " - ";
        cout << '\n' << "Player wins against dealer up card ranked 1 to 10: " << '\n';
        cout << '\n' << "Player hits a total of: " << PTotalHit << " hands";
        cout << '\n' << "Player wins after hit: " << PHitWin << " hands";
        cout << '\n' << "Player wins without hit: " << PStandedWin << " hands";
        cout << '\n' << "Player wins this many hands after 0 - 1 - 2 - ... - 10 hits: " << '\n';
        for (int i = 0; i < 11; i++) cout << PWinAfterHitTime[i] << " - ";
        cout << '\n' << "Player loses this many hands after 0 - 1 - 2 - ... - 10 hits: " << '\n';
        for (int i = 0; i < 11; i++) cout << PLoseAfterHitTime[i] << " - ";
        
    }
};

class StatBJ : public Stat {
protected:
    int PWinVSup [11] = {0}; //if player wins, track the up card of dealer
    int PLoseVSup [11] = {0}; //if player loses, track the up card of dealer
    int TotalSplit = 0;
    int SplitWin = 0;
    int PSplitWinRank[11] = {0};
    int PSplitWinVSup[11] = {0};
    int PSplitLoseVSup[11] = {0};

public:
    void upPWinVSup(int i) { PWinVSup[i]++; }
    void upPLoseVSup(int i) { PLoseVSup[i]++; }
    void upTotalSplit(int x) { TotalSplit += x; }
    void upSplitWin() { SplitWin++; }
    void upPSplitWinRank(int i) { PSplitWinRank[i]++; }
    void upPSplitWinVSup(int i) { PSplitWinVSup[i]++; }
    void upPSplitLoseVSup(int i) { PSplitLoseVSup[i]++; }

    void printStat() override {
        Stat::printStat();
        cout << '\n' << "BLACKJACK SPECIFIC STATS: " << '\n';
        for (int i = 1; i < 11; i++) cout << PWinVSup[i] << " - ";
        cout << '\n' << "Player loses against dealer up card ranked 1 to 10: " << '\n';
        for (int i = 1; i < 11; i++) cout << PLoseVSup[i] << " - ";
        cout << '\n' << "Player split " << TotalSplit << " hands";
        cout << '\n' << "Player wins after split " << SplitWin << " hands";
        cout << '\n' << "Player wins after split rank from 1 to 10: " << '\n';
        for (int i = 1; i < 11; i++) cout << PSplitWinRank[i] << " - ";
        cout << '\n' << "Player wins after split against dealer up card ranked 1 to 10: " << '\n';
        for (int i = 1; i < 11; i++) cout << PSplitWinVSup[i] << " - ";
        cout << '\n' << "Player loses after split against dealer up card ranked 1 to 10: " << '\n';
        for (int i = 1; i < 11; i++) cout << PSplitLoseVSup[i] << " - ";
    }
};

class StatXD : public Stat {
protected:
    int TotalCheck = 0;
    int DCheckWin = 0; //track all wins excluding XIDACH or XIBANG
    int DCheckLose = 0;
    int DCheckWinPreTurn = 0; //track all wins when checking pre-dealer-turn
    int DCheckLosePreTurn = 0;
    int DCheckWinPreTurnScore[11] = {0}; //track dealer's score for all wins when checking pre-dealer-turn
    int DCheckLosePreTurnScore[11] = {0};
    int DCheckWinPostTurn = 0; //exclude XETALL
    int DCheckLosePostTurn = 0; //exclude Dealer Bust
    int DCheckWinPostTurnScore[11] = {0}; //track dealer's score for all wins when checking at dealer's turn
    int DCheckLosePostTurnScore[11] = {0};
    int DLastWin = 0; //track wins at XETALL
    int DLastLose = 0;
    int DCheckWinAgainstSizePreTurn [6] = {0};
    int DCheckLoseAgainstSizePreTurn [6] = {0};
    int DCheckWinAgainstSizePostTurn [6] = {0};
    int DCheckLoseAgainstSizePostTurn [6] = {0};

    int XBCount = 0;
    int PNLCount = 0;
    int DNLCount = 0;
    int XBDrawCount = 0;
    int PNLWinCount = 0;
    int DNLWinCount = 0;
    int NLDrawCount = 0;
public:
    void upTotalCheck() { TotalCheck++; }
    void upDCheckWin() { DCheckWin++; }
    void upDCheckLose() { DCheckLose++; }
    void upDCheckWinPreTurn() { DCheckWinPreTurn++; }
    void upDCheckLosePreTurn() { DCheckLosePreTurn++; }
    void upDCheckWinPreTurnScore(int i) { DCheckWinPreTurnScore[i]++; }
    void upDCheckLosePreTurnScore(int i) { DCheckLosePreTurnScore[i]++; }
    void upDCheckWinPostTurn() { DCheckWinPostTurn++; }
    void upDCheckLosePostTurn() { DCheckLosePostTurn++; }
    void upDCheckWinPostTurnScore(int i) { DCheckWinPostTurnScore[i]++; }
    void upDCheckLosePostTurnScore(int i) { DCheckLosePostTurnScore[i]++; }
    void upDLastWin() { DLastWin++; }
    void upDLastLose() { DLastLose++; }
    void upDCheckWinAgainstSizePreTurn(int i) { DCheckWinAgainstSizePreTurn[i]++; }
    void upDCheckLoseAgainstSizePreTurn(int i) { DCheckLoseAgainstSizePreTurn[i]++; }
    void upDCheckWinAgainstSizePostTurn(int i) { DCheckWinAgainstSizePostTurn[i]++; }
    void upDCheckLoseAgainstSizePostTurn(int i) { DCheckLoseAgainstSizePostTurn[i]++; }

    void upXBCount() { XBCount++; }
    void upPNLCount() { PNLCount++; }
    void upDNLCount() { DNLCount++; }
    void upXBDrawCount() { XBDrawCount++; }
    void upPNLWinCount() { PNLWinCount++; }
    void upDNLWinCount() { DNLWinCount++; }
    void upNLDrawCount() { NLDrawCount++; }

    void printStat() override {
        Stat::printStat();
        cout << "XI DACH SPECIFIC STATS: " << '\n';
        cout << "Dealer manually checks " << TotalCheck << " hands total" << '\n';
        cout << "Dealer wins by manually checking " << DCheckWin << " hands total" << '\n';
        cout << "Dealer loses by manually checking " << DCheckLose << " hands total" << '\n';
        cout << "Dealer wins by checking before dealer's turn to draw " << DCheckWinPreTurn << " hands total" << '\n';
        cout << "With score from 15 - 21: " << '\n';
        for (int i = 15; i < 22; i++) cout << DCheckWinPreTurnScore[i] << " - ";
        cout << "Against player's hand that has 2 - 4 cards" << '\n';
        for (int i = 2; i < 5; i++) cout << DCheckWinAgainstSizePreTurn[i] << " - ";
        cout << '\n' << "Dealer loses by checking before dealer's turn to draw " << DCheckLosePreTurn << " hands total" << '\n';
        cout << "With score from 15 - 21: " << '\n';
        for (int i = 15; i < 22; i++) cout << DCheckLosePreTurnScore[i] << " - ";
        cout << "Against player's hand that has 2 - 4 cards" << '\n';
        for (int i = 2; i < 5; i++) cout << DCheckLoseAgainstSizePreTurn[i] << " - ";
        cout << '\n' << "Dealer wins by manually checking at dealer's turn to draw " << DCheckWinPostTurn << " hands total" << '\n';
        cout << "With score from 15 - 21: " << '\n';
        for (int i = 15; i < 22; i++) cout << DCheckWinPostTurnScore[i] << " - ";
        cout << "Against player's hand that has 2 - 4 cards" << '\n';
        for (int i = 2; i < 5; i++) cout << DCheckWinAgainstSizePostTurn[i] << " - ";
        cout << '\n' << "Dealer loses by manually checking at dealer's turn to draw " << DCheckLosePostTurn << " hands total" << '\n';
        cout << "With score from 15 - 21: " << '\n';
        for (int i = 15; i < 22; i++) cout << DCheckLosePostTurnScore[i] << " - ";
        cout << "Against player's hand that has 2 - 4 cards" << '\n';
        for (int i = 2; i < 5; i++) cout << DCheckLoseAgainstSizePostTurn[i] << " - ";

        cout << '\n' << "Dealer wins at final check (including against NGULINH) " << DLastWin << " hands total" << '\n';
        cout << "Dealer loses at final check (including against NGULINH) " << DLastLose << " hands total" << '\n';

        cout << "There are " << XBCount << " XIBANG hands total" << '\n';
        cout << "XIBANG tie " << XBDrawCount << " times total" << '\n';
        cout << "Players have NGULINH " << PNLCount << " hands total" << '\n';
        cout << "Player wins with NGULINH " << PNLWinCount << " hands total" << '\n';
        cout << "Dealer has NGULINH " << DNLCount << " hands total" << '\n';
        cout << "Dealer wins with NGULINH " << DNLWinCount << " hands total" << '\n';
        cout << "NGULINH tie " << NLDrawCount << " times total" << '\n';
    }
};
    
int compareHands(Hand& a, Hand& b) {
    if ((a.isBust && b.isBust) && a.resolved) return 0; //only in XD mode where players will have flag resolved being true when comparing
    if (a.isBust) {
        return -1;
    }
    if (b.isBust) {
        return 1;
    }
    HandType ta = a.getHandType();
    HandType tb = b.getHandType();

    if (ta != tb) {
        return (ta > tb) ? 1 : -1; //Compare types;
    }

    if (a.getScore() != b.getScore()) {
        return (a.getScore() > b.getScore()) ? 1 : -1; //Tie types -> compare score
    }

    return 0; //Tie
}

void doResultBJ(Hand& p, Hand& d, int& result, StatBJ& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: ";
        //p.printHand();
        stat.upPWin();
        if (p.getHandType() == XIDACH) stat.upPBJWin(); //wins due to BJ won't count for any stat below
        else {
            stat.upPWinVSup(d.getFaceup());
            if (p.getScore() == 16) stat.upPWin16();
            if (p.first2HardScore > 0) stat.upPWinFirst2Hard(p.first2HardScore);
                else stat.upPWinFirst2Soft(p.first2SoftScore);
            if (d.isBust) stat.upPWinDBust();
            if (p.isSoft) stat.upPWinSoft(p.getScore());
                else stat.upPWinHard(p.getScore());
            stat.upPTotalHit(p.hand.size() - 2);
            if (p.hand.size() - 2) stat.upPHitWin();
                else stat.upPStandedWin();
            stat.upPWinAfterHitTime(p.hand.size() - 2);
            if (p.SplitWon) {
                stat.upSplitWin();
                stat.upPSplitWinRank(rankTable[p.hand[0]]);
                stat.upPSplitWinVSup(d.getFaceup());
            }
        }
    } else if (result == -1) {
        //cout << '\n' << "Dealer wins against player's hand: ";
        //p.printHand();
        stat.upDWin();
        stat.upPLoseVSup(d.getFaceup());
        stat.upPLoseAfterHitTime(p.hand.size() - 2);
        if (p.first2HardScore > 0) stat.upPLoseFirst2Hard(p.first2HardScore);
            else stat.upPLoseFirst2Soft(p.first2SoftScore);
        if (d.getHandType() == XIDACH) stat.upDBJWin();
        if (p.getScore() == 16) stat.upPLose16();
        stat.upPTotalHit(p.hand.size() - 2);
        if (p.SplitLost) {
            stat.upPSplitLoseVSup(d.getFaceup());
        }
    } else {
        //cout << '\n' << "Draw with player's hand: ";
        //p.printHand();
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

void doResultXD(Hand& p, Hand& d, int& result, StatXD& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: " << '\n';
        //p.printHand();
        stat.upPWin();
        if (p.getHandType() == XIDACH) stat.upPBJWin(); //wins due to BJ won't count for any stat below
        else if (p.getHandType() == XIBANG) stat.upXBCount();
        else {
            if (p.getScore() == 16) stat.upPWin16();
            if (p.first2HardScore > 0) stat.upPWinFirst2Hard(p.first2HardScore);
                else stat.upPWinFirst2Soft(p.first2SoftScore);
            if (d.isBust) stat.upPWinDBust();
            if (p.isSoft) stat.upPWinSoft(p.getScore());
                else stat.upPWinHard(p.getScore());
            stat.upPTotalHit(p.hand.size() - 2);
            if (p.hand.size() - 2) stat.upPHitWin();
                else stat.upPStandedWin();
            stat.upPWinAfterHitTime(p.hand.size() - 2);
        }
    } else if (result == -1) {
        //cout << '\n' << "Dealer wins against player's hand: "<< '\n';
        //p.printHand();
        stat.upDWin();
        stat.upPLoseAfterHitTime(p.hand.size() - 2);
        if (d.getHandType() == XIBANG) stat.upXBCount();
        if (p.first2HardScore > 0) stat.upPLoseFirst2Hard(p.first2HardScore);
            else stat.upPLoseFirst2Soft(p.first2SoftScore);
        if (d.getHandType() == XIDACH) stat.upDBJWin();
        if (p.getScore() == 16) stat.upPLose16();
        stat.upPTotalHit(p.hand.size() - 2);
        
    } else {
        //cout << '\n' << "Draw with player's hand: "<< '\n';
        //p.printHand();
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
        if (d.getHandType() == XIBANG) {
            stat.upXBCount();
            stat.upXBCount();
            stat.upXBDrawCount();
        }
        if (d.getHandType() == NGULINH) {
            stat.upDNLCount();
        }
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

int playerWantsBJ(Hand& p, Hand& d) { //Modify this to change the strategy of players. Note that dealer always stand on 17 or higher
    /*0: stand
      1: hit
      2: split
      3: double down
    */
    p.calculateScore();
    if (p.getScore() >= 21) return 0; //best possible score
    if (p.isSplitable) return 2;
    if (p.getScore() < 16) return 1;
    if (p.isSoft) return 1;
    if (p.getScore() == 16) {
        if (d.getFaceup() == 1) return 1;
    }
    return 0;
}

int playerWantsXD(Hand& p, Hand& d) { //Modify this to change the strategy of players.
    /*0: stand
      1: hit
    */
    p.calculateScore();
    if (p.getScore() >= 21) return 0; //best possible score
    if (p.getScore() <= 16) return 1; //always hit on 16, stand otherwise
    if (p.isSoft && p.getScore() < 19) return 1; //always hit if soft hand < 19
    return 0;
}

bool dealerWantsToCheck(Hand& p, Hand& d) {
    if (d.getScore() < 15) return STAND;
    if (d.getScore() > 18) return XET; //dealer 19+ will always check hand;
    if (p.hand.size() > 2 && d.getScore() > 16) return XET; //dealer 17+ will check player with 3+ cards hand
    if (p.hand.size() > 3 && d.getScore() > 14) return XET;//dealer 15+ will check player with 4 cards hand
    return 0;
}

int dealerWantsToAction(Match& match, Hand& d) {
    int s = d.getScore();
    if (d.hand.size() == 5) return XETALL;
    if (s < 15) return HIT; //obvious
    /* We have 2 options for strategies here:
    1: If there are players holding 4 cards and dealer has enough just score to check, check them
       Then for players holding 3 cards, dealer should have a threshold (e.g. 17) to check
       Then for players who didn't hit, the threshold should be higher (e.g. 19+) to check
       Uncomment to test/change this strategy
       */
    //if (match.get4() != 0) return XET4; //if checking all 4-card player, get4() will return 0, don't worry
    //else if (match.get3() != 0 && s > 16) return XET3;
    //else if (s > 18) return XET3;
    //else if (!d.isBust) return HIT;

    // 2: "Dynamicly according to soft/hard hands as below"
    if (d.isSoft && (s == 18 || s == 19)) return (match.get3() || match.get4() ? XET3 : HIT); //soft 18 19: check 3+ card hand, else hit ()
    if (d.isSoft && (s == 20 || s == 21)) return XETALL; //soft 20 21: check all
    if (d.isSoft && (s == 16 || s == 17)) return (match.get4() ? XET4 : HIT); //soft 16 17: check 4 card hand
    if (!d.isSoft && (s >= 16 && s <= 18)) return (match.get4()) ? XET4 : HIT; //hard 16-18: check 4 card hand
    if (!d.isSoft && (s >= 19 && s <= 21)) return XETALL; //hard 19-21: check all
    return XETALL;
}

void ProcessSimulationBJ(Match& match, Dealer& dealer, StatBJ& stat) {
    //cout << "Dealer's first hand: ";
    //dealer.printDealerHand();
    dealer.hands[0].calculateScore();
    while (dealer.hands[0].getScore() < 17) {
        match.dealCardToPlayer(dealer, 0);
        dealer.hands[0].calculateScore();
    }
    //cout << '\n' << "Dealer's final hand: ";
    //dealer.printDealerHand();
    for (auto& p : match.players) {
        for (int i = 0; i < p.hands.size(); i++) {
            bool resolved = false;

            while (true) {
                Hand& h = p.hands[i];
                h.calculateScore();

                if (h.getHandType() == XIDACH || dealer.hands[0].getHandType() == XIDACH) {
                    int result = compareHands(h, dealer.hands[0]);
                    if (result == 1 && p.hands.size() > 1) h.setWon();
                    if (result == -1 && p.hands.size() > 1) h.setLost();
                    doResultBJ(h, dealer.hands[0], result, stat);
                    resolved = true;
                    break;
                }

                int action = playerWantsBJ(h, dealer.hands[0]);

                if (h.isBust || action == 0) {
                    break;
                }
                if (action == 2) { // SPLIT
                    match.splitTo(p, i);
                    stat.upTotalSplit(1);
                    continue;
                }
                if (action == 1) { // HIT
                    match.dealCardToHand(h);
                    continue;
                }
                break;
            }

            if (!resolved) {
                Hand& h = p.hands[i];
                int result = compareHands(h, dealer.hands[0]);
                if (result == 1 && p.hands.size() > 1) h.setWon();
                if (result == -1 && p.hands.size() > 1) h.setLost();
                doResultBJ(h, dealer.hands[0], result, stat);
            }
        }
    }
    //cout << '\n';
}

void SimulationBJ() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    Match match;
    StatBJ stat;
    match.dealer.hands[0].reset();
    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(Player());
    }
    for (int z = 0; z < u; z++) {
        match.deckReset();
        match.shuffle();
        match.dealer.hands[0].reset();
        for (auto& p : match.players) {
            p.resetPlayer();
        }
        
        match.deal2Card();

        ProcessSimulationBJ(match, match.dealer, stat);
    }
    cout << '\n';
    stat.printStat();
}

bool DealerWantsBJ(Match& match, Dealer& d) {
    d.hands[0].calculateScore();
    if (d.hands[0].getScore() < 17) return true;
    return false;
}

void HitPlay(Match& match, Dealer& d, int i) {
    match.dealCardToHand(match.players[0].hands[i]);
    cout << "You just hit for: ";
    match.players[0].hands[i].printNewCard();
    cout << '\n' << "Your hand number " << i + 1 << " is now ";
    match.players[0].hands[i].printHand();
    if (match.players[0].hands[i].isBust) {
        cout << '\n' << "This hand is busted! Bad luck ..." << '\n';
        return;
    }
}

void ProcessPlayBJ(Match& match, Dealer& d) {
    Stat stat;
    cout << "Dealer's hand: ";
    d.printDealerFirstHand();
    cout << '\n' << "Your hand: ";
    match.players[0].printHand();
    cout << '\n';

    if (d.hands[0].getHandType() == XIDACH && match.players[0].hands[0].getHandType() != XIDACH) {
        cout << "Dealer has BlackJack and you don't. You lost!" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') PlayBJ();
        else return;
    }
    else if (d.hands[0].getHandType() != XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "You have BlackJack and Dealer doesn't. You win!" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') PlayBJ();
        else return;
    }
    else if (d.hands[0].getHandType() == XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "Both you and dealer have BlackJack. This round is a tie" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') PlayBJ();
        else return;
    }
    for (int i = 0; i < match.players[0].hands.size(); i++) {
        match.players[0].hands[i].calculateScore();
        if (match.players[0].hands[i].isSplitable) {
            cout << "Split? (s = Split, n = No)" << '\n';
            char s; cin >> s;
            if (s == 's') {
                match.splitTo(match.players[0], i);
            }
            cout << "Your hands are now: ";
            match.players[0].printHand();
            i--;
            continue; //to see if the new hand is splitable again
        }
        else {
            while (true) {
                if (match.players[0].hands[i].isBust) break;
                if (match.players[0].hands[i].getScore() < 16) {
                    cout << "You must hit now! Any key to proceed" << '\n';
                    char x; cin >> x;
                    HitPlay(match, d, i);
                    continue;
                }
                if (!match.players[0].hands[i].isBust) {
                    cout << '\n' << "Choose action for your hand number " << i + 1 << ": h = Hit, s = Stand" << '\n';
                    char s; cin >> s;
                    if (s == 'h') {
                        HitPlay(match, d, i);
                        continue;
                    }
                    else if (s == 's') {
                        break;
                    }
                }
            }
        }
    }
    cout << "You chose Stand (or busted, haha) on all of your hands. Here comes the moment of truth ..." << '\n';
    cout << "Dealer's first hand is: ";
    d.hands[0].printHand();
    while (DealerWantsBJ(match, d)) {
        match.dealCardToHand(d.hands[0]);
        cout << '\n' << "Dealer just draw: ";
        d.hands[0].printNewCard();
    }
    cout << '\n' << "Dealer's final hand: ";
    d.hands[0].printHand();

    for (int i = 0; i < match.players[0].hands.size(); i++) {
        int result = compareHands(match.players[0].hands[i], d.hands[0]);                
        if (result == 1) {
            cout << '\n' << "Congrats! Your hand number " << i + 1 << " beats the Dealer!" << '\n';
        }
        else if (result == -1) {
            cout << '\n' << "Oh no! Dealer beats your hand number " << i + 1 << '\n';
        }
        else if (result == 0) {
            cout << '\n' << "Your hand number " << i + 1 << " ties with Dealer" << '\n';
        }
    }
    return;
}

void PlayBJ() {
    Match match;
    match.dealer.hands[0].reset();
    match.addPlayer(Player());
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlayBJ(match, match.dealer);
}

void ProcessSimulationXD(Match& match, Dealer& dealer, StatXD& stat) {
    Hand& d = dealer.hands[0];
    d.calculateScore();
    //cout << "Dealer's hand: ";
    //d.printHand();
    //cout << '\n';
    if (d.getHandType() == XIDACH || d.getHandType() == XIBANG) {
        int t = d.getHandType();
        for (auto& p : match.players) {
            //cout << "Player's hand: ";
            //p.printHand();
            //cout << '\n';
            p.hands[0].calculateScore();
            int result = compareHands(p.hands[0], d);
            doResultXD(p.hands[0], d, result, stat); //the match ends now
        }
        return;
    }
    for (auto& p : match.players) {
        while (true) {
            //cout << "This player's hand: ";
            //p.hands[0].printHand();
            //cout << '\n';
            if (p.hands[0].hand.size() == 5) {
                if (p.hands[0].getHandType() == NGULINH) stat.upPNLCount();
                break;
            }
            int action = playerWantsXD(p.hands[0], d);
            if (action == 1) {
                match.dealCardToHand(p.hands[0]);
                //cout << "Just hit for: ";
                //p.hands[0].printNewCard();
                //cout << '\n';
                continue;
            }
            if (action == 0) {
                //cout << "Standed!" << '\n';
                break;
            }
        }
        if (dealerWantsToCheck(p.hands[0], d)) {
            stat.upTotalCheck();
            //cout << "Dealer checks this hand before Dealer's turn to draw" << '\n';
            match.deductPlayerCount();
            p.hands[0].setSolved();
            int result = compareHands(p.hands[0], d);
            if (result == 1) {
                stat.upDCheckWin();
                stat.upDCheckWinPreTurn();
                stat.upDCheckWinPreTurnScore(d.getScore());
                stat.upDCheckWinAgainstSizePreTurn(p.hands[0].hand.size());
            }
            else if (result == -1) {
                stat.upDCheckLose();
                stat.upDCheckLosePreTurn();
                stat.upDCheckLosePreTurnScore(d.getScore());
                stat.upDCheckLoseAgainstSizePreTurn(p.hands[0].hand.size());
            }
            doResultXD(p.hands[0], d, result, stat);
        }
        continue;
    }
    for (auto& p : match.players) {
        if (p.hands[0].hand.size() == 3 && !p.hands[0].resolved) match.set3();
        if (p.hands[0].hand.size() == 4 && !p.hands[0].resolved) match.set4();
    }
    //cout << "Dealer's turn to draw starts now" << '\n';
    while (true) {
        if (match.getPlayerCount() == 0) break;
        int action = dealerWantsToAction(match, d);
        if (action == 1) {
            match.dealCardToHand(d);
            //cout << "Just hit for: ";
            //d.printNewCard();
            //cout << '\n' << "Dealer's hand is now: ";
            //d.printHand();
            //cout << '\n';
            d.calculateScore();
            if (d.isBust) {
                //cout << "Dealer Busted!" << '\n';
                for (auto& p : match.players) {
                    if (!p.hands[0].resolved) {
                        int result = compareHands(p.hands[0], d);
                        doResultXD(p.hands[0], d, result, stat);
                    }
                }
                return;
            }
            continue;
        }
        else if (action == 2 || action == 3) {
            if (match.getPlayerCount() != 0) {
                for (auto& p : match.players) {
                    if (p.hands[0].hand.size() >= (action + 1) && !p.hands[0].resolved) {
                        //cout << "Dealer checks this player hand: ";
                        //p.printHand();
                        //cout << '\n';
                        stat.upTotalCheck();
                        match.deductPlayerCount();
                        if (p.hands[0].hand.size() == 4) match.no4();
                        if (p.hands[0].hand.size() == 3) match.no3();
                        p.hands[0].setSolved();
                        int result = compareHands(p.hands[0], d);
                        if (result == -1) {
                            stat.upDCheckWin();
                            stat.upDCheckWinPostTurn();
                            stat.upDCheckWinPostTurnScore(d.getScore());
                            stat.upDCheckWinAgainstSizePostTurn(p.hands[0].hand.size());
                        }
                        else if (result == 1) {
                            stat.upDCheckLose();
                            stat.upDCheckLosePostTurn();
                            stat.upDCheckLosePostTurnScore(d.getScore());
                            stat.upDCheckLoseAgainstSizePostTurn(p.hands[0].hand.size());
                        }
                        doResultXD(p.hands[0], d, result, stat);
                    }
                }
            }
            continue;
        }
        //cout << "Dealer stand" << '\n';
        //cout << "Dealer's final hand: ";
        //d.printHand();
        if (d.getHandType() == NGULINH) stat.upDNLCount();
        if (d.isBust) stat.upDBustCount();
        for (auto& p : match.players) {
            if (!p.hands[0].resolved) {
                //cout << '\n' << "Dealer checks this player hand: ";
                //p.printHand();
                p.hands[0].setSolved();
                int result = compareHands(p.hands[0], d);
                if (result == 1) {
                    stat.upDLastLose();
                    if (p.hands[0].getHandType() == XIBANG) stat.upXBCount();
                    if (p.hands[0].getHandType() == NGULINH) stat.upPNLWinCount();
                }
                else if (result == -1) {
                    stat.upDLastWin();
                    if (d.getHandType() == NGULINH) stat.upDNLWinCount();
                }
                else if (result == 0) {
                    if (d.getHandType() == NGULINH) stat.upNLDrawCount();
                    if (d.isBust && p.hands[0].isBust) stat.upBustDraw();
                }
                doResultXD(p.hands[0], d, result, stat);
            }
        }
        break;
    }
    //cout << '\n';
}

void SimulationXD() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    Match match;
    StatXD stat;
    match.dealer.hands[0].reset();
    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(Player());
    }
    match.setPlayerCount();
    for (int z = 0; z < u; z++) {
        match.deckReset();
        match.shuffle();
        match.no3();
        match.no4();
        match.dealer.hands[0].reset();
        for (auto& p : match.players) {
            p.resetPlayer();
        }
        
        match.deal2Card();

        ProcessSimulationXD(match, match.dealer, stat);
    }
    cout << '\n';
    stat.printStat();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    initRankTable(rankTable);
    initRankName(rankName);

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

        if (choice == 1) SimulationBJ();
        else if (choice == 2) PlayBJ();
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

        if (choice == 1) SimulationXD();
        //else if (choice == 2) PlayXD();
        return 0;
    }
    return 0;
}