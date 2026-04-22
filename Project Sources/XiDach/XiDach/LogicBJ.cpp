#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

void doResultBJ(Hand& p, Hand& d, int& result, StatBJ& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: ";
        //p.printHand();
        stat.upPWin();
        if (p.getHandType() == XIDACH) {
            stat.upPBJWin();
            stat.upTotalProfit(1.5);
        }//wins due to BJ won't count for any stat below
        else {
            stat.upTotalProfit(1.0);
            if (p.isDoubled) {
                stat.upTotalProfit(1.0);
                stat.upDoubleWinCount();
                stat.upDoubleVSup(d.getFaceup());
                stat.upDoubleWinVSup(d.getFaceup());
                if (p.wasSoft) stat.upDoubleWinSoftScore(p.first2SoftScore);
                else {
                    stat.upDoubleWinHardScore(p.first2HardScore);
                }
            }
            stat.upDLoseAgainstPScore(p.getScore());
            stat.upDLoseAgainstPFirst2Score(p.first2Score);
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
            if (p.SplitWon) {
                if (p.getHandType() == XIDACH) stat.downPBJWin();
                stat.upSplitWin();
                stat.upPSplitWinRank(rankTable[p.hand[0]]);
                stat.upPSplitWinVSup(d.getFaceup());
            }
        }
    }
    else if (result == -1) {
        stat.upTotalProfit(-1.0);
        //cout << '\n' << "Dealer wins against player's hand: ";
        //p.printHand();
        if (p.isDoubled) {
            stat.upTotalProfit(-1.0);
            stat.upDoubleLoseCount();
            stat.upDoubleVSup(d.getFaceup());
            stat.upDoubleLoseVSup(d.getFaceup());
            if (p.wasSoft) stat.upDoubleLoseSoftScore(p.first2SoftScore);
            else stat.upDoubleLoseHardScore(p.first2HardScore);
        }
        if (p.isBust) {
            stat.upPBust();
        }
        stat.upPLose();
        stat.upDWin();
        stat.upDWinAgainstPScore(p.getScore());
        stat.upDWinAgainstPFirst2Score(p.first2Score);
        stat.upPLoseVSup(d.getFaceup());
        stat.upPLoseAfterHitTime(p.hand.size() - 2);
        if (p.first2HardScore > 0) stat.upPLoseFirst2Hard(p.first2HardScore);
        else stat.upPLoseFirst2Soft(p.first2SoftScore);
        if (d.getHandType() == XIDACH) stat.upDBJWin();
        if (p.getScore() == 16) stat.upPLose16();
        stat.upPTotalHit(p.hand.size() - 2);
        if (p.SplitLost) {
            stat.upSplitLose();
            stat.upPSplitLoseVSup(d.getFaceup());
            stat.upPSplitLoseRank(rankTable[p.hand[0]]);
        }
    }
    else {
        //cout << '\n' << "Draw with player's hand: ";
        //p.printHand();
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

int playerWantsBJ(Hand& p, Hand& d) { //Modify this to change the strategy of players. Note that dealer always stand on 17 or higher
    //Use getFaceUp() function to get the value of Splitable cards in hand
    p.calculateScore();
    if (p.getScore() >= 21) return STAND; //best possible score
    //Add Split conditions here
    if (p.isSplitable) return SPLIT;
    //If hand should no longer Split, consider Doubling now
    if (p.isSoft && p.getScore() >= 11 && p.getScore() <= 16 && p.hand.size() == 2) {
        return DDOUBLE; //2 'D's
    }
    else if (!p.isSoft && p.getScore() >= 10 && p.getScore() <= 13 && p.hand.size() == 2) {
        return DDOUBLE;
    }
    //Note that optimal strategy MIGHT stand when very low score like 12-15
    if (p.getScore() < 16) return HIT;
    if (p.isSoft) return HIT;
    if (!p.isSoft && p.getScore() == 16 && d.getFaceup() == 10) return SURRENDER;
    if (p.getScore() == 16) {
        if (d.getFaceup() == 1 && d.getFaceup() == 2) return HIT;
    }
    return STAND;
}

bool playerInsure(Hand& p, Hand& d) {
    //In stardard strategy, there's no way other than card counting that makes insurance an advised choice
    //The only thing that makes player lose less is by having no 10-value in your hand
    return false; //or true, if you want to see how it crashes EV down hehe
}

void ProcessSimulationBJ(Match& match, Dealer& dealer, StatBJ& stat) {
    //cout << "Dealer's first hand: ";
    //dealer.printprintDealerHand();
    Hand& d = dealer.hands[0];
    d.calculateScore();
    while (d.getScore() < 17) {
        match.dealCardToPlayer(dealer, 0);
    }
    //cout << '\n' << "Dealer's final hand: ";
    //dealer.printDealerHand();
    if (d.getFaceup() == 1) {
        //cout << "Dealer has revealed an Ace" << '\n';
        for (auto& p : match.players) {
            if (playerInsure(p.hands[0], d)) {
                //cout << "Player with hand ";
                //p.printHand();
                //cout << "took insurance." << '\n';
                stat.upInsureCount();
                if (d.getHandType() == XIDACH) stat.upInsureWinCount();
                else stat.upInsureLoseCount();
            }
        }
    }
    for (auto& p : match.players) {
        for (int i = 0; i < p.hands.size(); i++) {
            bool resolved = false;

            while (true) {
                Hand& h = p.hands[i];
                h.calculateScore();

                if (h.getHandType() == XIDACH || d.getHandType() == XIDACH) {
                    int result = compareHands(h, d);
                    if (result == 1 && p.hands.size() > 1) h.setWon();
                    if (result == -1 && p.hands.size() > 1) h.setLost();
                    doResultBJ(h, d, result, stat);
                    resolved = true;
                    break;
                }

                int action = playerWantsBJ(h, d);

                if (h.isBust || action == 0) {
                    break;
                }
                if (action == SPLIT) {
                    match.splitTo(p, i);
                    stat.upTotalSplit(1);
                    
                    continue;
                }
                if (action == HIT) {
                    match.dealCardToHand(h);
                    continue;
                }
                if (action == DDOUBLE) {
                    match.dealCardToHand(h);
                    stat.upDoubleCount();
                    h.setDouble();
                    break;
                }
                if (action == SURRENDER) {
                    resolved = true;
                    stat.upTotalProfit(-0.5);
                    stat.upSurrenderCount();
                }
                break;
            }

            if (!resolved) {
                Hand& h = p.hands[i];
                int result = compareHands(h, d);
                if (result == 1 && p.hands.size() > 1) h.setWon();
                if (result == -1 && p.hands.size() > 1) h.setLost();
                doResultBJ(h, d, result, stat);
            }
        }
    }
    //cout << '\n';
}

void SimulationBJ(ofstream& file) {
    cout << "How many players (including dealer)?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    Match match;
    StatBJ stat;
    stat.setTotalRound((n - 1) * u);
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

        ProcessSimulationBJ(match, match.dealer, stat);
    }
    cout << '\n';
    stat.setEVBJ();
    stat.printStat();
    stat.exportStat(file);
}

bool DealerWantsBJ(Match& match, Dealer& d) {
    d.hands[0].calculateScore();
    if (d.hands[0].getScore() < 17) return true;
    return false;
}

void ProcessPlayBJ(Match& match, Dealer& d) {
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
        if (x == 'y') PlayBJ();
        else return;
    }
    else if (d.hands[0].getHandType() != XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "You have BlackJack and Dealer doesn't. You win!" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') PlayBJ();
        else return;
    }
    else if (d.hands[0].getHandType() == XIDACH && match.players[0].hands[0].getHandType() == XIDACH) {
        cout << "Both you and dealer have BlackJack. This round is a tie" << '\n';
        cout << "Dealer's final hand: ";
        d.hands[0].printHand();
        cout << "Try another round? (y = Yes, n = no)" << '\n';
        char x; cin >> x;
        if (x == 'y') PlayBJ();
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
    while (DealerWantsBJ(match, d)) {
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

void PlayBJ() {
    Match match;
    match.dealer.hands[0].reset();
    match.addPlayer(Player());
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlayBJ(match, match.dealer);
}
