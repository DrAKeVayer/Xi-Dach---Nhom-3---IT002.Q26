#pragma once
#include "Objects.h"
#include "Constants.h"
#include "Logic.h"
#include "LogicBJ.h"
#include "LogicXD.h"

int compareHands(Hand& a, Hand& b);

void HitPlay(Match& match, Player& p, Hand& h, Dealer& d);
void InsurePlay(Match& match, Player& p, Hand& h, Dealer& d);
void SplitPlay(Match& match, Player& p, Hand& h, Dealer& d);
void SurrenderPlay(Match& match, Player& p, Hand& h, Dealer& d);
void StandPlay(Match& match, Player& p, Hand& h, Dealer& d);
void DoublePlay(Match& match, Player& p, Hand& h, Dealer& d);
void PrintResult(Match& match, Hand& h, Dealer& d, int res);
void PrintResultXD(Match& match, Player& p, Dealer& d, int res);
void PrintPBust(Hand& h);
