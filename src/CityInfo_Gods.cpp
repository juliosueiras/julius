#include "CityInfo.h"
#include "PlayerMessage.h"
#include "Data/CityInfo.h"
#include "Data/Constants.h"
#include "Data/Random.h"
#include "Data/Settings.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define TIE 10

#define UPDATE(x,val,min,max) \
	Data_CityInfo.x += val;\
	if (Data_CityInfo.x < min) Data_CityInfo.x = min; \
	else if (Data_CityInfo.x > max) Data_CityInfo.x = max;

static void performBlessingCeres()
{
	// TODO
}

static void performBlessingMercury()
{
	// TODO
}

static void performCurseCeres(int isLarge)
{
	// TODO
}

static void sinkAllShips()
{
	// TODO
}

static void performCurseMercury(int isLarge)
{
	// TODO
}

static int startLocalUprising()
{
	// TODO
	return 0;
}

static int performLargeCurseMars()
{
	// TODO
	return 0;
}

static void performBlessing(int god)
{
	switch (god) {
		case God_Ceres:
			PlayerMessage_post(1, 96, 0, 0);
			performBlessingCeres();
			break;
		case God_Neptune:
			PlayerMessage_post(1, 97, 0, 0);
			Data_CityInfo.godBlessingNeptuneDoubleTrade = 1;
			break;
		case God_Mercury:
			PlayerMessage_post(1, 98, 0, 0);
			performBlessingMercury();
			break;
		case God_Mars:
			PlayerMessage_post(1, 99, 0, 0);
			Data_CityInfo.godBlessingMarsEnemiesToKill = 10;
			break;
		case God_Venus:
			PlayerMessage_post(1, 100, 0, 0);
			CityInfo_Population_increaseHappiness(25);
			break;
	}
}

static void performSmallCurse(int god)
{
	switch (god) {
		case God_Ceres:
			PlayerMessage_post(1, 91, 0, 0);
			performCurseCeres(0);
			break;
		case God_Neptune:
			PlayerMessage_post(1, 92, 0, 0);
			sinkAllShips();
			Data_CityInfo.godCurseNeptuneSankShips = 1;
			break;
		case God_Mercury:
			PlayerMessage_post(1, 93, 0, 0);
			performCurseMercury(0);
			break;
		case God_Mars:
			if (startLocalUprising()) {
				PlayerMessage_post(1, 94, 0, 0);
			} else {
				PlayerMessage_post(1, 44, 0, 0);
			}
			break;
		case God_Venus:
			PlayerMessage_post(1, 95, 0, 0);
			CityInfo_Population_setMaxHappiness(50);
			CityInfo_Population_increaseHappiness(-5);
			CityInfo_Population_changeHealthRate(-10);
			CityInfo_Population_calculateSentiment();
	}
}

static void performLargeCurse(int god)
{
	switch (god) {
		case God_Ceres:
			PlayerMessage_post(1, 41, 0, 0);
			performCurseCeres(1);
			break;
		case God_Neptune:
			if (Data_CityInfo.tradeNumOpenSeaRoutes) {
				PlayerMessage_post(1, 42, 0, 0);
			} else {
				PlayerMessage_post(1, 81, 0, 0);
				sinkAllShips();
				Data_CityInfo.godCurseNeptuneSankShips = 1;
				Data_CityInfo.tradeSeaProblemDuration = 80;
			}
			break;
		case God_Mercury:
			PlayerMessage_post(1, 43, 0, 0);
			performCurseMercury(1);
			break;
		case God_Mars:
			if (performLargeCurseMars()) {
				PlayerMessage_post(1, 82, 0, 0);
				startLocalUprising();
			} else {
				PlayerMessage_post(1, 44, 0, 0);
			}
			break;
		case God_Venus:
			PlayerMessage_post(1, 45, 0, 0);
			CityInfo_Population_setMaxHappiness(40);
			CityInfo_Population_increaseHappiness(-10);
			if (Data_CityInfo.healthRate >= 80) {
				CityInfo_Population_changeHealthRate(-50);
			} else if (Data_CityInfo.healthRate >= 60) {
				CityInfo_Population_changeHealthRate(-40);
			} else {
				CityInfo_Population_changeHealthRate(-20);
			}
			Data_CityInfo.godCurseVenusActive = 1;
			CityInfo_Population_calculateSentiment();
			break;
	}
}

