#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <windows.h> //for print unicode symbols
using namespace std;

void Play();

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
    HIT = 0,
    STAND = 1,
    SPLIT = 2,
    DOUBLED = 3,
    XET = 4
};

class Hand {
public:
    vector<int> hand;
    int score = 0;
    bool isBust = false;
    bool hasAce = false;
    bool isSoft = false;
    bool isSplitable = false;
    int softScore = 0;
    int first2SoftScore = 0;
    int first2HardScore = 0;

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

    int getFaceup() {
        int r = rankTable[hand[0]];
        return r;
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

    void printHand() const {
        for (auto c : hand) {
            cout << getRankName(c);
            cout << getSuit(c) << ", ";
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
    
int compareHands(Hand& a, Hand& b) {
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

void doResult(Player& p, Dealer& d, int& result, Stat& stat) {
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
        
    } else {
        //cout << '\n' << "Draw with player's hand: ";
        //p.printHand();
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

int playerWants(Hand& p, Hand& d) { //Modify this to change the strategy of players. Note that dealer always stand on 17 or higher
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

void ProcessSimulation(Match& match, Dealer& dealer, Stat& stat) {
    //cout << "Dealer's first hand: ";
    //dealer.printHand();
    dealer.calculateScore();
    while (dealer.getScore() < 17) {
        match.dealCardTo(dealer);
        dealer.calculateScore();
    }
    //cout << '\n' << "Dealer's final hand: ";
    //dealer.printHand();
    for (auto& p : match.players) {
        for (int i = 0; i < p.hands.size(); i++) {
            bool resolved = false;

            while (true) {
                Hand& h = p.hands[i];
                h.calculateScore();

                if (h.getHandType() == XIDACH || dealer.hands[0].getHandType() == XIDACH) {
                    int result = compareHands(h, dealer.hands[0]);
                    doResult(h, dealer.hands[0], result, stat);
                    resolved = true;
                    break;
                }

                int action = playerWants(h, dealer.hands[0]);

                if (h.isBust || action == 0) {
                    break;
                }
                if (action == 2) { // SPLIT
                    match.splitTo(p, i);
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
                doResult(h, dealer.hands[0], result, stat);
            }
        }
    }
    //cout << '\n';
}

void Simulation() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    Match match;
    Stat stat;
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

        ProcessSimulation(match, match.dealer, stat);
    }
    cout << '\n';
    stat.printStat();
}

bool DealerWants(Match& match, Dealer& d) {
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

void ProcessPlay(Match& match, Dealer& d) {
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
        if (x == 'y') Play();
        else return;
    }
    else if (d.hands[0].getHandType() != XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "You have BlackJack and Dealer doesn't. You win!" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') Play();
        else return;
    }
    else if (d.hands[0].getHandType() == XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "Both you and dealer have BlackJack. This round is a tie" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') Play();
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
    while (DealerWants(match, d)) {
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

void Play() {
    Match match;
    match.dealer.hands[0].reset();
    match.addPlayer(Player());
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlay(match, match.dealer);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    initRankTable(rankTable);

    int choice;

    cout << R"(
    Type your mode:
    1: Simulation
    2: Play
    )";
    cin >> choice;

    if (choice == 1) Simulation();
    else if (choice == 2) Play();
    
    return 0;
}