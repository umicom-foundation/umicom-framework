/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/replay_clock.c
 *
 * PURPOSE:
 *   Provide a deterministic logical time source for historical playback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of replay clock. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/replay_clock.h"
/*
 * Initialise replay clock from caller-provided values so later operations receive a known
 * state.
 */
void umi_replay_clock_init(UmiReplayClock *c,int64_t start,double speed){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL){c->now_ms=start;c->speed=speed>0.0?speed:1.0;}}
/*
 * Provide the replay clock advance operation used by this module and its client
 * applications.
 */
void umi_replay_clock_advance(UmiReplayClock *c,int64_t event_time){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL&&event_time>=c->now_ms)c->now_ms=event_time;}
