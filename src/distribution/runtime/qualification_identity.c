/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_identity.c
 * Purpose: Encode selection and source identity without ambiguous separators.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include <string.h>

bool UmiQualificationDigestValid(const char *text)
{
    if (text == NULL) return false;
    for (size_t i = 0U; i < 64U; ++i) {
        char c = text[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    }
    return text[64] == '\0';
}
static UmiStatus Number(UmiSha256 *hash, uint64_t number)
{
    unsigned char bytes[8];
    for (size_t i = 0U; i < sizeof(bytes); ++i) {
        bytes[sizeof(bytes) - i - 1U] = (unsigned char)(number & 255U);
        number >>= 8U;
    }
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}
static UmiStatus Field(UmiSha256 *hash, const char *text)
{
    size_t length = strlen(text);
    UmiStatus status = Number(hash, (uint64_t)length);
    return status == UMI_STATUS_OK ? UmiSha256Update(hash, text, length) : status;
}
UmiStatus UmiQualificationPlanFingerprint(UmiReleaseQualification *run)
{
    UmiSha256 hash;
    UmiSha256Init(&hash);
    const char *fields[] = {"umicom-qualification-plan-1", run->inventory, run->source, run->toolchain};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(fields) / sizeof(fields[0]); ++i)
        status = Field(&hash, fields[i]);
    uint64_t numbers[] = {(uint64_t)run->selection.changedPaths, (uint64_t)run->selection.targets,
        (uint64_t)run->selection.tests, run->selection.fallbackReasons,
        run->selection.noWork ? 1U : 0U, run->selection.missingTestCoverage ? 1U : 0U,
        (uint64_t)run->count};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(numbers) / sizeof(numbers[0]); ++i)
        status = Number(&hash, numbers[i]);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < run->count; ++i) {
        status = Number(&hash, (uint64_t)run->items[i].kind);
        if (status == UMI_STATUS_OK) status = Field(&hash, run->items[i].name);
        if (status == UMI_STATUS_OK) status = Number(&hash, run->items[i].reasons);
    }
    unsigned char digest[32];
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, digest);
    if (status == UMI_STATUS_OK) UmiSha256Hex(digest, run->identity.plan);
    return status;
}
UmiStatus UmiQualificationMatch(const UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity)
{
    if (run == NULL || identity == NULL ||
        !UmiQualificationDigestValid(identity->execution) ||
        !UmiQualificationDigestValid(identity->plan)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(run->identity.execution, identity->execution) != 0 ||
        strcmp(run->identity.plan, identity->plan) != 0) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
