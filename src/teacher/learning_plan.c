/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_plan.c
 *
 * PURPOSE:
 *   Maintain an ordered learner plan of lessons, exercises and assessments.
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
#include "umicom/teacher/learning_plan.h"
#include <string.h>
static void umi_teacher_learning_plan_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source != NULL) {
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

void umi_teacher_learning_plan_init(UmiTeacherLearningPlan *sequence) { if(sequence!=NULL) memset(sequence,0,sizeof(*sequence));
    }
UmiStatus umi_teacher_learning_plan_append(UmiTeacherLearningPlan *sequence,const char *id) { size_t i;
    if(sequence==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<sequence->count;++i) if(strcmp(sequence->ids[i],id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if(sequence->count>=UMI_TEACHER_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_teacher_learning_plan_copy(sequence->ids[sequence->count],UMI_TEACHER_ID_CAPACITY,id);
    ++sequence->count;
    ++sequence->revision;
    return UMI_STATUS_OK;
    }
const char *umi_teacher_learning_plan_current(const UmiTeacherLearningPlan *sequence) { if(sequence==NULL||sequence->count==0U||sequence->cursor>=sequence->count) return NULL;
    return sequence->ids[sequence->cursor];
    }
UmiStatus umi_teacher_learning_plan_next(UmiTeacherLearningPlan *sequence) { if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(sequence->cursor+1U>=sequence->count) return UMI_STATUS_NOT_FOUND;
    ++sequence->cursor;
    return UMI_STATUS_OK;
    }
UmiStatus umi_teacher_learning_plan_previous(UmiTeacherLearningPlan *sequence) { if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(sequence->cursor==0U) return UMI_STATUS_NOT_FOUND;
    --sequence->cursor;
    return UMI_STATUS_OK;
    }
size_t umi_teacher_learning_plan_count(const UmiTeacherLearningPlan *sequence) { return sequence==NULL?0U:sequence->count;
    }
