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
    int dUp = d.getFaceup();
    p.calculateScore();
    if (p.getScore() >= 21) return STAND; //best possible score
    //Add Split conditions here
    if (p.getFaceup() == 1 && p.splited) return STAND; //Must stand if splited Ace
    if (p.isSplitable) {
        int up = p.getFaceup();
        if (up == 1 || up == 8) return SPLIT;
        else if (up == 10) return STAND;
        else if (up == 5) return DDOUBLE;
        else if (up == 4 && dUp >= 3 && dUp <= 6) return SPLIT;
        else if ((up == 2 || up == 3 || up == 6 || up == 7) && dUp >= 2 && dUp <= 7) return SPLIT;
        else if (up == 9 && dUp >= 2 && dUp <= 8) return SPLIT;
    }
    //If hand should no longer Split, consider Doubling now
    if (p.isSoft && p.getScore() >= 13 && p.getScore() <= 18 && p.hand.size() == 2) {
        if (dUp >= 3 && dUp <= 6) return DDOUBLE;
    }
    else if (!p.isSoft && p.getScore() >= 9 && p.getScore() <= 11 && p.hand.size() == 2) {
        if (p.getScore() == 11) return DDOUBLE;
        else if (p.getScore() == 10 && (dUp != 1 && dUp != 10)) return DDOUBLE;
        else if (p.getScore() == 9 && dUp >= 3 && dUp <= 6) return DDOUBLE;
    }
    //Note that optimal strategy MIGHT stand when very low score like 12-15
    if (p.isSoft) {
        if (p.getScore() < 18) return HIT;
        if (p.getScore() == 18 && (dUp == 1 || dUp >= 8)) return HIT;
        else return STAND;
    }
    else {
        if (p.getScore() < 12) return HIT;
        if (p.getScore() > 16) return STAND;
        if ((p.getScore() == 15 || p.getScore() == 16) && (p.hand.size() == 2) && !p.splited && (dUp == 1 || dUp >= 8)) return SURRENDER;
        if (p.getScore() >= 12 && p.getScore() <= 16 && dUp >= 3 && dUp <= 6) return STAND;
    }
    return HIT;
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
        if (d.isBust) stat.upDBustCount();
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
        match.addPlayer(i);
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

void AnotherRound() {
    cout << "Try another round? (y = Yes, n = no)" << '\n';
    char x; cin >> x;
    if (x == 'y') PlayBJ();
    else return;
}

int GetSit(Match& match, Player& p, Hand& h, Dealer& d) {
    if (p.hands.size() == 1 && h.hand.size() == 2) {
        return FIRSTDOUBLE;
    }
    else if (p.hands.size() > 1 && h.hand.size() == 2) {
        return SECONDDOUBLE;
    }
    else if (h.isSplitable && p.hands.size() == 1) {
        return ALL;
    }
    else if (h.isSplitable && p.hands.size() > 1) {
        return SECONDSPLIT;
    }
    return HITSTAND;
}

void Options(Match& match, Player& p, Hand& h, Dealer& d, int x) {
    char i;
    h.printHand();
    if (x == 0) {
        cout << "Buy (I)nsurance?";
        cin >> i;
        if (x == 'I') {
            if (d.hands[0].getHandType() == XIDACH) {
                cout << "Success! Dealer has BlackJack and you break even this round" << '\n';
            }
            else {
                cout << "Fail! Dealer doesn't have BlackJack. You lost 0.5x bet" << '\n';
                h.upProfit(-0.5);
            }
        }
        return;
    }
    else if (x == ALL) cout << "Enter your choice: S(u)rrender, S(p)lit, (D)ouble, (H)it, (S)tand";
    else if (x == SECONDSPLIT) cout << "Enter your choice: S(p)lit, (D)ouble, (H)it, (S)tand";
    else if (x == SECONDDOUBLE) cout << "Enter your choice: (D)ouble, (H)it, (S)tand";
    else if (x == FIRSTDOUBLE) cout << "Enter your choice: S(u)rrender, (D)ouble, (H)it, (S)tand";
    else if (x == HITSTAND) cout << "Enter your choice: (H)it, (S)tand";
    AISuggest(p, h, d);
    cout << "--> ";
    cin >> i;
    if (i == 'U') SurrenderPlay(match, p, h, d);
    else if (i == 'P') SplitPlay(match, p, h, d);
    else if (i == 'D') DoublePlay(match, p, h, d);
    else if (i == 'H') HitPlay(match, p, h, d);
    else if (i == 'S') StandPlay(match, p, h, d);
}

void PrintState(Match& match, Player& p, Dealer& d) {
    cout << "Current state: " << '\n';
    d.printDealerFirstHand();
}

void AISuggest(Player& p, Hand& h, Dealer& d) {
    int res = playerWantsBJ(h, d.hands[0]);
    cout << " (AI suggested choice: ";
    if (res == HIT) cout << "Hit)" << '\n';
    if (res == DDOUBLE) cout << "Double)" << '\n';
    if (res == STAND) cout << "Stand)" << '\n';
    if (res == SPLIT) cout << "Split)" << '\n';
    if (res == SURRENDER) cout << "Surrender)" << '\n';
}

void ProcessPlayBJ(Match& match, Dealer& d) {
    Stat stat; //
    d.printDealerFirstHand();
    match.players[0].printHand();
    
    if (d.hands[0].getFaceup() == 1) {
        for (auto& p : match.players) {
            Options(match, p, p.hands[0], d, INSURE);
        }
    }

    if (d.hands[0].getHandType() == XIDACH) {
        cout << "Dealer has BlackJack! All non-BlackJack players lose";
        for (auto& p : match.players) {
            int res = compareHands(p.hands[0], d.hands[0]);
            PrintResult(match, p.hands[0], d, res);
        }
        AnotherRound();
        return;
    }

    for (auto& p : match.players) {
        cout << "It's player no." << p.Pos << " turn" << '\n';
        for (auto& h : p.hands) {
            while (!h.surrendered && !h.stood && !h.isDoubled && !h.isBust) {
                int sit = GetSit(match, p, h, d);
                Options(match, p, h, d, sit);
            }
        }
    }

    while (DealerWantsBJ(match, d)) {
        d.printDealerHand();
        match.dealCardToHand(d.hands[0]);
        d.hands[0].printNewCard();
    }
    d.stood = true;
    d.printDealerHand();

    int o = 1;
    for (auto& p : match.players) {
        cout << '\n' << "Player " << o++ << " results:";
        for (auto& h : p.hands) {
            int res = compareHands(h, d.hands[0]);
            PrintResult(match, h, d, res);
        }
        p.printProfit();
    }
    AnotherRound();
    return;
}

void PlayBJ() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    Match match;
    match.dealer.hands[0].reset();
    for (int i = 0; i < n; i++) {
        match.addPlayer(i);
    }
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlayBJ(match, match.dealer);
}
