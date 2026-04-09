#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
using namespace std;

int getRank(int x) {
    int rank = (x - 1) % 13 + 1;
    if (rank > 10) {
        return 10;
    }
    return rank;
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

    Player() : score(0), isBust(false), hasAce(false) {}

    virtual ~Player() {}

    void reset() {
        hand.clear();
        isBust = false;
        hasAce = false;
        score = 0;
    }

    void calculateScore() {
        isBust = false;
        hasAce = false;
        score = 0;
        for (auto c : hand) {
            if (getRank(c) == 1) hasAce = true;
            score += getRank(c);
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
            int r1 = getRank(hand[0]);
            int r2 = getRank(hand[1]);
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
public:
    vector<Player*> players;
    Dealer* dealer;

    Match() {
        deckInit();
        shuffle();
    }

    ~Match() {
        for (Player* p : players) {
            delete p; 
        }
        players.clear();
    }

    void addPlayer(Player* p) {
        players.push_back(p);
    }

    void setDealer(Dealer* d) {
        dealer = d;
    }

    void deckInit() {
        for (int i = 1; i < 53; i++) {
            deck.push_back(i);
        }
    }

    void shuffle() {
        unsigned seed = static_cast<unsigned>(
            chrono::high_resolution_clock::now().time_since_epoch().count()
        );
        mt19937 gen(seed);

        std::shuffle(deck.begin(), deck.end(), gen);
    }

    int drawCard() {
        if (deck.empty()) return -1; //never happens, because we will impose a limit of 10 players max
        int card = deck.back();
        deck.pop_back();
        return card;
    }

    void deal2Card() {
        for (Player* p : players) {
            p->hand.push_back(drawCard());
            p->hand.push_back(drawCard());
            p->calculateScore();
        }

        dealer->hand.push_back(drawCard());
        dealer->hand.push_back(drawCard());
        dealer->calculateScore();
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

void Process(Match& match, Dealer* dealer) {
    cout << "Dealer's hand: ";
    printHand(*dealer);
    for (auto p : match.players) {
        int result = compareHands(*p, *dealer);
        
        if (result == 1) {
            cout << '\n' << "Player wins with hand: ";
            printHand(*p);
        } else if (result == -1) {
            cout << '\n' << "Dealer wins against player's hand: ";
            printHand(*p);
        } else {
            cout << '\n' << "Draw with player's hand: ";
            printHand(*p);
        }
    }
}

int main() {
    Match match;
    Dealer* dealer = new Dealer(); //always has dealer
    match.setDealer(dealer);

    cout << "How many players?" << '\n';
    int n; cin >> n;

    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(new Player());
    }

    match.deal2Card();

    Process(match, dealer);

    return 0;
}
