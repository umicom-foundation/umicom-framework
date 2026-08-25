/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/difficulty_model.c
 *
 * PURPOSE:
 *   Calculate normalized activity difficulty.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable learning capability. Umicom Studio, Desk and
 *   future applications are thin consumers and do not reimplement pedagogy,
 *   progression, assessment or AI Teacher orchestration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/difficulty_model.h"
void umi_teacher_difficulty_model_init(UmiTeacherDifficultyModel *rubric) { if(rubric!=NULL) { rubric->correctness_weight=60U;
    rubric->quality_weight=25U;
    rubric->efficiency_weight=15U;
    rubric->minimum_score=70U;
    } }
UmiStatus umi_teacher_difficulty_model_configure(UmiTeacherDifficultyModel *rubric,uint32_t correctness_weight,uint32_t quality_weight,uint32_t efficiency_weight,uint32_t minimum_score) { if(rubric==NULL || correctness_weight+quality_weight+efficiency_weight!=100U || minimum_score>100U) return UMI_STATUS_INVALID_ARGUMENT;
    rubric->correctness_weight=correctness_weight;
    rubric->quality_weight=quality_weight;
    rubric->efficiency_weight=efficiency_weight;
    rubric->minimum_score=minimum_score;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_difficulty_model_compute(const UmiTeacherDifficultyModel *rubric,uint32_t correctness,uint32_t quality,uint32_t efficiency) { uint64_t total;
    if(rubric==NULL) return 0U;
    total=(uint64_t)umi_teacher_clamp_score(correctness)*rubric->correctness_weight + (uint64_t)umi_teacher_clamp_score(quality)*rubric->quality_weight + (uint64_t)umi_teacher_clamp_score(efficiency)*rubric->efficiency_weight;
    return (uint32_t)(total/UINT64_C(100));
    }
int umi_teacher_difficulty_model_passes(const UmiTeacherDifficultyModel *rubric,uint32_t score) { return rubric!=NULL && score>=rubric->minimum_score;
    }
