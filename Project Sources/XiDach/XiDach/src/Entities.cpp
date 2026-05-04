#include "Constants.h"
#include "Entities.h"
using namespace std;

Hand::Hand(Player* _p, int _C) : score(0), owner(_p), Pos(_C + 1) {}

void Hand::reset() {
    hand.clear();
    isBust = false;
    hasAce = false;
    score = 0;
}

void Hand::calculateScore() {
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
        if (hand.size() == 2) {
            first2SoftScore = score;
            first2Score = score;
            wasSoft = true;
        }
    }

    if (!isSoft && hand.size() == 2) {
        first2HardScore = score;
        first2Score = score;
    }

    if (hand.size() == 2) {
        if (getRankName(hand[0]) == getRankName(hand[1])) isSplitable = true;
    }

    if (score > 21) {
        isBust = true;
    }
}

void Hand::receiveCard(int x) {
    hand.push_back(x);
    calculateScore();
}

void Hand::setSolved() {
    resolved = true;
}

int Hand::getFaceup() {
    int r = rankTable[hand[0]];
    return r;
}

void Hand::setWon() {
    SplitWon = true;
}

void Hand::setLost() {
    SplitLost = true;
}

void Hand::setDouble() {
    isDoubled = true;
}

HandType Hand::getHandType() {
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

int Hand::getScore() const {
    return score;
}

int Hand::getSoftScore() const {
    return softScore;
}

void Hand::upProfit(double x) {
    profit += x;
}

void Hand::printNewCard() {
    owner->printOwner();
    cout << getRankName(hand.back());
    cout << getSuit(hand.back());
    owner->printForWho(*this); 
}

void Hand::printHand() const {
    cout << "Your hand no." << Pos << ": [";
    for (int c : hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
    cout << "(";
    if (isSoft) cout << " Soft ";
    cout << getScore() << ")]" << '\n';
}

void Hand::printJustHand() const {
    cout << "[";
    for (int c : hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
    cout << "(";
    if (isSoft) cout << " Soft ";
    cout << getScore() << ")]" << '\n';
}

/*...................... PLAYER ........................*/
Player::Player(int _p) : Pos(_p + 1) {
    hands.reserve(10);
    hands.push_back(Hand(this, 0));
}

Player::~Player() {}

void Player::resetPlayer() {
    hands.clear();
    hands.push_back(Hand(this, 0));
}

void Player::splitHand(int x) {
    splitCount++;
    hands.push_back(Hand(this, splitCount));
    hands.back().hand.push_back(hands[x].hand.back());
    hands[x].hand.pop_back();
}

void Player::upProfit(double x) {
    profit += x;
}

void Player::printOwner() {
    cout << "You just drew ";
}

void Player::printForWho(Hand& h) {
    cout << " for your hand no." << h.Pos << '\n';
}

void Player::printHand() {
    for (int i = 0; i < hands.size(); i++) {
        cout << "Your hand number " << i + 1 << ": [";
        for (int c : hands[i].hand) {
            cout << getRankName(c);
            cout << getSuit(c) << ", ";
        }
        cout << "(";
        if (hands[i].isSoft) cout << " Soft ";
        cout << hands[i].getScore() << ")]" << '\n';
        upProfit(hands[i].profit);
    }
}

void Player::printProfit() {
    for (auto& h : hands) {
        upProfit(h.profit);
    }
    cout << "Current Profit: " << profit << " x bet";
    cout << '\n';
}

/*............. DEALER ..............*/

void Dealer::printDealerFirstHand() const {
    cout << "Dealer's hand is: [";
    cout << getRankName(hands[0].hand[0]);
    cout << getSuit(hands[0].hand[0]) << ", " << "?]" << '\n';
}

void Dealer::printOwner() {
    cout << "Dealer just drew ";
}

void Dealer::printForWho(Hand& h) {
    cout << '\n';
}

void Dealer::printDealerHand() const {
    if (!stood) cout << "Dealer's hand is: [";
    else cout << "Dealer's final hand is: [";
    
    for (int c : hands[0].hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
    cout << " (";
    if (hands[0].isSoft) cout << " Soft ";
    cout << hands[0].getScore() << ")]" << '\n';
}