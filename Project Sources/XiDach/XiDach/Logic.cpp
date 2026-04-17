#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

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