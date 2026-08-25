/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/exercise_attempt.c
 *
 * PURPOSE:
 *   Track one learner exercise attempt and its score.
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
#include "umicom/teacher/exercise_attempt.h"
#include <string.h>
static void umi_teacher_exercise_attempt_copy(char *destination, size_t capacity, const char *source) {
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

void umi_teacher_exercise_attempt_init(UmiTeacherExerciseAttempt *attempt) { if(attempt!=NULL) memset(attempt,0,sizeof(*attempt));
    }
UmiStatus umi_teacher_exercise_attempt_begin(UmiTeacherExerciseAttempt *attempt,const char *activity_id,uint32_t attempt_number) { if(attempt==NULL||activity_id==NULL||activity_id[0]=='\0'||attempt_number==0U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_exercise_attempt_init(attempt);
    umi_teacher_exercise_attempt_copy(attempt->activity_id,sizeof(attempt->activity_id),activity_id);
    attempt->attempt_number=attempt_number;
    return UMI_STATUS_OK;
    }
UmiStatus umi_teacher_exercise_attempt_finish(UmiTeacherExerciseAttempt *attempt,uint32_t score,uint32_t hints_used,uint32_t elapsed_seconds,uint32_t pass_score) { if(attempt==NULL||attempt->activity_id[0]=='\0'||score>100U||pass_score>100U) return UMI_STATUS_INVALID_ARGUMENT;
    attempt->score=score;
    attempt->hints_used=hints_used;
    attempt->elapsed_seconds=elapsed_seconds;
    attempt->passed=score>=pass_score;
    return UMI_STATUS_OK;
    }
uint32_t umi_teacher_exercise_attempt_effective_score(const UmiTeacherExerciseAttempt *attempt,uint32_t hint_penalty) { uint64_t penalty;
    if(attempt==NULL) return 0U;
    penalty=(uint64_t)attempt->hints_used*hint_penalty;
    return penalty>=attempt->score?0U:(uint32_t)((uint64_t)attempt->score-penalty);
    }
