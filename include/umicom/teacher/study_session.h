/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/study_session.h
 *
 * PURPOSE:
 *   Track time, activity and completion metrics for one study session.
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
#ifndef UMICOM_TEACHER_STUDY_SESSION_H
#define UMICOM_TEACHER_STUDY_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher study session data shared with callers of this public contract.
 */
typedef struct UmiTeacherStudySession { char id[UMI_TEACHER_ID_CAPACITY]; uint32_t activities_started; uint32_t activities_completed; uint32_t questions_asked; uint32_t elapsed_minutes; uint32_t mastery; UmiTeacherState state; uint64_t revision; } UmiTeacherStudySession;
/**
 * Initialise teacher study session from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_study_session_init(UmiTeacherStudySession *session, const char *id);
/**
 * Provide the teacher study session record activity operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_study_session_record_activity(UmiTeacherStudySession *session, int completed, uint32_t mastery);
/**
 * Provide the teacher study session record question operation used by this module and its
 * client applications.
 */
void umi_teacher_study_session_record_question(UmiTeacherStudySession *session);
/**
 * Provide the teacher study session add minutes operation used by this module and its
 * client applications.
 */
void umi_teacher_study_session_add_minutes(UmiTeacherStudySession *session, uint32_t minutes);
/**
 * Provide the teacher study session completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_study_session_completion(const UmiTeacherStudySession *session);
/**
 * Provide the teacher study session ready to close operation used by this module and its
 * client applications.
 */
int umi_teacher_study_session_ready_to_close(const UmiTeacherStudySession *session);

#ifdef __cplusplus
}
#endif

#endif
