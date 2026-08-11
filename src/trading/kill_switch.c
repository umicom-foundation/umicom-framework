/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/kill_switch.c
 *
 * PURPOSE:
 *   Represent an explicit trading kill switch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of kill switch. Product-specific UI and vendor details stay outside this file.
 */

#include <stdio.h>
#include "umicom/trading/kill_switch.h"
void umi_kill_switch_engage(UmiTradingKillSwitch *s,const char *r){if(s!=NULL){s->engaged=1;(void)snprintf(s->reason,sizeof(s->reason),"%s",r!=NULL?r:"operator kill switch");}}
void umi_kill_switch_reset(UmiTradingKillSwitch *s){if(s!=NULL){s->engaged=0;s->reason[0]='\0';}}
