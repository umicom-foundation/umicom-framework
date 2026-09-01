/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/replay_event.c
 *
 * PURPOSE:
 *   Validate deterministic replay event envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of replay event. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/replay_event.h"
int umi_replay_event_valid(const UmiReplayEvent *e){return e!=NULL&&e->sequence>0U&&e->event_time_ms>=0&&e->type[0]!='\0';}
