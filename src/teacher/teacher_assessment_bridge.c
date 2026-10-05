/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/teacher_assessment_bridge.c
 *
 * PURPOSE:
 *   Bind Teacher assessments to existing Framework compiler/testing services.
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
#include "umicom/teacher/teacher_assessment_bridge.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Copy teacher teacher assessment bridge into module-owned storage so callers keep
 * ownership of their input values.
 */
static void umi_teacher_teacher_assessment_bridge_copy(char *destination, size_t capacity, const char *source) {
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
 * Initialise teacher teacher assessment bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_teacher_teacher_assessment_bridge_init(UmiTeacherTeacherAssessmentBridge *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->language = UMI_TEACHER_LANGUAGE_GENERAL;
    value->level = UMI_TEACHER_LEVEL_FOUNDATION;
    value->enabled = 1;
}
/*
 * Provide the teacher teacher assessment bridge configure operation used by this module
 * and its client applications.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_configure(UmiTeacherTeacherAssessmentBridge *value, const char *id, const char *title, UmiTeacherLanguage language, UmiTeacherLevel level, uint32_t weight, uint32_t required_score) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0' || required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_teacher_assessment_bridge_init(value);
    umi_teacher_teacher_assessment_bridge_copy(value->id, sizeof(value->id), id);
    umi_teacher_teacher_assessment_bridge_copy(value->title, sizeof(value->title), title);
    value->language = language;
    value->level = level;
    value->weight = weight;
    value->required_score = required_score;
    value->revision = 1U;
    return UMI_STATUS_OK;
}
/*
 * Check that teacher teacher assessment bridge satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_teacher_teacher_assessment_bridge_validate(const UmiTeacherTeacherAssessmentBridge *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->title, '\0', sizeof(value->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->id[0] == '\0' || value->required_score > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
/*
 * Provide the teacher teacher assessment bridge priority operation used by this module and
 * its client applications.
 */
uint32_t umi_teacher_teacher_assessment_bridge_priority(const UmiTeacherTeacherAssessmentBridge *value, uint32_t relevance) {
    uint32_t bonus;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    relevance = umi_teacher_clamp_score(relevance);
    bonus = value->weight > 25U ? 25U : value->weight;
    return umi_teacher_clamp_score(relevance + bonus);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTeacherTeacherAssessmentBridgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1525bb93334065d6);
    schema = (schema ^ (uint64_t)sizeof(((UmiTeacherTeacherAssessmentBridge *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTeacherTeacherAssessmentBridge *)0)->title)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTeacherTeacherAssessmentBridgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTeacherTeacherAssessmentBridge *)0)->id) - 1U +
        8U + sizeof(((UmiTeacherTeacherAssessmentBridge *)0)->title) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiTeacherTeacherAssessmentBridgeArchiveWrite(UmiArchiveWriter *writer, const UmiTeacherTeacherAssessmentBridge *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteSigned(writer, (int64_t)value->language);
    UmiArchiveWriteSigned(writer, (int64_t)value->level);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->weight);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_score);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
}
static void UmiTeacherTeacherAssessmentBridgeArchiveRead(UmiArchiveReader *reader, UmiTeacherTeacherAssessmentBridge *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    value->language = (UmiTeacherLanguage)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->level = (UmiTeacherLevel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->weight = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->required_score = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTeacherTeacherAssessmentBridgeArchiveValidate(const UmiTeacherTeacherAssessmentBridge *value)
{
    return umi_teacher_teacher_assessment_bridge_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_teacher_teacher_assessment_bridge_archive_encode, umi_teacher_teacher_assessment_bridge_archive_decode,
    UmiTeacherTeacherAssessmentBridge, UmiTeacherTeacherAssessmentBridgeArchiveSchema, UmiTeacherTeacherAssessmentBridgeArchiveBound, UmiTeacherTeacherAssessmentBridgeArchiveWrite, UmiTeacherTeacherAssessmentBridgeArchiveRead, UmiTeacherTeacherAssessmentBridgeArchiveValidate)
