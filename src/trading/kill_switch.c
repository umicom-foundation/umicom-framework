/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/kill_switch.c
 *
 * PURPOSE:
 *   Represent an explicit trading kill switch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of kill switch. Product-specific UI and vendor details stay outside this file.
 */

#include <stdio.h>
#include "umicom/trading/kill_switch.h"
/*
 * Provide the kill switch engage operation used by this module and its client
 * applications.
 */
void umi_kill_switch_engage(UmiTradingKillSwitch *s,const char *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){s->engaged=1;(void)snprintf(s->reason,sizeof(s->reason),"%s",r!=NULL?r:"operator kill switch");}}
/* Release or reset state held by kill switch so the same storage can be reused safely. */
void umi_kill_switch_reset(UmiTradingKillSwitch *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){s->engaged=0;s->reason[0]='\0';}}
