/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_evolution_goal.c
 *
 * PURPOSE:
 *   Regression coverage for describe a bounded software-evolution objective with measurable fitness and compatibility constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/evolution_goal.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixEvolutionGoal r; umi_helix_evolution_goal_init(&r,"evolution_goal"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_evolution_goal_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_evolution_goal_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
