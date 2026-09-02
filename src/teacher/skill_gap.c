/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/skill_gap.c
 *
 * PURPOSE:
 *   Calculate the gap between current and required mastery.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/skill_gap.h"
/*
 * Initialise teacher skill gap from caller-provided values so later operations receive a
 * known state.
 */
void umi_teacher_skill_gap_init(UmiTeacherSkillGap *rubric) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(rubric!=NULL) { rubric->correctness_weight=60U;
    rubric->quality_weight=25U;
    rubric->efficiency_weight=15U;
    rubric->minimum_score=70U;
    } }
/*
 * Provide the teacher skill gap configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_skill_gap_configure(UmiTeacherSkillGap *rubric,uint32_t correctness_weight,uint32_t quality_weight,uint32_t efficiency_weight,uint32_t minimum_score) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(rubric==NULL || correctness_weight+quality_weight+efficiency_weight!=100U || minimum_score>100U) return UMI_STATUS_INVALID_ARGUMENT;
    rubric->correctness_weight=correctness_weight;
    rubric->quality_weight=quality_weight;
    rubric->efficiency_weight=efficiency_weight;
    rubric->minimum_score=minimum_score;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher skill gap compute operation used by this module and its client
 * applications.
 */
uint32_t umi_teacher_skill_gap_compute(const UmiTeacherSkillGap *rubric,uint32_t correctness,uint32_t quality,uint32_t efficiency) { uint64_t total;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(rubric==NULL) return 0U;
    total=(uint64_t)umi_teacher_clamp_score(correctness)*rubric->correctness_weight + (uint64_t)umi_teacher_clamp_score(quality)*rubric->quality_weight + (uint64_t)umi_teacher_clamp_score(efficiency)*rubric->efficiency_weight;
    return (uint32_t)(total/UINT64_C(100));
    }
/*
 * Provide the teacher skill gap passes operation used by this module and its client
 * applications.
 */
int umi_teacher_skill_gap_passes(const UmiTeacherSkillGap *rubric,uint32_t score) { return rubric!=NULL && score>=rubric->minimum_score;
    }
