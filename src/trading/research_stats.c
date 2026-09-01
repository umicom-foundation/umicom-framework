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
void umi_research_stats_init(UmiResearchStats *s){if(s!=NULL)*s=(UmiResearchStats){0};}
void umi_research_stats_add(UmiResearchStats *s,const UmiMovementEvent *e){if(s!=NULL&&e!=NULL){s->observations++;if(e->qualifies)s->qualifying_moves++;s->total_absolute_points+=e->absolute_points;}}
double umi_research_stats_mean_points(const UmiResearchStats *s){return s!=NULL&&s->observations>0U?s->total_absolute_points/(double)s->observations:0.0;}
