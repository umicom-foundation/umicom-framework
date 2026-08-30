/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/mastery_model.h
 *
 * PURPOSE:
 *   Update skill mastery using weighted evidence and confidence.
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
#ifndef UMICOM_TEACHER_MASTERY_MODEL_H
#define UMICOM_TEACHER_MASTERY_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherMasteryModel { uint32_t mastery; uint32_t confidence; uint32_t evidence_count; uint64_t revision; } UmiTeacherMasteryModel;
void umi_teacher_mastery_model_init(UmiTeacherMasteryModel *model);
UmiStatus umi_teacher_mastery_model_observe(UmiTeacherMasteryModel *model, uint32_t score, uint32_t evidence_weight);
uint32_t umi_teacher_mastery_model_value(const UmiTeacherMasteryModel *model);
int umi_teacher_mastery_model_meets(const UmiTeacherMasteryModel *model, uint32_t threshold, uint32_t confidence_threshold);

#ifdef __cplusplus
}
#endif

#endif
