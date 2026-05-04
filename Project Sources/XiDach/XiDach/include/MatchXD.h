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

	virtual int playerWantsXD(Hand& p, Hand& d, StatXD& stat);

	void ProcessSimulationXD(Dealer& dealer, StatXD& stat);

	virtual void SimulationXD(ofstream& file);

	virtual bool dealerWantsToCheck(Hand& p, Hand& d);

	virtual int dealerWantsToAction(Hand& d);

	void ProcessPlayPlayer(Dealer& d);

	void PrintState();

	void PlayDealerCheckAll(Dealer& dealer);

	void DealerOptions(Dealer& dealer);

	void ProcessPlayDealer(Dealer& dealer, StatXD& stat);

	void PlayPlayer();

	void PlayDealer();

	void PlayXD();
};

class MatchXDAI : public MatchXD {
protected:
	Table3x3 HardHitTable = { {
		{{0.5, 0.5, 0.5}},    // 16 điểm (2, 3, 4 lá)
		{{0.5, 0.5, 0.5}},    // 17 điểm
		{{0.5, 0.5, 0.5}}     // 18 điểm
	} };

	Table3x3 SoftHitTable = { {
		{{0.5, 0.5, 0.5}},    // 16 điểm (2, 3, 4 lá)
		{{0.5, 0.5, 0.5}},    // 17 điểm
		{{0.5, 0.5, 0.5}}     // 18 điểm
	} };
	double mult = 0.1;
	vector<Table3x3> Hard;
	vector<Table3x3> Soft;
public:
	void LateLearning(StatXD& stat);
	void FirstLearning(StatXD& stat);

	void PrintTable(StatXD& stat) const;

	int playerWantsXD(Hand& p, Hand& d, StatXD& stat) override;

	bool dealerWantsToCheck(Hand& p, Hand& d) override;

	int dealerWantsToAction(Hand& d) override;

	void SimulationXD(ofstream& file) override;
};