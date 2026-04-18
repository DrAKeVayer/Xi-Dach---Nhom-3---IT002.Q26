#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

void doResultXD(Hand& p, Hand& d, int& result, StatXD& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: " << '\n';
        //p.printHand();
        stat.upPWin();
        if (p.getHandType() == NGULINH) stat.upPNLWinCount();
        if (p.getHandType() == XIDACH) stat.upPBJWin(); //wins due to BJ won't count for any stat below
        else if (p.getHandType() == XIBANG) stat.upXBCount();
        else {
            if (p.getScore() == 16) stat.upPWin16();
            if (p.first2HardScore > 0) stat.upPWinFirst2Hard(p.first2HardScore);
            else stat.upPWinFirst2Soft(p.first2SoftScore);
            if (d.isBust) stat.upPWinDBust();
            if (p.isSoft) stat.upPWinSoft(p.getScore());
            else if (p.getHandType() != NGULINH) {
                stat.upPWinHard(p.getScore());
            }
            stat.upPTotalHit(p.hand.size() - 2);
            if (p.hand.size() - 2) stat.upPHitWin();
            else stat.upPStandedWin();
            stat.upPWinAfterHitTime(p.hand.size() - 2);
        }
    }
    else if (result == -1) {
        //cout << '\n' << "Dealer wins against player's hand: "<< '\n';
        //p.printHand();
        stat.upDWin();
        if (d.getHandType() == NGULINH) stat.upDNLWinCount();
        stat.upPLoseAfterHitTime(p.hand.size() - 2);
        if (d.getHandType() == XIBANG) stat.upXBCount();
        if (p.first2HardScore > 0) stat.upPLoseFirst2Hard(p.first2HardScore);
        else if (p.getHandType() != XIBANG) {
            stat.upPLoseFirst2Soft(p.first2SoftScore);
        }
        if (d.getHandType() == XIDACH) stat.upDBJWin();
        if (p.getScore() == 16) stat.upPLose16();
        stat.upPTotalHit(p.hand.size() - 2);

    }
    else {
        //cout << '\n' << "Draw with player's hand: "<< '\n';
        //p.printHand();
        stat.upDraw();
        if (d.getHandType() == XIDACH) stat.upBJDraw();
        if (d.getHandType() == XIBANG) {
            stat.upXBCount();
            stat.upXBCount();
            stat.upXBDrawCount();
        }
        if (d.getHandType() == NGULINH) {
            stat.upDNLCount();
        }
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

int playerWantsXD(Hand& p, Hand& d) { //Modify this to change the strategy of players.
    /*0: stand
      1: hit
    */
    p.calculateScore();
    if (p.getScore() >= 21) return 0; //best possible score
    if (p.getScore() <= 16) return 1; //always hit on 16, stand otherwise
    if (p.isSoft && p.getScore() < 19) return 1; //always hit if soft hand < 19
    return 0;
}

bool dealerWantsToCheck(Hand& p, Hand& d) {
    if (d.getScore() < 15) return STAND;
    if (d.getScore() > 18) return XET; //dealer 19+ will always check hand;
    if (p.hand.size() > 2 && d.getScore() > 16) return XET; //dealer 17+ will check player with 3+ cards hand
    if (p.hand.size() > 3 && d.getScore() > 14) return XET;//dealer 15+ will check player with 4 cards hand
    return 0;
}

int dealerWantsToAction(Match& match, Hand& d) {
    int s = d.getScore();
    if (d.hand.size() == 5) return XETALL;
    if (s < 15) return HIT; //obvious
    /* We have 2 options for strategies here:
    1: If there are players holding 4 cards and dealer has enough just score to check, check them
       Then for players holding 3 cards, dealer should have a threshold (e.g. 17) to check
       Then for players who didn't hit, the threshold should be higher (e.g. 19+) to check
       Uncomment to test/change this strategy
       */
       //if (match.get4() != 0) return XET4; //if checking all 4-card player, get4() will return 0, don't worry
       //else if (match.get3() != 0 && s > 16) return XET3;
       //else if (s > 18) return XET3;
       //else if (!d.isBust) return HIT;

       // 2: "Dynamicly according to soft/hard hands as below"
    if (d.isSoft && (s == 18 || s == 19)) return (match.get3() || match.get4() ? XET3 : HIT); //soft 18 19: check 3+ card hand, else hit ()
    if (d.isSoft && (s == 20 || s == 21)) return XETALL; //soft 20 21: check all
    if (d.isSoft && (s == 16 || s == 17)) return (match.get4() ? XET4 : HIT); //soft 16 17: check 4 card hand
    if (!d.isSoft && (s >= 16 && s <= 18)) return (match.get4()) ? XET4 : HIT; //hard 16-18: check 4 card hand
    if (!d.isSoft && (s >= 19 && s <= 21)) return XETALL; //hard 19-21: check all
    return XETALL;
}

void ProcessSimulationXD(Match& match, Dealer& dealer, StatXD& stat) {
    Hand& d = dealer.hands[0];
    d.calculateScore();
    //cout << "Dealer's hand: ";
    //d.printHand();
    //cout << '\n';
    if (d.getHandType() == XIDACH || d.getHandType() == XIBANG) {
        int t = d.getHandType();
        for (auto& p : match.players) {
            //cout << "Player's hand: ";
            //p.printHand();
            //cout << '\n';
            p.hands[0].calculateScore();
            int result = compareHands(p.hands[0], d);
            doResultXD(p.hands[0], d, result, stat); //the match ends now
        }
        return;
    }
    for (auto& p : match.players) {
        while (true) {
            //cout << "This player's hand: ";
            //p.hands[0].printHand();
            //cout << '\n';
            if (p.hands[0].hand.size() == 5) {
                if (p.hands[0].getHandType() == NGULINH) stat.upPNLCount();
                break;
            }
            if (p.hands[0].getHandType() == XIDACH || p.hands[0].getHandType() == XIBANG) break;
            int action = playerWantsXD(p.hands[0], d);
            if (action == 1) {
                match.dealCardToHand(p.hands[0]);
                //cout << "Just hit for: ";
                //p.hands[0].printNewCard();
                //cout << '\n';
                continue;
            }
            if (action == 0) {
                //cout << "Standed!" << '\n';
                break;
            }
        }
        if (dealerWantsToCheck(p.hands[0], d)) {
            stat.upTotalCheck();
            //cout << "Dealer checks this hand before Dealer's turn to draw" << '\n';
            match.deductPlayerCount();
            p.hands[0].setSolved();
            int result = compareHands(p.hands[0], d);
            if (result == -1) {
                stat.upDCheckWin();
                stat.upDCheckWinPreTurn();
                stat.upDCheckWinPreTurnScore(d.getScore());
                stat.upDCheckWinAgainstSizePreTurn(p.hands[0].hand.size());
            }
            else if (result == 1) {
                stat.upDCheckLose();
                stat.upDCheckLosePreTurn();
                stat.upDCheckLosePreTurnScore(d.getScore());
                stat.upDCheckLoseAgainstSizePreTurn(p.hands[0].hand.size());
            }
            doResultXD(p.hands[0], d, result, stat);
        }
        continue;
    }
    for (auto& p : match.players) {
        if (p.hands[0].hand.size() == 3 && !p.hands[0].resolved) match.set3();
        if (p.hands[0].hand.size() == 4 && !p.hands[0].resolved) match.set4();
    }
    //cout << "Dealer's turn to draw starts now" << '\n';
    while (true) {
        if (match.getPlayerCount() == 0) break;
        int action = dealerWantsToAction(match, d);
        if (action == 1) {
            match.dealCardToHand(d);
            //cout << "Just hit for: ";
            //d.printNewCard();
            //cout << '\n' << "Dealer's hand is now: ";
            //d.printHand();
            //cout << '\n';
            d.calculateScore();
            if (d.isBust) {
                stat.upDBustCount();
                //cout << "Dealer Busted!" << '\n';
                for (auto& p : match.players) {
                    if (!p.hands[0].resolved) {
                        int result = compareHands(p.hands[0], d);
                        if (p.hands[0].isBust) stat.upBustDraw();
                        doResultXD(p.hands[0], d, result, stat);
                    }
                }
                return;
            }
            continue;
        }
        else if (action == 2 || action == 3) {
            if (match.getPlayerCount() != 0) {
                for (auto& p : match.players) {
                    if (p.hands[0].hand.size() >= (action + 1) && !p.hands[0].resolved) {
                        //cout << "Dealer checks this player hand: ";
                        //p.printHand();
                        //cout << '\n';
                        stat.upTotalCheck();
                        match.deductPlayerCount();
                        if (p.hands[0].hand.size() == 4) match.no4();
                        if (p.hands[0].hand.size() == 3) match.no3();
                        p.hands[0].setSolved();
                        int result = compareHands(p.hands[0], d);
                        if (result == -1) {
                            stat.upDCheckWin();
                            stat.upDCheckWinPostTurn();
                            stat.upDCheckWinPostTurnScore(d.getScore());
                            stat.upDCheckWinAgainstSizePostTurn(p.hands[0].hand.size());
                        }
                        else if (result == 1) {
                            stat.upDCheckLose();
                            stat.upDCheckLosePostTurn();
                            stat.upDCheckLosePostTurnScore(d.getScore());
                            stat.upDCheckLoseAgainstSizePostTurn(p.hands[0].hand.size());
                        }
                        doResultXD(p.hands[0], d, result, stat);
                    }
                }
            }
            continue;
        }
        //cout << "Dealer stand" << '\n';
        //cout << "Dealer's final hand: ";
        //d.printHand();
        if (d.getHandType() == NGULINH) stat.upDNLCount();
        for (auto& p : match.players) {
            if (!p.hands[0].resolved) {
                //cout << '\n' << "Dealer checks this player hand: ";
                //p.printHand();
                p.hands[0].setSolved();
                int result = compareHands(p.hands[0], d);
                if (result == 1) {
                    stat.upDLastLose();
                    if (p.hands[0].getHandType() == XIBANG) stat.upXBCount();
                }
                else if (result == -1) {
                    stat.upDLastWin();
                }
                else if (result == 0) {
                    if (d.getHandType() == NGULINH) stat.upNLDrawCount();
                }
                doResultXD(p.hands[0], d, result, stat);
            }
        }
        break;
    }
    //cout << '\n';
}

void SimulationXD(ofstream& file) {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    Match match;
    StatXD stat;
    match.dealer.hands[0].reset();
    for (int i = 0; i < n - 1; i++) {
        match.addPlayer(Player());
    }
    match.setPlayerCount();
    for (int z = 0; z < u; z++) {
        match.deckReset();
        match.shuffle();
        match.no3();
        match.no4();
        match.dealer.hands[0].reset();
        for (auto& p : match.players) {
            p.resetPlayer();
        }

        match.deal2Card();

        ProcessSimulationXD(match, match.dealer, stat);
    }
    cout << '\n';
    stat.setEVXD();
    stat.printStat();
    stat.exportStat(file);
}
