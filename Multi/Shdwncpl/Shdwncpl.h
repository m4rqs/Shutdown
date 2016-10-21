#pragma once

#include "ShellCommon.h"
#include "ManageServices.h"

#include "..\common\common.h"

#define OPTION_ITEMS_COUNT 3
int aiOption[OPTION_ITEMS_COUNT][8] = {
    {7, IDC_RB_SHDWNTIME, IDC_STC_TIMEINFO, IDC_STC_TIME, IDC_DTP_TIME, IDC_STC_WAITINFO, IDC_STC_WAIT, IDC_CB_WAIT},
    {4, IDC_RB_SHDWNIDLE, IDC_STC_IDLEINFO, IDC_STC_IDLE, IDC_CB_IDLE, 0, 0, 0},
    {2, IDC_RB_SHDWNNO, IDC_STC_SHDWNNOINFO, 0, 0, 0, 0, 0}
};
