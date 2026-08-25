/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/adaptive_progression.c
 *
 * PURPOSE:
 *   Choose advance/repeat/remediate progression from mastery evidence.
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
#include "umicom/teacher/adaptive_progression.h"
void umi_teacher_adaptive_progression_init(UmiTeacherAdaptiveProgression *model) { if(model!=NULL) { model->remediate_below=45U;
    model->repeat_below=70U;
    model->advance_at=80U;
    model->minimum_confidence=50U;
    } }
UmiStatus umi_teacher_adaptive_progression_configure(UmiTeacherAdaptiveProgression *model,uint32_t remediate_below,uint32_t repeat_below,uint32_t advance_at,uint32_t minimum_confidence) { if(model==NULL||remediate_below>repeat_below||repeat_below>advance_at||advance_at>100U||minimum_confidence>100U) return UMI_STATUS_INVALID_ARGUMENT;
    model->remediate_below=remediate_below;
    model->repeat_below=repeat_below;
    model->advance_at=advance_at;
    model->minimum_confidence=minimum_confidence;
    return UMI_STATUS_OK;
    }
UmiTeacherProgressDecision umi_teacher_adaptive_progression_decide(const UmiTeacherAdaptiveProgression *model,uint32_t mastery,uint32_t confidence) { if(model==NULL||confidence<model->minimum_confidence) return UMI_TEACHER_PROGRESS_HOLD;
    if(mastery<model->remediate_below) return UMI_TEACHER_PROGRESS_REMEDIATE;
    if(mastery<model->advance_at) return UMI_TEACHER_PROGRESS_REPEAT;
    return UMI_TEACHER_PROGRESS_ADVANCE;
    }
