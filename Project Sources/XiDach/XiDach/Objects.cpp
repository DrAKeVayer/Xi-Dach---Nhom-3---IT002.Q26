#include "constants.h"
#include "Objects.h"
using namespace std;

Hand::Hand() : score(0) {}

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

void Hand::printNewCard() const {
    cout << getRankName(hand.back());
    cout << getSuit(hand.back());
}

void Hand::printHand() const {
    for (int c : hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
    cout << "(";
    if (isSoft) cout << " Soft ";
    cout << getScore() << ")";
}

/*...................... PLAYER ........................*/

Player::Player() {
    hands.reserve(10);
    hands.push_back(Hand());
}

Player::~Player() {}

void Player::resetPlayer() {
    hands.clear();
    hands.push_back(Hand());
}

void Player::splitHand(int x) {
    hands.push_back(Hand());
    hands.back().hand.push_back(hands[x].hand.back());
    hands[x].hand.pop_back();
}

void Player::printHand() const {
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

/*............. DEALER ..............*/

void Dealer::printDealerFirstHand() const {
    cout << getRankName(hands[0].hand[0]);
    cout << getSuit(hands[0].hand[0]) << ", " << "?";
}

void Dealer::printDealerHand() const {
    for (int c : hands[0].hand) {
        cout << getRankName(c);
        cout << getSuit(c) << ", ";
    }
}

/*...................... MATCH ........................*/

Match::Match() {
    deckInit();
    shuffle();
}

void Match::addPlayer(Player p) {
    players.push_back(p);
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

void Match::splitTo(Player& p, int x) {
    p.splitHand(x);
    dealCardToPlayer(p, x);
    dealCardToPlayer(p, p.hands.size() - 1);
    p.hands[x].calculateScore();
    p.hands.back().calculateScore();
}

void Match::setPlayerCount() { playerCount = players.size(); }

void Match::deductPlayerCount() { playerCount--; }

int Match::getPlayerCount() const { return playerCount; }

void Match::set3() { Has3 = true; }

void Match::set4() { Has4 = true; }

void Match::no3() { Has3 = false;}

void Match::no4() { Has4 = false; }

bool Match::get3() const { return Has3; }

bool Match::get4() const { return Has4; }

/*...................... STAT ........................*/

void Stat::upPWinDBust() { PWinDBust++; }
void Stat::upPLose() { PLose++; }
void Stat::upPLose16() { PLose16++; }
void Stat::upPWin() { PWin++; }
void Stat::upDWin() { DWin++; }
void Stat::upPWin16() { PWin16++; }
void Stat::upDraw() { Draw++; }
void Stat::upPBJWin() { PBJWin++; }
void Stat::upDBJWin() { DBJWin++; }
void Stat::upBJDraw() { BJDraw++; }
void Stat::upPTotalHit(int x) { PTotalHit += x; }
void Stat::upPHitWin() { PHitWin++; }
void Stat::upPStandedWin() { PStandedWin++; }

void Stat::upPWinSoft(int i) { PWinSoft[i]++; }
void Stat::upPWinHard(int i) { PWinHard[i]++; }
void Stat::upPLoseFirst2Soft(int i) { PLoseFirst2Soft[i]++; }
void Stat::upPLoseFirst2Hard(int i) { PLoseFirst2Hard[i]++; }
void Stat::upPWinFirst2Soft(int i) { PWinFirst2Soft[i]++; }
void Stat::upPWinFirst2Hard(int i) { PWinFirst2Hard[i]++; }

void Stat::upPWinAfterHitTime(int i) { PWinAfterHitTime[i]++; }
void Stat::upPLoseAfterHitTime(int i) { PLoseAfterHitTime[i]++; }

void Stat::upDBustCount() { DBustCount++; }
void Stat::upBustDraw() { BustDraw++; }

void Stat::printStat() {
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

void StatBJ::upPWinVSup(int i) { PWinVSup[i]++; }
void StatBJ::upPLoseVSup(int i) { PLoseVSup[i]++; }
void StatBJ::upTotalSplit(int x) { TotalSplit += x; }
void StatBJ::upSplitWin() { SplitWin++; }
void StatBJ::upPSplitWinRank(int i) { PSplitWinRank[i]++; }
void StatBJ::upPSplitWinVSup(int i) { PSplitWinVSup[i]++; }
void StatBJ::upPSplitLoseVSup(int i) { PSplitLoseVSup[i]++; }
void StatBJ::upInsureCount() { InsureCount++; }
void StatBJ::upInsureWinCount() { InsureWinCount++; }
void StatBJ::upInsureLoseCount() { InsureLoseCount++; }
void StatBJ::setEVBJ() {
    double nomi = (PWin - PBJWin) + 1.5 * PBJWin - PLose;
}

void StatBJ::printStat() {
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
    cout << '\n' << "Player takes insurance " << InsureCount << " times total";
    cout << '\n' << "Player Success with insurance " << InsureWinCount << " times total";
    cout << '\n' << "Player Fail with insurance " << InsureLoseCount << " times total";
}

void StatXD::upTotalCheck() { TotalCheck++; }
void StatXD::upDCheckWin() { DCheckWin++; }
void StatXD::upDCheckLose() { DCheckLose++; }
void StatXD::upDCheckWinPreTurn() { DCheckWinPreTurn++; }
void StatXD::upDCheckLosePreTurn() { DCheckLosePreTurn++; }
void StatXD::upDCheckWinPreTurnScore(int i) { DCheckWinPreTurnScore[i]++; }
void StatXD::upDCheckLosePreTurnScore(int i) { DCheckLosePreTurnScore[i]++; }
void StatXD::upDCheckWinPostTurn() { DCheckWinPostTurn++; }
void StatXD::upDCheckLosePostTurn() { DCheckLosePostTurn++; }
void StatXD::upDCheckWinPostTurnScore(int i) { DCheckWinPostTurnScore[i]++; }
void StatXD::upDCheckLosePostTurnScore(int i) { DCheckLosePostTurnScore[i]++; }
void StatXD::upDLastWin() { DLastWin++; }
void StatXD::upDLastLose() { DLastLose++; }
void StatXD::upDCheckWinAgainstSizePreTurn(int i) { DCheckWinAgainstSizePreTurn[i]++; }
void StatXD::upDCheckLoseAgainstSizePreTurn(int i) { DCheckLoseAgainstSizePreTurn[i]++; }
void StatXD::upDCheckWinAgainstSizePostTurn(int i) { DCheckWinAgainstSizePostTurn[i]++; }
void StatXD::upDCheckLoseAgainstSizePostTurn(int i) { DCheckLoseAgainstSizePostTurn[i]++; }

void StatXD::upXBCount() { XBCount++; }
void StatXD::upPNLCount() { PNLCount++; }
void StatXD::upDNLCount() { DNLCount++; }
void StatXD::upXBDrawCount() { XBDrawCount++; }
void StatXD::upPNLWinCount() { PNLWinCount++; }
void StatXD::upDNLWinCount() { DNLWinCount++; }
void StatXD::upNLDrawCount() { NLDrawCount++; }

void StatXD::printStat() {
    Stat::printStat();
    cout << '\n' << "XI DACH SPECIFIC STATS: " << '\n';
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

