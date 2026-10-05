/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/problem.c
 *
 * PURPOSE:
 *   Validate normalized developer problems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/problem.h"
#include "../base/value_archive_internal.h"

/* Check that developer problem satisfies its contract before another service relies on it. */
UmiStatus umi_developer_problem_validate(
    const UmiDeveloperProblem *problem)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (problem == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(problem->source, '\0', sizeof(problem->source)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(problem->code, '\0', sizeof(problem->code)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(problem->message, '\0', sizeof(problem->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(problem->location.uri, '\0', sizeof(problem->location.uri)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (problem == NULL ||
        problem->source[0] == '\0' ||
        problem->message[0] == '\0' ||
        problem->severity < UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT ||
        problem->severity > UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (problem->location.uri[0] != '\0') {
        return umi_developer_productivity_location_validate(
            &problem->location);
    }

    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDeveloperProblemArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc7f1958451c2bece);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperProblem *)0)->source)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperProblem *)0)->code)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperProblem *)0)->message)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeveloperProblem *)0)->location.uri)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDeveloperProblemArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiDeveloperProblem *)0)->source) - 1U +
        8U + sizeof(((UmiDeveloperProblem *)0)->code) - 1U +
        8U + sizeof(((UmiDeveloperProblem *)0)->message) - 1U +
        8U +
        8U + sizeof(((UmiDeveloperProblem *)0)->location.uri) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDeveloperProblemArchiveWrite(UmiArchiveWriter *writer, const UmiDeveloperProblem *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->problem_id);
    UmiArchiveWriteText(writer, value->source, sizeof(value->source));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteText(writer, value->location.uri, sizeof(value->location.uri));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.column);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.end_line);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->location.end_column);
    UmiArchiveWriteSigned(writer, (int64_t)value->suppressible);
    UmiArchiveWriteSigned(writer, (int64_t)value->transient);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiDeveloperProblemArchiveRead(UmiArchiveReader *reader, UmiDeveloperProblem *value)
{
    value->problem_id = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->source, sizeof(value->source));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    value->severity = (UmiDeveloperProductivitySeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->location.uri, sizeof(value->location.uri));
    value->location.line = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->location.column = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->location.end_line = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->location.end_column = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->suppressible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->transient = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDeveloperProblemArchiveValidate(const UmiDeveloperProblem *value)
{
    return umi_developer_problem_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_developer_problem_archive_encode, umi_developer_problem_archive_decode,
    UmiDeveloperProblem, UmiDeveloperProblemArchiveSchema, UmiDeveloperProblemArchiveBound, UmiDeveloperProblemArchiveWrite, UmiDeveloperProblemArchiveRead, UmiDeveloperProblemArchiveValidate)