static void updateGodMoods()
{
	for (int i = 0; i < 5; i++) {
		if (Data_CityInfo.godHappiness[i] < Data_CityInfo.godTargetHappiness[i]) {
			Data_CityInfo.godHappiness[i]++;
		} else if (Data_CityInfo.godHappiness[i] < Data_CityInfo.godTargetHappiness[i]) {
			Data_CityInfo.godHappiness[i]--;
		}
		if (Data_Settings.currentMissionId == 0 && !Data_Settings.isCustomScenario) {
			// tutorial mission 1
			if (Data_CityInfo.godHappiness[i] < 50) {
				Data_CityInfo.godHappiness[i] = 50;
			}
		}
	}
	for (int i = 0; i < 5; i++) {
		if (Data_CityInfo.godHappiness[i] > 50) {
			Data_CityInfo.godSmallCurseDone[i] = 0;
		}
		if (Data_CityInfo.godHappiness[i] < 50) {
			Data_CityInfo.godBlessingDone[i] = 0;
		}
	}

	int god = Data_Random.random1_7bit & 7;
	if (god <= 4) {
		if (Data_CityInfo.godHappiness[god] >= 50) {
			Data_CityInfo.godWrathBolts[god] = 0;
		} else if (Data_CityInfo.godHappiness[god] < 40) {
			if (Data_CityInfo.godHappiness[god] >= 20) {
				Data_CityInfo.godWrathBolts[god] += 1;
			} else if (Data_CityInfo.godHappiness[god] >= 10) {
				Data_CityInfo.godWrathBolts[god] += 2;
			} else {
				Data_CityInfo.godWrathBolts[god] += 5;
			}
		}
		if (Data_CityInfo.godWrathBolts[god] > 50) {
			Data_CityInfo.godWrathBolts[god] = 50;
		}
	}
	if (Data_CityInfo_Extra.gameTimeWeek != 0) {
		return;
	}

	// handle blessings, curses, etc every month
	for (int i = 0; i < 5; i++) {
		Data_CityInfo.godMonthsSinceFestival[i]++;
	}
	if (god > 4) {
		if (CityInfo_Gods_calculateLeastHappy()) {
			god = Data_CityInfo.godLeastHappy - 1;
		}
	}
	if (!Data_Settings.godsEnabled) {
		return;
	}
	if (god <= 4) {
		if (Data_CityInfo.godHappiness[god] >= 100 &&
				!Data_CityInfo.godBlessingDone) {
			Data_CityInfo.godBlessingDone[god] = 1;
			performBlessing(god);
		} else if (Data_CityInfo.godWrathBolts[god] >= 20 &&
				!Data_CityInfo.godSmallCurseDone[god] &&
				Data_CityInfo.godMonthsSinceFestival[god] > 3) {
			Data_CityInfo.godSmallCurseDone[god] = 1;
			Data_CityInfo.godWrathBolts[god] = 0;
			Data_CityInfo.godHappiness[god] += 12;
			performSmallCurse(god);
		} else if (Data_CityInfo.godWrathBolts[god] >= 50 &&
				Data_CityInfo.godMonthsSinceFestival[god] > 3) {
			if (Data_Settings.currentMissionId < 4 && !Data_Settings.isCustomScenario) {
				// no large curses in early scenarios
				Data_CityInfo.godSmallCurseDone[god] = 0;
				return;
			}
			Data_CityInfo.godWrathBolts[god] = 0;
			Data_CityInfo.godHappiness[god] += 30;
			performLargeCurse(god);
		}
	}

	int minHappiness = 100;
	for (int i = 0; i < 5; i++) {
		if (Data_CityInfo.godHappiness[i] < minHappiness) {
			minHappiness = Data_CityInfo.godHappiness[i];
		}
	}
	if (Data_CityInfo.godAngryMessageDelay) {
		Data_CityInfo.godAngryMessageDelay--;
	} else if (minHappiness < 30) {
		Data_CityInfo.godAngryMessageDelay = 20;
		if (minHappiness < 10) {
			PlayerMessage_post(0, 101, 0, 0);
		} else {
			PlayerMessage_post(0, 55, 0, 0);
		}
	}
}

