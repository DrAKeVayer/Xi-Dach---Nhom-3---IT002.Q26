#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

int compareHands(Hand& a, Hand& b) {
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

void HitPlay(Match& match, Player& p, Hand& h, Dealer& d) {
    match.dealCardToHand(h);
    h.printNewCard();
    if (h.isBust) {
        PrintPBust(h);
    }
    return;
}
void InsurePlay(Match& match, Player& p, Hand& h, Dealer& d) {
    if (d.hands[0].getHandType() == XIDACH) {
        cout << "Success! Dealer has BlackJack and you break even this round" << '\n';
        h.upProfit(1.0);
    }
    else {
        cout << "Fail! Dealer doesn't have BlackJack. You lost 0.5x bet" << '\n';
        h.upProfit(-0.5);
    }
    return;
}
void SplitPlay(Match& match, Player& p, Hand& h, Dealer& d) {
    cout << "You splited your hand number " << h.Pos << " and drew 2 new cards for each" << '\n';
    match.splitTo(p, (h.Pos - 1));
    PrintState(match, p, d);
    return;
}
void SurrenderPlay(Match& match, Player& p, Hand& h, Dealer& d) {
    cout << "You surrendered your hand and lost 0.5x bet" << '\n';
    h.surrendered = true;
}
void StandPlay(Match& match, Player& p, Hand& h, Dealer& d) {
    cout << "You stood your hand number " << h.Pos << '\n';
    h.stood = true;
}
void DoublePlay(Match& match, Player& p, Hand& h, Dealer& d) {
    cout << "You doubled down your current hand." << '\n';
    h.isDoubled = true;
    match.dealCardToHand(h);
    h.printNewCard();
    if (h.isBust) {
        PrintPBust(h);
    }
}

void PrintResult(Match& match, Hand& h, Dealer& d, int res) {
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

void PrintResultXD(Match& match, Player& p, Dealer& d, int res) {
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

void PrintPBust(Hand& h) {
    cout << "Your hand no." << h.Pos << " is Busted! Bad luck ..." << '\n';
}