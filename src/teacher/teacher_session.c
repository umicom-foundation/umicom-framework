/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/teacher_session.c
 *
 * PURPOSE:
 *   Aggregate learner, plan, AI, assessment and progress readiness for a Teacher session.
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
#include "umicom/teacher/teacher_session.h"
#include <string.h>
/*
 * Copy teacher teacher session into module-owned storage so callers keep ownership of
 * their input values.
 */
static void umi_teacher_teacher_session_copy(char *destination, size_t capacity, const char *source) {
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
 * Initialise teacher teacher session from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_teacher_session_init(UmiTeacherTeacherSession *session,const char *id) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL) return;
    memset(session,0,sizeof(*session));
    umi_teacher_teacher_session_copy(session->id,sizeof(session->id),id);
    session->state=UMI_TEACHER_STATE_READY;
    }
/*
 * Provide the teacher teacher session record activity operation used by this module and
 * its client applications.
 */
UmiStatus umi_teacher_teacher_session_record_activity(UmiTeacherTeacherSession *session,int completed,uint32_t mastery) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session==NULL||mastery>100U) return UMI_STATUS_INVALID_ARGUMENT;
    ++session->activities_started;
    /* Apply this branch only when its contract condition is satisfied. */
    if(completed) ++session->activities_completed;
    session->mastery=mastery;
    session->state=UMI_TEACHER_STATE_ACTIVE;
    ++session->revision;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher teacher session record question operation used by this module and
 * its client applications.
 */
void umi_teacher_teacher_session_record_question(UmiTeacherTeacherSession *session) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL) { ++session->questions_asked;
    ++session->revision;
    } }
/*
 * Provide the teacher teacher session add minutes operation used by this module and its
 * client applications.
 */
void umi_teacher_teacher_session_add_minutes(UmiTeacherTeacherSession *session,uint32_t minutes) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL) { session->elapsed_minutes+=minutes;
    ++session->revision;
    } }
/*
 * Provide the teacher teacher session completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_teacher_session_completion(const UmiTeacherTeacherSession *session) { return session==NULL?0U:umi_teacher_percent(session->activities_completed,session->activities_started);
    }
/*
 * Provide the teacher teacher session ready to close operation used by this module and its
 * client applications.
 */
int umi_teacher_teacher_session_ready_to_close(const UmiTeacherTeacherSession *session) { return session!=NULL && session->activities_started>0U && session->activities_completed==session->activities_started;
    }
