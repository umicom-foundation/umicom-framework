/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/teacher_chat_session.c
 *
 * PURPOSE:
 *   Track a durable AI Teacher conversation session.
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
#include "umicom/teacher/teacher_chat_session.h"
#include <string.h>
static void umi_teacher_teacher_chat_session_copy(char *destination, size_t capacity, const char *source) {
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

void umi_teacher_teacher_chat_session_init(UmiTeacherTeacherChatSession *session,const char *id) { if(session==NULL) return;
    memset(session,0,sizeof(*session));
    umi_teacher_teacher_chat_session_copy(session->id,sizeof(session->id),id);
    session->state=UMI_TEACHER_STATE_READY;
    }
UmiStatus umi_teacher_teacher_chat_session_record_activity(UmiTeacherTeacherChatSession *session,int completed,uint32_t mastery) { if(session==NULL||mastery>100U) return UMI_STATUS_INVALID_ARGUMENT;
    ++session->activities_started;
    if(completed) ++session->activities_completed;
    session->mastery=mastery;
    session->state=UMI_TEACHER_STATE_ACTIVE;
    ++session->revision;
    return UMI_STATUS_OK;
    }
void umi_teacher_teacher_chat_session_record_question(UmiTeacherTeacherChatSession *session) { if(session!=NULL) { ++session->questions_asked;
    ++session->revision;
    } }
void umi_teacher_teacher_chat_session_add_minutes(UmiTeacherTeacherChatSession *session,uint32_t minutes) { if(session!=NULL) { session->elapsed_minutes+=minutes;
    ++session->revision;
    } }
uint32_t umi_teacher_teacher_chat_session_completion(const UmiTeacherTeacherChatSession *session) { return session==NULL?0U:umi_teacher_percent(session->activities_completed,session->activities_started);
    }
int umi_teacher_teacher_chat_session_ready_to_close(const UmiTeacherTeacherChatSession *session) { return session!=NULL && session->activities_started>0U && session->activities_completed==session->activities_started;
    }
