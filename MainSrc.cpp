#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

// Handle one player's turn (Hit or Stand)
void playerTurn(Player* p, Match& match) {
    while (true) {
        cout << "\nYour hand: ";
        printHand(*p);
        cout << " | Score: " << p->getScore() << '\n';

        // If player already bust -> stop
        if (p->isBust) {
            cout << "You busted!\n";
            break;
        }

        cout << "Choose action: (h = Hit, s = Stand): ";
        char choice;
        cin >> choice;

        if (choice == 'h') {
            // Draw 1 more card
            int card = match.drawCard();
            p->hand.push_back(card);

            // Recalculate score after drawing
            p->calculateScore();

        } else if (choice == 's') {
            // Player decides to stop
            break;

        } else {
            cout << "Invalid input, try again.\n";
        }
    }
}

// Dealer automatically draws until reaching at least 17
void dealerTurn(Dealer* dealer, Match& match) {
    cout << "\nDealer's turn...\n";

    while (dealer->getScore() < 17) {
        cout << "Dealer hits...\n";

        int card = match.drawCard();
        dealer->hand.push_back(card);

        dealer->calculateScore();
    }

    cout << "Dealer stands with score: " << dealer->getScore() << '\n';
}

// Reset all players and dealer for a new round
void resetMatch(Match& match, Dealer* dealer) {
    // Reset players
    for (auto p : match.players) {
        p->reset();
    }

    // Reset dealer
    dealer->reset();

    // Reinitialize and shuffle deck
    match.deck.clear();
    match.deckInit();
    match.shuffle();
}

int main() {
    Match match;
    Dealer* dealer = new Dealer();
    match.setDealer(dealer);

    cout << "How many players?\n";
    int n; cin >> n;

    for (int i = 0; i < n; i++) { // fixed n (not n-1)
        match.addPlayer(new Player());
    }

    while (true) {
        // Reset game state each round
        resetMatch(match, dealer);

        // Deal initial 2 cards
        match.deal2Card();

        // ===== PLAYER TURNS =====
        for (auto p : match.players) {
            cout << "\n--- Player Turn ---\n";
            playerTurn(p, match);
        }

        // ===== DEALER TURN =====
        dealerTurn(dealer, match);

        // ===== RESULT =====
        cout << "\n--- FINAL RESULT ---\n";
        Process(match, dealer);

        // Ask to play again
        cout << "\nPlay again? (y/n): ";
        char c; cin >> c;
        if (c == 'n') break;
    }

    delete dealer; // fix memory leak
    return 0;
}