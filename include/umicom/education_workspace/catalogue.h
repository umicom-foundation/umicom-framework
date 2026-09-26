/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education_workspace/catalogue.h
 * PURPOSE: Describe the shared, offline learning practicum.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_WORKSPACE_CATALOGUE_H
#define UMICOM_EDUCATION_WORKSPACE_CATALOGUE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/teacher/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDUCATION_LESSONS 12U
#define UMI_EDUCATION_QUESTIONS 3U
#define UMI_EDUCATION_CHOICES 3U
#define UMI_EDUCATION_CATALOGUE_REVISION 1U
/* Strings and arrays in catalogue records are immutable, process-lifetime
 * Framework storage. Applications borrow them; they must not free or edit them.
 * foundationId links a practicum to the existing foundations curriculum; this
 * catalogue adds worked activities, not a replacement for that curriculum. */
typedef struct UmiEducationQuestion {
    const char *prompt;
    const char *choices[UMI_EDUCATION_CHOICES];
    uint32_t correctChoice;
    const char *explanation;
} UmiEducationQuestion;
typedef struct UmiEducationLesson {
    const char *id;
    const char *courseId;
    const char *foundationId;
    const char *title;
    const char *explanation;
    const char *example;
    const char *practice;
    const char *hint;
    const char *projectId;
    UmiTeacherLanguage language;
    uint32_t minutes;
    UmiEducationQuestion questions[UMI_EDUCATION_QUESTIONS];
} UmiEducationLesson;
typedef struct UmiEducationCourse {
    const char *id;
    const char *title;
    const char *description;
    size_t firstLesson;
    size_t lessonCount;
} UmiEducationCourse;
size_t UmiEducationCourseCount(void);
const UmiEducationCourse *UmiEducationCourseAt(size_t index);
const UmiEducationLesson *UmiEducationLessonAt(size_t index);
const UmiEducationLesson *UmiEducationLessonFind(const char *id);
/* Verify IDs, ownership, ordering and all assessment references. No IO. */
UmiStatus UmiEducationCatalogueValidate(void);
#ifdef __cplusplus
}
#endif
#endif
