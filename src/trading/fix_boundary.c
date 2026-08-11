/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fix_boundary.c
 *
 * PURPOSE:
 *   Validate minimal FIX-style sequence and session identifiers without binding to a specific FIX engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of fix boundary. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/fix_boundary.h"
int umi_fix_session_info_valid(const UmiFixSessionInfo *s){return s!=NULL&&s->sender_comp_id[0]!='\0'&&s->target_comp_id[0]!='\0'&&s->next_out_sequence>0U&&s->next_in_sequence>0U;}
