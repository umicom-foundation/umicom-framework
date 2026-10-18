/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/lesson_resource.h
 *
 * PURPOSE:
 *   Describe a resource linked to a lesson without tying the model to a UI toolkit.
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
#ifndef UMICOM_TEACHER_LESSON_RESOURCE_H
#define UMICOM_TEACHER_LESSON_RESOURCE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/teacher/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the teacher lesson resource data shared with callers of this public contract.
 */
typedef struct UmiTeacherLessonResource {
    char id[UMI_TEACHER_ID_CAPACITY];
    char title[UMI_TEACHER_TEXT_CAPACITY];
    UmiTeacherLanguage language;
    UmiTeacherLevel level;
    uint32_t weight;
    uint32_t required_score;
    uint64_t revision;
    int enabled;
} UmiTeacherLessonResource;

/**
 * Initialise teacher lesson resource from caller-provided values so later operations
 * receive a known state.
 */
void umi_teacher_lesson_resource_init(UmiTeacherLessonResource *value);
/**
 * Provide the teacher lesson resource configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_lesson_resource_configure(UmiTeacherLessonResource *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score);
/**
 * Check that teacher lesson resource satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_teacher_lesson_resource_validate(const UmiTeacherLessonResource *value);
/**
 * Provide the teacher lesson resource priority operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_lesson_resource_priority(const UmiTeacherLessonResource *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_teacher_lesson_resource_archive_encode(const UmiTeacherLessonResource *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_teacher_lesson_resource_archive_decode(const void *bytes, size_t byte_count,
    UmiTeacherLessonResource *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTeacherLessonResource object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_teacher_lesson_resource_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTeacherLessonResource *workspace, UmiTeacherLessonResource *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_teacher_lesson_resource_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTeacherLessonResource *expected, UmiTeacherLessonResource *value);


#ifdef __cplusplus
}
#endif

#endif
