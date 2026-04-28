#include "Constants.h"
#include "Engine.h"
#include "Entities.h"

Match::Match() : dealer(0) {
    players.reserve(15);
    deckInit();
    shuffle();
}

void Match::addPlayer(int i) {
    players.emplace_back(Player(i));
}

void Match::deckInit() {
    deck.clear();
    for (int i = 1; i < 53; i++) {
        deck.push_back(i);
    }
}

void Match::shuffle() {
    static unsigned seed = static_cast<unsigned>(
        chrono::high_resolution_clock::now().time_since_epoch().count()
        );
    static mt19937 gen(seed);

    std::shuffle(deck.begin(), deck.end(), gen);
}

void Match::deckReset() {
    cardIdx = 0;
}

int Match::drawCard() {
    int card = deck[cardIdx++];
    if (cardIdx > 51) {
        deckInit();
        shuffle();
        cardIdx = 0;
    }
    return card;
}

void Match::dealCardToPlayer(Player& p, int x) {
    p.hands[x].receiveCard(drawCard());
    p.hands[x].calculateScore();
}

void Match::dealCardToHand(Hand& h) {
    h.receiveCard(drawCard());
    h.calculateScore();
}

void Match::deal2Card() {
    for (Player& p : players) {
        p.hands[0].hand.push_back(drawCard());
        p.hands[0].hand.push_back(drawCard());
        p.hands[0].calculateScore();
    }

    dealer.hands[0].hand.push_back(drawCard());
    dealer.hands[0].hand.push_back(drawCard());
    dealer.hands[0].calculateScore();
}

int Match::compareHands(Hand& a, Hand& b) {
    HandType ta = a.getHandType();
    HandType tb = b.getHandType();
    if (a.resolved) {
        if (a.isBust && b.isBust) return 0;
        else if (a.isBust) return -1;
        else if (b.isBust) return 1;
    }
    else {
        if (a.isBust) return -1;
        if (b.isBust) return 1;
        if (ta == NGULINH || ta == XIBANG) ta = THUONG;
        if (a.splited && ta == XIDACH) ta = THUONG;
        if (tb == NGULINH || tb == XIBANG) tb = THUONG;
    }
    if (ta != tb) {
        return (ta > tb) ? 1 : -1; //Compare types;
    }

    if (a.getScore() != b.getScore()) {
        return (a.getScore() > b.getScore()) ? 1 : -1; //Tie types -> compare score
    }

    return 0; //Tie
}

void Match::HitPlay(Hand& h) {
    dealCardToHand(h);
    h.printNewCard();
    h.printHand();
    if (h.isBust) {
        PrintPBust(h);
    }
    return;
}

void Match::StandPlay(Player& p, Hand& h, Dealer& d) {
    cout << "You stood your hand number " << h.Pos << '\n';
    h.stood = true;
}

void Match::PrintResult(Hand& h, Dealer& d, int res) {
    if (res == 1) {
        cout << '\n' << "Congrats! Your hand no." << h.Pos << " beats the Dealer!" << '\n';
        h.upProfit(1.0);
    }
    else if (res == -1) {
        cout << '\n' << "Oh no! Dealer beats your hand no." << h.Pos << '\n';
        h.upProfit(-1.0);
    }
    else if (res == 0) {
        cout << '\n' << "Your hand no." << h.Pos << " ties with Dealer" << '\n';
    }
}

void Match::PrintResultXD(Player& p, Dealer& d, int res) {
    if (res == 1) {
        cout << '\n' << "Congrats! Player no." << p.Pos << " beats the Dealer with hand: ";
        p.hands[0].printJustHand();
        p.upProfit(1.0);
    }
    else if (res == -1) {
        cout << '\n' << "Oh no! Player no." << p.Pos << " loses to the Dealer with hand: ";
        p.hands[0].printJustHand();
        p.upProfit(-1.0);
    }
    else if (res == 0) {
        cout << '\n' << "Player no." << p.Pos << " ties the Dealer with hand: ";
        p.hands[0].printJustHand();
    }
}

void Match::PrintState(Player& p, Dealer& d) {
    cout << "Current state: " << '\n';
    d.printDealerFirstHand();
}

void Match::PrintPBust(Hand& h) {
    cout << "Your hand no." << h.Pos << " is Busted! Bad luck ..." << '\n';
}