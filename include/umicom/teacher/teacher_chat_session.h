/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/teacher_chat_session.h
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
#ifndef UMICOM_TEACHER_TEACHER_CHAT_SESSION_H
#define UMICOM_TEACHER_TEACHER_CHAT_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTeacherTeacherChatSession { char id[UMI_TEACHER_ID_CAPACITY]; uint32_t activities_started; uint32_t activities_completed; uint32_t questions_asked; uint32_t elapsed_minutes; uint32_t mastery; UmiTeacherState state; uint64_t revision; } UmiTeacherTeacherChatSession;
void umi_teacher_teacher_chat_session_init(UmiTeacherTeacherChatSession *session, const char *id);
UmiStatus umi_teacher_teacher_chat_session_record_activity(UmiTeacherTeacherChatSession *session, int completed, uint32_t mastery);
void umi_teacher_teacher_chat_session_record_question(UmiTeacherTeacherChatSession *session);
void umi_teacher_teacher_chat_session_add_minutes(UmiTeacherTeacherChatSession *session, uint32_t minutes);
uint32_t umi_teacher_teacher_chat_session_completion(const UmiTeacherTeacherChatSession *session);
int umi_teacher_teacher_chat_session_ready_to_close(const UmiTeacherTeacherChatSession *session);

#ifdef __cplusplus
}
#endif

#endif
