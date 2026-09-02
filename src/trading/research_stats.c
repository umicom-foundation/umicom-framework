/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/research_stats.c
 *
 * PURPOSE:
 *   Accumulate qualifying movement counts and mean absolute movement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of research stats. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/research_stats.h"
/*
 * Initialise research stats from caller-provided values so later operations receive a
 * known state.
 */
void umi_research_stats_init(UmiResearchStats *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)*s=(UmiResearchStats){0};}
/* Add research stats only after its inputs and available capacity have been checked. */
void umi_research_stats_add(UmiResearchStats *s,const UmiMovementEvent *e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL&&e!=NULL){s->observations++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->qualifies)s->qualifying_moves++;s->total_absolute_points+=e->absolute_points;}}
/*
 * Provide the research stats mean points operation used by this module and its client
 * applications.
 */
double umi_research_stats_mean_points(const UmiResearchStats *s){return s!=NULL&&s->observations>0U?s->total_absolute_points/(double)s->observations:0.0;}
