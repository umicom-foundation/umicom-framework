/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/compiler_result.c
 *
 * PURPOSE:
 *   Track compilation outcome evidence without embedding compiler ownership in Teacher.
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
#include "umicom/teacher/compiler_result.h"
#include <string.h>
void umi_teacher_compiler_result_init(UmiTeacherCompilerResult *value) { if (value!=NULL) { memset(value,0,sizeof(*value));
    value->state=UMI_TEACHER_STATE_READY;
    } }
UmiStatus umi_teacher_compiler_result_record(UmiTeacherCompilerResult *value,uint32_t score,int passed) { if (value==NULL || score>100U) return UMI_STATUS_INVALID_ARGUMENT;
    value->score=score;
    ++value->attempts;
    ++value->revision;
    value->state=passed?UMI_TEACHER_STATE_PASSED:UMI_TEACHER_STATE_FAILED;
    return UMI_STATUS_OK;
    }
void umi_teacher_compiler_result_set_units(UmiTeacherCompilerResult *value,uint32_t completed_units,uint32_t total_units) { if (value==NULL) return;
    value->total_units=total_units;
    value->completed_units=completed_units>total_units?total_units:completed_units;
    ++value->revision;
    if (total_units>0U && value->completed_units==total_units) value->state=UMI_TEACHER_STATE_COMPLETED;
    }
uint32_t umi_teacher_compiler_result_completion(const UmiTeacherCompilerResult *value) { return value==NULL?0U:umi_teacher_percent(value->completed_units,value->total_units);
    }
int umi_teacher_compiler_result_complete(const UmiTeacherCompilerResult *value) { return value!=NULL && value->state==UMI_TEACHER_STATE_COMPLETED;
    }
