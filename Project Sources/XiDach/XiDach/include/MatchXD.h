#pragma once
#include "Constants.h"
#include "Entities.h"
#include "Engine.h"
#include "Stats.h"

class MatchXD : public Match {
protected:
	bool Has3 = false;
	bool Has4 = false;
public:
	void setPlayerCount();
	void deductPlayerCount();
	int getPlayerCount() const;
	void set3();
	void set4();
	void no3();
	void no4();
	bool get3() const;
	bool get4() const;

	void doResultXD(Hand& p, Hand& d, int& result, StatXD& stat);

	int playerWantsXD(Hand& p, Hand& d);

	void ProcessSimulationXD(Dealer& dealer, StatXD& stat);

	void SimulationXD(ofstream& file);

	bool dealerWantsToCheck(Hand& p, Hand& d);

	int dealerWantsToAction(Hand& d);

	void ProcessPlayPlayer(Dealer& d);

	void PrintState();

	void PlayDealerCheckAll(Dealer& dealer);

	void DealerOptions(Dealer& dealer);

	void ProcessPlayDealer(Dealer& dealer);

	void PlayPlayer();

	void PlayDealer();

	void PlayXD();
};