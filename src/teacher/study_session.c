/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/study_session.c
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
#include "umicom/teacher/study_session.h"
#include <string.h>
/*
 * Copy teacher study session into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_teacher_study_session_copy(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < capacity && source[i] != '\0') {
            destination[i] = source[i];
            ++i;
        }
    }
    destination[i] = '\0';
}

/*
 * Initialise teacher study session from caller-provided values so later operations receive
 * a known state.
 */
void umi_teacher_study_session_init(UmiTeacherStudySession *session,const char *id) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL) return;
    memset(session,0,sizeof(*session));
    umi_teacher_study_session_copy(session->id,sizeof(session->id),id);
    session->state=UMI_TEACHER_STATE_READY;
    }
/*
 * Provide the teacher study session record activity operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_study_session_record_activity(UmiTeacherStudySession *session,int completed,uint32_t mastery) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||mastery>100U) return UMI_STATUS_INVALID_ARGUMENT;
    ++session->activities_started;
    /* Apply this branch only when its contract condition is satisfied. */
    if(completed) ++session->activities_completed;
    session->mastery=mastery;
    session->state=UMI_TEACHER_STATE_ACTIVE;
    ++session->revision;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher study session record question operation used by this module and its
 * client applications.
 */
void umi_teacher_study_session_record_question(UmiTeacherStudySession *session) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL) { ++session->questions_asked;
    ++session->revision;
    } }
/*
 * Provide the teacher study session add minutes operation used by this module and its
 * client applications.
 */
void umi_teacher_study_session_add_minutes(UmiTeacherStudySession *session,uint32_t minutes) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL) { session->elapsed_minutes+=minutes;
    ++session->revision;
    } }
/*
 * Provide the teacher study session completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_study_session_completion(const UmiTeacherStudySession *session) { return session==NULL?0U:umi_teacher_percent(session->activities_completed,session->activities_started);
    }
/*
 * Provide the teacher study session ready to close operation used by this module and its
 * client applications.
 */
int umi_teacher_study_session_ready_to_close(const UmiTeacherStudySession *session) { return session!=NULL && session->activities_started>0U && session->activities_completed==session->activities_started;
    }
