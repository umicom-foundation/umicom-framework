/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/lesson_progress.c
 *
 * PURPOSE:
 *   Track lesson progress, attempts and completion evidence.
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
#include "umicom/teacher/lesson_progress.h"
#include <string.h>
/*
 * Initialise teacher lesson progress from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_progress_init(UmiTeacherLessonProgress *progress) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(progress!=NULL) memset(progress,0,sizeof(*progress));
    }
/*
 * Provide the teacher lesson progress update operation used by this module and its client
 * applications.
 */
void umi_teacher_lesson_progress_update(UmiTeacherLessonProgress *progress,uint32_t sections_completed,uint32_t section_count,uint32_t exercises_passed,uint32_t exercise_count,uint32_t mastery) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(progress==NULL) return;
    progress->section_count=section_count;
    progress->sections_completed=sections_completed>section_count?section_count:sections_completed;
    progress->exercise_count=exercise_count;
    progress->exercises_passed=exercises_passed>exercise_count?exercise_count:exercises_passed;
    progress->mastery=umi_teacher_clamp_score(mastery);
    ++progress->revision;
    }
/*
 * Provide the teacher lesson progress completion operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_lesson_progress_completion(const UmiTeacherLessonProgress *progress) { uint64_t done,total;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(progress==NULL) return 0U;
    done=(uint64_t)progress->sections_completed+progress->exercises_passed;
    total=(uint64_t)progress->section_count+progress->exercise_count;
    return umi_teacher_percent(done,total);
    }
/*
 * Provide the teacher lesson progress ready for assessment operation used by this module
 * and its client applications.
 */
int umi_teacher_lesson_progress_ready_for_assessment(const UmiTeacherLessonProgress *progress,uint32_t required_mastery) { return progress!=NULL && umi_teacher_lesson_progress_completion(progress)==100U && progress->mastery>=required_mastery;
    }
