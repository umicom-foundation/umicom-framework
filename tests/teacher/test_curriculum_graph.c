/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_curriculum_graph.c
 *
 * PURPOSE:
 *   Implement the test curriculum graph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/curriculum_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTeacherCurriculumGraph g; uint8_t done[3]={1U,0U,0U}; umi_teacher_curriculum_graph_init(&g,3U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_graph_add(&g,0U,1U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_graph_add(&g,1U,2U)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_graph_has_cycle(&g)) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_teacher_curriculum_graph_prerequisite_satisfied(&g,1U,done,3U)) return 4; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_teacher_curriculum_graph_add(&g,2U,0U)!=UMI_STATUS_OK) return 5; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_teacher_curriculum_graph_has_cycle(&g)) return 6; return 0; }
