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
    rank.clear(); // added this thing

    for (auto c : hand) {
        int r = rankTable[c];
        rank.push_back(r);
        if (r == 1) hasAce = true;
        score += r;
    }

    if (hasAce && (score + 10) < 22) {
        score += 10;
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


// Player chooses Hit or Stand (interactive mode)
void playerTurn(Player& p, Match& match) {
    while (true) {
        cout << "\nYour hand: ";
        printHand(p);
        cout << " | Score: " << p.getScore() << '\n';

        if (p.isBust) {
            cout << "You busted!\n";
            break;
        }

        cout << "Choose action: (h = Hit, s = Stand): ";
        char choice;
        cin >> choice;

        if (choice == 'h') {
            int card = match.drawCard();
            p.hand.push_back(card);
            p.calculateScore();
        }
        else if (choice == 's') {
            break;
        }
        else {
            cout << "Invalid input. Try again.\n";
        }
    }
}

// Simple AI: hit until score >= 16
void playerAuto(Player& p, Match& match) {
    while (p.getScore() < 16 && !p.isBust) {
        int card = match.drawCard();
        p.hand.push_back(card);
        p.calculateScore();
    }
}

// Dealer must hit until score >= 17
void dealerTurn(Dealer& dealer, Match& match) {
    while (dealer.getScore() < 17 && !dealer.isBust) {
        int card = match.drawCard();
        dealer.hand.push_back(card);
        dealer.calculateScore();
    }
}

// Reset all players + dealer + deck index
void resetRound(Match& match) {
    match.deckReset();

    match.dealer.reset();

    for (auto& p : match.players) {
        p.reset();
    }
}

int main() {
    initRankTable(rankTable);

    cout << "1. Simulation\n2. Play Game\nChoose mode: ";
    int mode;
    cin >> mode;

    // ===== SIMULATION MODE =====
    if (mode == 1) {
        cout << "How many players?\n";
        int n; cin >> n;

        int DWin = 0;
        int Draw = 0;
        int PWin = 0;

        Match match;

        // Add players
        for (int i = 0; i < n; i++) {
            match.addPlayer(Player());
        }

        // Run simulation
        for (int z = 0; z < 1000000; z++) {
            resetRound(match);
            match.shuffle();

            match.deal2Card();

            // Player AI turn
            for (auto& p : match.players) {
                playerAuto(p, match);
            }

            // Dealer turn
            dealerTurn(match.dealer, match);

            // Process result
            Process(match, match.dealer, DWin, Draw, PWin);
        }

        cout << "\n--- Simulation Result ---\n";
        cout << "Dealer wins " << DWin << " matches\n";
        cout << "Player wins " << PWin << " matches\n";
        cout << "Draw " << Draw << " matches\n";
    }

    // ===== GAME MODE =====
    else if (mode == 2) {
        cout << "How many players?\n";
        int n; cin >> n;

        Match match;

        for (int i = 0; i < n; i++) {
            match.addPlayer(Player());
        }

        while (true) {
            resetRound(match);
            match.shuffle();

            match.deal2Card();

            // Player turns (interactive)
            for (auto& p : match.players) {
                cout << "\n--- Player Turn ---\n";
                playerTurn(p, match);
            }

            // Dealer turn
            dealerTurn(match.dealer, match);
            cout << "\nDealer's hand: ";
            printHand(match.dealer);
            cout << " | Score: " << match.dealer.getScore() << '\n';

            // Show result
            int DWin = 0, Draw = 0, PWin = 0;
            Process(match, match.dealer, DWin, Draw, PWin);

            cout << "\nResult this round:\n";
            cout << "Dealer wins: " << DWin << '\n';
            cout << "Player wins: " << PWin << '\n';
            cout << "Draw: " << Draw << '\n';

            cout << "\nPlay again? (y/n): ";
            char c; cin >> c;
            if (c == 'n') break;
        }
    }

    else {
        cout << "Invalid mode.\n";
    }

    return 0;
}