/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/lesson_navigation.c
 *
 * PURPOSE:
 *   Track deterministic next/previous lesson navigation order.
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
#include "umicom/teacher/lesson_navigation.h"
#include <string.h>
/*
 * Copy teacher lesson navigation into module-owned storage so callers keep ownership of
 * their input values.
 */
static void umi_teacher_lesson_navigation_copy(char *destination, size_t capacity, const char *source) {
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
 * Initialise teacher lesson navigation from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_navigation_init(UmiTeacherLessonNavigation *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence!=NULL) memset(sequence,0,sizeof(*sequence));
    }
/*
 * Add teacher lesson navigation only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_teacher_lesson_navigation_append(UmiTeacherLessonNavigation *sequence,const char *id) { size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(sequence==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<sequence->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(sequence->ids[i],id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(sequence->count>=UMI_TEACHER_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_teacher_lesson_navigation_copy(sequence->ids[sequence->count],UMI_TEACHER_ID_CAPACITY,id);
    ++sequence->count;
    ++sequence->revision;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher lesson navigation current operation used by this module and its
 * client applications.
 */
const char *umi_teacher_lesson_navigation_current(const UmiTeacherLessonNavigation *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL||sequence->count==0U||sequence->cursor>=sequence->count) return NULL;
    return sequence->ids[sequence->cursor];
    }
/*
 * Provide the teacher lesson navigation next operation used by this module and its client
 * applications.
 */
UmiStatus umi_teacher_lesson_navigation_next(UmiTeacherLessonNavigation *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(sequence->cursor+1U>=sequence->count) return UMI_STATUS_NOT_FOUND;
    ++sequence->cursor;
    return UMI_STATUS_OK;
    }
/*
 * Provide the teacher lesson navigation previous operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_lesson_navigation_previous(UmiTeacherLessonNavigation *sequence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(sequence==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(sequence->cursor==0U) return UMI_STATUS_NOT_FOUND;
    --sequence->cursor;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by teacher lesson navigation without changing
 * their state.
 */
size_t umi_teacher_lesson_navigation_count(const UmiTeacherLessonNavigation *sequence) { return sequence==NULL?0U:sequence->count;
    }
