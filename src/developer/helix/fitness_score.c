/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/fitness_score.c
 *
 * PURPOSE:
 *   Combine correctness, quality, performance, compatibility and change-cost dimensions into a score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/fitness_score.h"
/* Weight correctness highest so a fast but incorrect variant cannot outrank valid code. */
double umi_helix_fitness_score_compute(UmiHelixFitnessScore *s){ if(s==NULL){ return 0.0; } double v=umi_helix_clamp_score(s->correctness)*0.45+umi_helix_clamp_score(s->quality)*0.20+umi_helix_clamp_score(s->performance)*0.15+umi_helix_clamp_score(s->compatibility)*0.15+(1.0-umi_helix_clamp_score(s->change_cost))*0.05; s->total=umi_helix_clamp_score(v); return s->total; }
