#include "TPS_GameMode.h"
#include "TPSProject.h"

ATPS_GameMode::ATPS_GameMode()
{
	PRINT_LOG(TEXT("My Log : %s"), TEXT("TPS project!!"));
	// 로그찍기 ("======LOG=====") 이런식으로
	PRINT_CALLINFO();
}