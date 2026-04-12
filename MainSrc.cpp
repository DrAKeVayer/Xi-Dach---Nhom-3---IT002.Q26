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
        score = 0;
        for (auto c : hand) {
            int r = rankTable[c];
            rank.push_back(r);
            if (r == 1) hasAce = true;
            score += r;
        }

        if (hasAce && (score + 10) < 22) {
            score += 10; //for this code, one of the aces always counts as 11 if possible
        }

        if (score > 21) {
            isBust = true;
        }
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
};

class Dealer : public Player {
public:

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
        if (deck.empty()) return -1; //never happens, because we will impose a limit of 10 players max
        int card = deck[cardIdx];
        cardIdx++;
        return card;
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
    
int compareHands(Player& a, Player& b) {
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

void Process(Match& match, Dealer& dealer, int& DWin, int& Draw, int& PWin) {
    //cout << "Dealer's hand: ";
    //printHand(*dealer);
    for (auto& p : match.players) {
        int result = compareHands(p, dealer);
        
        if (result == 1) {
            //cout << '\n' << "Player wins with hand: ";
            //printHand(*p);
            PWin++;
        } else if (result == -1) {
            //cout << '\n' << "Dealer wins against player's hand: ";
            //printHand(*p);
            DWin++;
        } else {
            //cout << '\n' << "Draw with player's hand: ";
            //printHand(*p);
            Draw++;
        }
    }
}

int main() {
    cout << "How many players?" << '\n';
    int n; cin >> n;

    int DWin = 0;
    int Draw = 0;
    int PWin = 0;

    initRankTable(rankTable);
    
    Match match;
    match.dealer.reset();
    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(Player());
    }
    for (int z = 0; z < 1000000; z++) {
        match.deckReset();
        match.shuffle();
        match.dealer.reset();
        for (auto& p : match.players) {
            p.reset();
        }
        
        match.deal2Card();

        Process(match, match.dealer, DWin, Draw,PWin);
    }
    cout << "Dealer wins " << DWin << " matches" << '\n';
    cout << "Player wins " << PWin << " matches" << '\n';
    cout << "Draw " << Draw << " matches" << '\n';
    return 0;
}
