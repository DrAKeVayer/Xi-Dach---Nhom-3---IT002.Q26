        while (true) {
            if (p.hands[0].hand.size() == 5) {
                if (p.hands[0].getHandType() == NGULINH) stat.upPNLCount();
                break;
            }
            int action = playerWantsXD(p.hands[0], d);
            if (action == 1) {
                match.dealCardToHand(p.hands[0]);
                continue;
            }
            if (action == 0) {
                break;
            }
        }

        if (d.getHandType() == XIDACH || d.getHandType() == XIBANG) {
    for (auto& p : match.players) {
        int result = compareHands(p.hands[0], d);
        doResultXD(p.hands[0], d, result, stat);
    }
    return;
}