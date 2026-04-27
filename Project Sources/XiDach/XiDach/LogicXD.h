#pragma once
#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

void doResultXD(Hand& p, Hand& d, int& result, StatXD& stat);

int playerWantsXD(Hand& p, Hand& d);

void ProcessSimulationXD(Match& match, Dealer& dealer, StatXD& stat);

void SimulationXD(ofstream& file);

bool dealerWantsToCheck(Hand& p, Hand& d);

int dealerWantsToAction(Match& match, Hand& d);

void ProcessPlayPlayer(Match& match, Dealer& d);

void PrintState(Match& match);

void PlayDealerCheckAll(Match& match, Dealer& dealer);

void DealerOptions(Match& match, Dealer& dealer);

void ProcessPlayDealer(Match& match, Dealer& dealer);

void PlayPlayer();

void PlayDealer();
