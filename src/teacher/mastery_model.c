/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/mastery_model.c
 *
 * PURPOSE:
 *   Update skill mastery using weighted evidence and confidence.
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
#include "umicom/teacher/mastery_model.h"
#include <string.h>
void umi_teacher_mastery_model_init(UmiTeacherMasteryModel *model) { if(model!=NULL) memset(model,0,sizeof(*model));
    }
UmiStatus umi_teacher_mastery_model_observe(UmiTeacherMasteryModel *model,uint32_t score,uint32_t evidence_weight) { uint64_t weighted,total_weight;
    if(model==NULL||score>100U||evidence_weight==0U) return UMI_STATUS_INVALID_ARGUMENT;
    weighted=(uint64_t)model->mastery*model->confidence + (uint64_t)score*evidence_weight;
    total_weight=(uint64_t)model->confidence+evidence_weight;
    model->mastery=(uint32_t)(weighted/total_weight);
    model->confidence=total_weight>100U?100U:(uint32_t)total_weight;
    ++model->evidence_count;
    ++model->revision;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_mastery_model_value(const UmiTeacherMasteryModel *model) { return model==NULL?0U:model->mastery;
    }
int umi_teacher_mastery_model_meets(const UmiTeacherMasteryModel *model,uint32_t threshold,uint32_t confidence_threshold) { return model!=NULL && model->mastery>=threshold && model->confidence>=confidence_threshold;
    }
