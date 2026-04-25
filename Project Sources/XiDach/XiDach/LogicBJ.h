#pragma once
#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

void doResultBJ(Hand& p, Hand& d, int& result, StatBJ& stat);

int playerWantsBJ(Hand& p, Hand& d);

bool playerInsure(Hand& p, Hand& d);

bool playerDouble(Hand& p, Hand& d);

void ProcessSimulationBJ(Match& match, Dealer& dealer, StatBJ& stat);

void SimulationBJ(ofstream& file);

bool DealerWantsBJ(Match& match, Dealer& d);

void AnotherRound();
void Options(Player& p, Hand& h, Dealer& d, int x);
void PrintState(Player& p, Dealer& d);
void PrintPBust(Hand& h);
void PrintResult(Player& p, Dealer& d);
void AISuggest(Player& p, Hand& h, Dealer& d);


void ProcessPlayBJ(Match& match, Dealer& d);

void PlayBJ();