void CityInfo_Gods_calculateMoods(int updateMoods)
{
	// base happiness: percentage of houses covered
	Data_CityInfo.godTargetHappiness[God_Ceres] = Data_CityInfo_CultureCoverage.religionCeres;
	Data_CityInfo.godTargetHappiness[God_Neptune] = Data_CityInfo_CultureCoverage.religionNeptune;
	Data_CityInfo.godTargetHappiness[God_Mercury] = Data_CityInfo_CultureCoverage.religionMercury;
	Data_CityInfo.godTargetHappiness[God_Mars] = Data_CityInfo_CultureCoverage.religionMars;
	Data_CityInfo.godTargetHappiness[God_Venus] = Data_CityInfo_CultureCoverage.religionVenus;

	// happiness factor based on months since festival (max 40)
	for (int i = 0; i < 5; i++) {
		int festivalPenalty = Data_CityInfo.godMonthsSinceFestival[i];
		if (festivalPenalty > 40) {
			festivalPenalty = 40;
		}
		Data_CityInfo.godTargetHappiness[i] += 12 - festivalPenalty;
	}

	int maxTemples = 0;
	int maxGod = TIE;
	int minTemples = 100000;
	int minGod = TIE;
	for (int i = 0; i < 5; i++) {
		int numTemples = 0;
		switch (i) {
			case God_Ceres:
				numTemples = Data_CityInfo_Buildings.smallTempleCeres.total + Data_CityInfo_Buildings.largeTempleCeres.total;
				break;
			case God_Neptune:
				numTemples = Data_CityInfo_Buildings.smallTempleNeptune.total + Data_CityInfo_Buildings.largeTempleNeptune.total;
				break;
			case God_Mercury:
				numTemples = Data_CityInfo_Buildings.smallTempleMercury.total + Data_CityInfo_Buildings.largeTempleMercury.total;
				break;
			case God_Mars:
				numTemples = Data_CityInfo_Buildings.smallTempleMars.total + Data_CityInfo_Buildings.largeTempleMars.total;
				break;
			case God_Venus:
				numTemples = Data_CityInfo_Buildings.smallTempleVenus.total + Data_CityInfo_Buildings.largeTempleVenus.total;
				break;
		}
		if (numTemples == maxTemples) {
			maxGod = TIE;
		} else if (numTemples > maxTemples) {
			maxTemples = numTemples;
			maxGod = i;
		}
		if (numTemples == minTemples) {
			minGod = TIE;
		} else if (numTemples < minTemples) {
			minTemples = numTemples;
			minGod = i;
		}
	}
	// BUGFIX poor Venus never gets points here!
	if (maxGod < 4) {
		if (Data_CityInfo.godTargetHappiness[maxGod] >= 50) {
			Data_CityInfo.godTargetHappiness[maxGod] = 100;
		} else {
			Data_CityInfo.godTargetHappiness[maxGod] += 50;
		}
	}
	if (minGod < 4) {
		Data_CityInfo.godTargetHappiness[minGod] -= 25;
	}
	int minHappiness;
	if (Data_CityInfo.population < 100) {
		minHappiness = 50;
	} else if (Data_CityInfo.population < 200) {
		minHappiness = 40;
	} else if (Data_CityInfo.population < 300) {
		minHappiness = 30;
	} else if (Data_CityInfo.population < 400) {
		minHappiness = 20;
	} else if (Data_CityInfo.population < 500) {
		minHappiness = 10;
	} else {
		minHappiness = 0;
	}
	for (int i = 0; i < 5; i++) {
		if (Data_CityInfo.godTargetHappiness[i] > 100) {
			Data_CityInfo.godTargetHappiness[i] = 100;
		}
		if (Data_CityInfo.godTargetHappiness[i] < minHappiness) {
			Data_CityInfo.godTargetHappiness[i] = minHappiness;
		}
	}
	if (updateMoods) {
		updateGodMoods();
	}
}

int CityInfo_Gods_calculateLeastHappy()
{
	int maxGod = 0;
	int maxWrath = 0;
	for (int i = 0; i < 5; i++) {
		if (Data_CityInfo.godWrathBolts[i] > maxWrath) {
			maxGod = i + 1;
			maxWrath = Data_CityInfo.godWrathBolts[i];
		}
	}
	if (maxGod <= 0) {
		int minHappiness = 40;
		for (int i = 0; i < 5; i++) {
			if (Data_CityInfo.godHappiness[i] < minHappiness) {
				maxGod = i + 1;
				minHappiness = Data_CityInfo.godHappiness[i];
			}
		}
		Data_CityInfo.godLeastHappy = maxGod;
		return maxGod > 0;
	} else {
		Data_CityInfo.godLeastHappy = maxGod;
		return 1;
	}
}

void CityInfo_Gods_reset()
{
	for (int i = 0; i < 5; i++) {
		Data_CityInfo.godTargetHappiness[i] = 50;
		Data_CityInfo.godHappiness[i] = 50;
		Data_CityInfo.godWrathBolts[i] = 0;
		Data_CityInfo.godBlessingDone[i] = 0;
		Data_CityInfo.godSmallCurseDone[i] = 0;
		Data_CityInfo.godUnused1[i] = 0;
		Data_CityInfo.godUnused2[i] = 0;
		Data_CityInfo.godUnused3[i] = 0;
		Data_CityInfo.godMonthsSinceFestival[i] = 0;
	}
	Data_CityInfo.godAngryMessageDelay = 0;
}