/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/replay_cursor.c
 *
 * PURPOSE:
 *   Track monotonic replay sequence progress.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of replay cursor. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/replay_cursor.h"
#include "umicom/trading/replay_event.h"
void umi_replay_cursor_init(UmiReplayCursor *c,uint64_t first){if(c!=NULL)c->next_sequence=first;}
int umi_replay_cursor_accept(UmiReplayCursor *c,const UmiReplayEvent *e){if(c==NULL||!umi_replay_event_valid(e)||e->sequence!=c->next_sequence)return 0;c->next_sequence++;return 1;}
