/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_fitness_score.c
 *
 * PURPOSE:
 *   Regression coverage for combine correctness, quality, performance, compatibility and change-cost dimensions into a score.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/fitness_score.h"
int main(void){ UmiHelixFitnessScore s={1.0,0.8,0.7,1.0,0.2,0.0}; double v=umi_helix_fitness_score_compute(&s); if(v<0.8||v>1.0||s.total!=v){ return 1; } return 0; }
