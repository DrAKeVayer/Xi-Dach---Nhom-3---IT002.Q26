#include "Engine.h"
#include "Entities.h"
#include "MatchXD.h"

void MatchXD::setPlayerCount() { playerCount = players.size(); }

void MatchXD::deductPlayerCount() { playerCount--; }

int MatchXD::getPlayerCount() const { return playerCount; }

void MatchXD::set3() { Has3 = true; }

void MatchXD::set4() { Has4 = true; }

void MatchXD::no3() { Has3 = false; }

void MatchXD::no4() { Has4 = false; }

bool MatchXD::get3() const { return Has3; }

bool MatchXD::get4() const { return Has4; }

void MatchXD::doResultXD(Hand& p, Hand& d, int& result, StatXD& stat) {
    if (result == 1) {
        //cout << '\n' << "Player wins with hand: " << '\n';
        //p.printHand();
        stat.upPWin();
        if (p.getHandType() == NGULINH) stat.upPNLWinCount();
        else if (p.getHandType() == XIDACH) stat.upPBJWin(); //wins due to BJ won't count for any stat below
        else if (p.getHandType() == XIBANG) stat.upXBCount();
        else {
            stat.upDLoseAgainstPScore(p.getScore());
            stat.upDLoseAgainstPFirst2Score(p.first2Score);
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
        stat.upDWinAgainstPScore(p.getScore());
        stat.upDWinAgainstPFirst2Score(p.first2Score);
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
        else if (d.getHandType() == XIBANG) {
            stat.upXBCount();
            stat.upXBCount();
            stat.upXBDrawCount();
        }
        else if (d.getHandType() == NGULINH) {
            stat.upDNLCount();
        }
        stat.upPTotalHit(p.hand.size() - 2);
    }
}

int MatchXD::playerWantsXD(Hand& p, Hand& d) { //Modify this to change the strategy of players.
    /*0: stand
      1: hit
    */
    p.calculateScore();
    if (p.getScore() >= 16) return 0;
    if (p.getScore() <= 15) return 1;
    // (p.isSoft && p.getScore() < 18) return 1;
    return 0;
}

bool MatchXD::dealerWantsToCheck(Hand& p, Hand& d) {
    int ds = d.getScore();

    if (ds >= 20) return XET;

    if (ds >= 18)
        return ((p.hand.size() == 3 || p.hand.size() == 4) ? XET : 0);

    if (ds >= 15)
        return (p.hand.size() == 4 ? XET : 0);

    return 0;
}

int MatchXD::dealerWantsToAction(Hand& d) {
    int ds = d.getScore();

    if (d.hand.size() == 5) return XETALL;

    if (ds >= 18)
        return ((get3() || get4()) ? XETALL : XETALL);

    if (ds >= 15)
        return (get4() ? XET : HIT);

    return HIT;
}

void MatchXD::ProcessSimulationXD(Dealer& dealer, StatXD& stat) {
    Hand& d = dealer.hands[0];
    d.calculateScore();
    //cout << "Dealer's hand: ";
    //d.printHand();
    //cout << '\n';
    if (d.getHandType() == XIDACH || d.getHandType() == XIBANG) {
        int t = d.getHandType();
        for (auto& p : players) {
            //cout << "Player's hand: ";
            //p.printHand();
            //cout << '\n';
            p.hands[0].calculateScore();
            int result = compareHands(p.hands[0], d);
            doResultXD(p.hands[0], d, result, stat); //the match ends now
        }
        return;
    }
    for (auto& p : players) {
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
                dealCardToHand(p.hands[0]);
                if (p.hands[0].isBust) stat.upPBust();
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
            deductPlayerCount();
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
    for (auto& p : players) {
        if (p.hands[0].hand.size() == 3 && !p.hands[0].resolved) set3();
        if (p.hands[0].hand.size() == 4 && !p.hands[0].resolved) set4();
    }
    //cout << "Dealer's turn to draw starts now" << '\n';
    while (true) {
        if (getPlayerCount() == 0) break;
        int action = dealerWantsToAction(d);
        if (action == 1) {
            dealCardToHand(d);
            //cout << "Just hit for: ";
            //d.printNewCard();
            //cout << '\n' << "Dealer's hand is now: ";
            //d.printHand();
            //cout << '\n';
            d.calculateScore();
            if (d.isBust) {
                stat.upDBustCount();
                //cout << "Dealer Busted!" << '\n';
                for (auto& p : players) {
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
            if (getPlayerCount() != 0) {
                for (auto& p : players) {
                    if (p.hands[0].hand.size() >= (action + 1) && !p.hands[0].resolved) {
                        //cout << "Dealer checks this player hand: ";
                        //p.printHand();
                        //cout << '\n';
                        stat.upTotalCheck();
                        deductPlayerCount();
                        if (p.hands[0].hand.size() == 4) no4();
                        if (p.hands[0].hand.size() == 3) no3();
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
        for (auto& p : players) {
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

void MatchXD::SimulationXD(ofstream& file) {
    cout << "How many players (including dealer)?" << '\n';
    int n; cin >> n;
    cout << "How many Matches?" << '\n';
    int u; cin >> u;

    StatXD stat;
    dealer.hands[0].reset();
    for (int i = 0; i < n - 1; i++) {
        addPlayer(i);
    }
    setPlayerCount();
    for (int z = 0; z < u; z++) {
        deckReset();
        shuffle();
        no3();
        no4();
        dealer.hands[0].reset();
        for (auto& p : players) {
            p.resetPlayer();
        }

        deal2Card();

        ProcessSimulationXD(dealer, stat);
    }
    cout << '\n';
    stat.setEVXD();
    stat.printStat();
    stat.exportStat(file);
}

void MatchXD::ProcessPlayPlayer(Dealer& dealer) {
    Hand& d = dealer.hands[0];
    if (d.getHandType() == XIBANG || d.getHandType() == XIDACH) {
        cout << "Dealer has ";
        if (d.getHandType() == XIBANG) cout << "XIBANG!";
        else cout << "XIDACH!";
        cout << " The round ends now with results:" << '\n';
        for (auto& p : players) {
            int res = compareHands(p.hands[0], d);
            PrintResultXD(p, dealer, res);
        }
        return;
    }
    for (auto& p : players) {
        cout << "Player no." << p.Pos << " turn:" << '\n';
        if (p.hands[0].getHandType() == XIDACH || p.hands[0].getHandType() == XIBANG) {
            cout << "You have ";
            if (d.getHandType() == XIBANG) cout << "XIBANG!";
            else cout << "XIDACH!";
            cout << '\n';
            continue;
        }
        while (!p.hands[0].isBust && !p.hands[0].stood && p.hands[0].hand.size() < 5) {
            p.hands[0].printHand();
            if (p.hands[0].getScore() < 16) {
                cout << "You must Hit! Press any key to Hit" << '\n' << "--> ";
                char x; cin >> x;
                HitPlay(p.hands[0]);
                continue;
            }
            cout << "Enter your choice: (H)it, (S)tand" << '\n' << "--> ";
            char x; cin >> x;
            if (x == 'H') {
                HitPlay(p.hands[0]);
            }
            else {
                cout << "You stood!" << '\n';
                p.hands[0].stood = true;
                if (dealerWantsToCheck(p.hands[0], d)) {
                    p.hands[0].toBeChecked = true;
                }
            }
        }
        if (p.hands[0].hand.size() == 5) {
            if (p.hands[0].isBust) PrintPBust(p.hands[0]);
            else {
                cout << "You got NGULINH!";
            }
        }
    }
    for (auto& p : players) {
        if (p.hands[0].toBeChecked) {
            cout << "Dealer wants to check player no." << p.Pos << " before their turn!" << '\n';
            int res = compareHands(p.hands[0], d);
            PrintResultXD(p, dealer, res);
            p.hands[0].setSolved();
            deductPlayerCount();
        }
    }
    while (getPlayerCount() != 0 || d.hand.size() < 5) {
        dealer.printDealerHand();
        if (d.isBust) {
            cout << "Dealer Busted!" << '\n';
            for (auto& p : players) {
                if (!p.hands[0].resolved) {
                    p.hands[0].setSolved();
                    int res = compareHands(p.hands[0], d);
                    PrintResultXD(p, dealer, res);
                }
            }
            return;
        }
        else if (d.getHandType() == NGULINH) {
            cout << "Dealer has NGULINH!" << '\n';
            for (auto& p : players) {
                if (!p.hands[0].resolved) {
                    p.hands[0].setSolved();
                    int res = compareHands(p.hands[0], d);
                    PrintResultXD(p, dealer, res);
                }
            }
        }
        int action = dealerWantsToAction(d);
        if (action == HIT) {
            dealCardToHand(d);
            d.printNewCard();
            cout << '\n';
            continue;
        }
        else if (action == 2 || action == 3) {
            if (getPlayerCount() != 0) {
                for (auto& p : players) {
                    if (p.hands[0].hand.size() >= (action + 1) && !p.hands[0].resolved) {
                        cout << "Dealer checks player no." << p.Pos << " hand: ";
                        p.printHand();
                        cout << '\n';
                        deductPlayerCount();
                        if (p.hands[0].hand.size() == 4) no4();
                        if (p.hands[0].hand.size() == 3) no3();
                        p.hands[0].setSolved();
                        int res = compareHands(p.hands[0], d);
                        PrintResultXD(p, dealer, res);
                    }
                }
            }
            continue;
        }
        else {
            cout << "Dealer stood" << '\n';
            dealer.stood = true;
            dealer.printDealerHand();
            for (auto& p : players) {
                if (!p.hands[0].resolved) {
                    cout << '\n' << "Dealer checks player no." << p.Pos << " hand: ";
                    p.hands[0].printJustHand();
                    p.hands[0].setSolved();
                    int res = compareHands(p.hands[0], d);
                    PrintResultXD(p, dealer, res);
                }
            }
            break;
        }
    }
}

void MatchXD::PlayDealerCheckAll(Dealer& dealer) {
    Hand& d = dealer.hands[0];
    for (auto& p : players) {
        if (!p.hands[0].resolved) {
            cout << '\n' << "You check player no." << p.Pos << " hand: ";
            p.hands[0].printJustHand();
            p.hands[0].setSolved();
            deductPlayerCount();
            int res = compareHands(p.hands[0], d);
            PrintResultXD(p, dealer, res);
        }
    }
}

void MatchXD::PrintState() {
    cout << "All players' hands composition:" << '\n';
    for (auto& p : players) {
        if (!p.hands[0].resolved)
            cout << "Player no." << p.Pos << " hand size: " << p.hands[0].hand.size() << '\n';
    }
}

void MatchXD::DealerOptions(Dealer& dealer) {
    Hand& d = dealer.hands[0];
    if (d.getScore() >= 15) {
        string x;
        cout << "Enter your choice: Number: Check that player, (H)it, (S)tand" << '\n' << "--> ";
        cin >> x;
        if (x == "H") {
            HitPlay(d);
        }
        else if (x == "S") {
            cout << "You stood! Now checking all." << '\n';
            PlayDealerCheckAll(dealer);
            dealer.stood = true;
        }
        else {
            for (auto& p : players) {
                if (!p.hands[0].resolved && p.Pos == stoi(x)) {
                    cout << '\n' << "You checks player no." << p.Pos << " hand: ";
                    p.hands[0].printJustHand();
                    p.hands[0].setSolved();
                    deductPlayerCount();
                    int res = compareHands(p.hands[0], d);
                    PrintResultXD(p, dealer, res);
                }
            }
        }
    }
    else {
        cout << "You must Hit! Press any key to Hit." << '\n' << "--> ";
        char x; cin >> x;
        HitPlay(d);
    }
}

void MatchXD::ProcessPlayDealer(Dealer& dealer) {
    Hand& d = dealer.hands[0];
    if (d.getHandType() == XIBANG || d.getHandType() == XIDACH) {
        cout << "Dealer has ";
        if (d.getHandType() == XIBANG) cout << "XIBANG!";
        else cout << "XIDACH!";
        cout << " The round ends now with results:" << '\n';
        for (auto& p : players) {
            int res = compareHands(p.hands[0], d);
            PrintResultXD(p, dealer, res);
        }
        cout << "The round ends now!";
        return;
    }

    for (auto& p : players) {
        while (true) {
            if (p.hands[0].getHandType() == XIDACH || p.hands[0].getHandType() == XIBANG) break;
            int action = playerWantsXD(p.hands[0], d);
            if (action == 1) {
                dealCardToHand(p.hands[0]);
                continue;
            }
            if (action == 0) {
                break;
            }
        }
    }
    cout << "All players have completed their turn" << '\n';
    while (!d.stood && getPlayerCount() != 0) {
        if (d.isBust) {
            cout << "Now checking all:" << '\n';
            PlayDealerCheckAll(dealer);
            cout << "The round ends now!";
            return;
        }
        PrintState();
        cout << "Your hand is: ";
        d.printJustHand();
        DealerOptions(dealer);
    }

}

void MatchXD::PlayPlayer() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    Match match;
    match.dealer.hands[0].reset();
    no3();
    no4();
    for (int i = 0; i < n; i++) {
        match.addPlayer(i);
    }
    setPlayerCount();
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlayPlayer(dealer);
}

void MatchXD::PlayDealer() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    Match match;
    match.dealer.hands[0].reset();
    no3();
    no4();
    for (int i = 0; i < n; i++) {
        match.addPlayer(i);
    }
    setPlayerCount();
    match.deckReset();
    match.shuffle();
    match.deal2Card();

    ProcessPlayDealer(dealer);
}

void MatchXD::PlayXD() {
    cout << "How many players?" << '\n';
    int n; cin >> n;
    dealer.hands[0].reset();
    for (int i = 0; i < n; i++) {
        addPlayer(i);
    }
    deckReset();
    shuffle();
    deal2Card();

    ProcessPlayPlayer(dealer);
}