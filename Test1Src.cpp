#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

int rankTable[53];

void initRankTable(int rankTable[]) {
    for (int i = 1; i <= 52; i++) {
        int r = (i - 1) % 13 + 1; //Return ranks
        rankTable[i] = (r > 10) ? 10 : r; //Return points
    }
}
//Deck will be A of Spades (1) to K of Spades (13). Then Clubs, Diamonds and Hearts
string getSuit(int x) {
    int suit = (x - 1) / 13;
    if (suit == 0) return " of Spades";
    if (suit == 1) return " of Clubs";
    if (suit == 2) return " of Diamonds";
    if (suit == 3) return " of Hearts";
    return " of Default";
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

class Player {  
public:
    vector<int> hand;
    int score = 0;
    bool isBust = false;
    bool hasAce = false;
    bool isSoft = false;
    int softScore = 0;
    int first2SoftScore = 0;
    int first2HardScore = 0;
    vector<int> rank;

    Player() : score(0), isBust(false), hasAce(false) {}

    virtual ~Player() {}

    void reset() {
        hand.clear();
        rank.clear();
        isBust = false;
        hasAce = false;
        score = 0;
    }

    void calculateScore() {
        isBust = false;
        hasAce = false;
        isSoft = false;
        rank.clear();
        score = 0;
        softScore = 0;
        first2SoftScore = 0;
        first2HardScore = 0;
        for (auto c : hand) {
            int r = rankTable[c];
            rank.push_back(r);
            if (r == 1) hasAce = true;
            score += r;
        }

        if (hasAce) {
            if ((score + 10) <= 21) isSoft = true;
        }

        if (hasAce && (score + 10) < 22) {
            score += 10;
            softScore = score;
            if (hand.size() == 2) first2SoftScore = score;
        }

        if (!isSoft && hand.size() == 2) first2HardScore = score;

        if (score > 21) {
            isBust = true;
        }
    }

    int getRank(int x) {
        return rank[x];
    }

    void receiveCard(int x) {
        hand.push_back(x);
        calculateScore();
    }

    HandType getHandType() {
        if (hand.size() == 2) {
            int r1 = rank[0];
            int r2 = rank[1];
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
};

class Dealer : public Player {
public:
    int getFaceup() {
        int r = rankTable[hand[0]];
        return r;
    }
};

class Match {
protected:
    vector<int> deck;
    int playerCount;
    int cardIdx = 0;
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

    void dealCardTo(Player& p) {
        p.receiveCard(drawCard());
    }

    void deal2Card() {
        for (Player& p : players) {
            p.hand.push_back(drawCard());
            p.hand.push_back(drawCard());
            p.calculateScore();
        }

        dealer.hand.push_back(drawCard());
        dealer.hand.push_back(drawCard());
        dealer.calculateScore();
    }
};

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
    int PWinFirst2Soft [22] = {0}; //if player wins, track the score of first 2 cards (soft or hard)
    int PWinFirst2Hard [22] = {0};
    int PWinVSup [11] = {0}; //if player wins, track the up card of dealer
    int PLoseVSup [11] = {0}; //if player loses, track the up card of dealer
    int PTotalHit = 0;
    int PHitWin = 0;
    int PStandedWin = 0;
    int PStandSoftWin = 0; //if player loses, but would have won if they stand at the last soft hand
    int PWinAfterHitTime[12] = {0};  //track how many hits if player wins
    int PLoseAfterHitTime[12] = {0}; //track how many hits if player loses
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
    void upPWinVSup(int i) { PWinVSup[i]++; }
    void upPLoseVSup(int i) { PLoseVSup[i]++; }
    void upPWinAfterHitTime(int i) { PWinAfterHitTime[i]++; }
    void upPLoseAfterHitTime(int i) { PLoseAfterHitTime[i]++; }

    

    void printStat() {
        cout << "Player wins " << PWin << " times" << '\n';
        cout << "Dealer wins " << DWin << " times" << '\n';
        cout << "Draw " << Draw << " times" << '\n';
        cout << "Player wins with BlackJack " << PBJWin << " times" << '\n';
        cout << "Dealer wins with BlackJack " << DBJWin << " times" << '\n';
        cout << "Draw with BlackJack " << BJDraw << " times" << '\n';
        cout << "Player wins on 16 " << PWin16 << " times" << '\n';
        cout << "Player loses on 16 " << PLose16 << " times" << '\n';
        cout << "Player wins and Dealer busts " << PWinDBust << " times" << '\n';
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
        for (int i = 1; i < 11; i++) cout << PWinVSup[i] << " - ";
        cout << '\n' << "Player loses against dealer up card ranked 1 to 10: " << '\n';
        for (int i = 1; i < 11; i++) cout << PLoseVSup[i] << " - ";
        cout << '\n' << "Player hits a total of: " << PTotalHit << " times" << '\n';
        cout << '\n' << "Player wins after hit: " << PHitWin << " times" << '\n';
        cout << '\n' << "Player wins without hit: " << PStandedWin << " times" << '\n';
        cout << '\n' << "Player wins this many times after 0 - 1 - 2 - ... - 10 hits: " << '\n';
        for (int i = 0; i < 11; i++) cout << PWinAfterHitTime[i] << " - ";
        cout << '\n' << "Player loses this many times after 0 - 1 - 2 - ... - 10 hits: " << '\n';
        for (int i = 0; i < 11; i++) cout << PLoseAfterHitTime[i] << " - ";
    }
};
    
int compareHands(Player& a, Player& b) {
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

void printHand(Player& p) {
    for (auto c : p.hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
}

void doResult(Player& p, Dealer& d, int& result, Stat& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: ";
        //printHand(p);
        stat.upPWin();
        if (p.getScore() == 16) stat.upPWin16();
        if (p.getHandType() == XIDACH) stat.upPBJWin();
        else {
            stat.upPWinVSup(d.getFaceup());
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
        //cout << '\n' << "Dealer wins against player's hand: ";
        //printHand(p);
        stat.upDWin();
        stat.upPLoseVSup(d.getFaceup());
        stat.upPLoseAfterHitTime(p.hand.size() - 2);
        if (p.first2HardScore > 0) stat.upPLoseFirst2Hard(p.first2HardScore);
            else stat.upPLoseFirst2Soft(p.first2SoftScore);
        if (d.getHandType() == XIDACH) stat.upDBJWin();
        if (p.getScore() == 16) stat.upPLose16();
        
    } else {
        //cout << '\n' << "Draw with player's hand: ";
        //printHand(p);
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
    }
}

bool playerWants(Player& p, Dealer& d) { //Modify this to change the strategy of players. Note that dealer always stand on 17 or higher
    p.calculateScore();
    if (p.getScore() >= 21) return false; //best possible score
    if (p.getScore() < 16) return true;
    if (p.isSoft) return true;
    if (p.getScore() == 16) {
        if (d.getFaceup() == 1) return true;
    }
    return false;
}

void Process(Match& match, Dealer& dealer, Stat& stat) {
    //cout << "Dealer's first hand: ";
    //printHand(dealer);
    dealer.calculateScore();
    while (dealer.getScore() < 17) {
        match.dealCardTo(dealer);
        dealer.calculateScore();
    }
    //cout << '\n' << "Dealer's final hand: ";
    //printHand(dealer);
    for (auto& p : match.players) {
        p.calculateScore();
        if (p.getHandType() == XIDACH || dealer.getHandType() == XIDACH) {
            int result = compareHands(p, dealer);
            doResult(p, dealer, result, stat);
            continue;
        }

        while(playerWants(p, dealer)) {
            match.dealCardTo(p);
        }

        int result = compareHands(p, dealer);
        doResult(p, dealer, result, stat);
    }
    //cout << '\n';
}

int main() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    initRankTable(rankTable);
    
    Match match;
    Stat stat;
    match.dealer.reset();
    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(Player());
    }
    for (int z = 0; z < u; z++) {
        match.deckReset();
        match.shuffle();
        match.dealer.reset();
        for (auto& p : match.players) {
            p.reset();
        }
        
        match.deal2Card();

        Process(match, match.dealer, stat);
    }
    cout << '\n';
    stat.printStat();
    return 0;
}