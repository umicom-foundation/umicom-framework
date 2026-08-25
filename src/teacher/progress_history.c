/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/progress_history.c
 *
 * PURPOSE:
 *   Maintain bounded historical progress evidence.
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
#include "umicom/teacher/progress_history.h"
#include <string.h>
void umi_teacher_progress_history_init(UmiTeacherProgressHistory *history) { if(history!=NULL) memset(history,0,sizeof(*history));
    }
void umi_teacher_progress_history_record(UmiTeacherProgressHistory *history,uint64_t revision,uint32_t mastery,uint32_t completion,uint32_t score) { size_t slot;
    if(history==NULL) return;
    slot=history->next_slot;
    history->entries[slot].revision=revision;
    history->entries[slot].mastery=umi_teacher_clamp_score(mastery);
    history->entries[slot].completion=umi_teacher_clamp_score(completion);
    history->entries[slot].score=umi_teacher_clamp_score(score);
    history->next_slot=(slot+1U)%UMI_TEACHER_MEDIUM_CAPACITY;
    if(history->count<UMI_TEACHER_MEDIUM_CAPACITY) ++history->count;
    }
size_t umi_teacher_progress_history_count(const UmiTeacherProgressHistory *history) { return history==NULL?0U:history->count;
    }
const UmiTeacherProgressHistoryEntry *umi_teacher_progress_history_latest(const UmiTeacherProgressHistory *history) { size_t slot;
    if(history==NULL||history->count==0U) return NULL;
    slot=(history->next_slot+UMI_TEACHER_MEDIUM_CAPACITY-1U)%UMI_TEACHER_MEDIUM_CAPACITY;
    return &history->entries[slot];
    }
