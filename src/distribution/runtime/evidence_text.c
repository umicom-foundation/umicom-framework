/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/evidence_text.c
 *
 * PURPOSE:
 *   Parse a small explicit release-manifest grammar with transactional publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "evidence_internal.h"
#include <stdlib.h>
#include <string.h>

#define LINE_CAPACITY 2048U
#define FIELD_LIMIT 16U
static bool Token(const char *value)
{
    if (value[0] == '\0') return false;
    for (const unsigned char *p = (const unsigned char *)value; *p != 0U; ++p)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
            (*p >= '0' && *p <= '9') || *p == '.' || *p == '-' || *p == '_')) return false;
    return true;
}
static bool Copy(char *destination, size_t capacity, const char *source)
{
    size_t length = strlen(source);
    if (length == 0U || length >= capacity) return false;
    memcpy(destination, source, length + 1U);
    return true;
}
static bool Hex(const char *value, size_t length)
{
    if (strlen(value) != length) return false;
    for (size_t i = 0U; i < length; ++i)
        if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'f'))) return false;
    return true;
}
static bool Number(const char *text, uint64_t *out)
{
    uint64_t value = 0U;
    if (text[0] == '\0') return false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned digit = (unsigned)(unsigned char)text[i] - (unsigned)'0';
        if (digit > 9U || value > (UINT64_MAX - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    *out = value;
    return true;
}
static bool Kind(const char *text)
{ return strcmp(text,"native") == 0 || strcmp(text,"installed") == 0 ||
    strcmp(text,"analysis") == 0 || strcmp(text,"review") == 0; }
static bool Outcome(const char *text)
{ return strcmp(text,"passed") == 0 || strcmp(text,"failed") == 0 ||
    strcmp(text,"skipped") == 0 || strcmp(text,"not_run") == 0; }
static bool Reference(const char *text)
{
    if (strcmp(text,"-") == 0) return true;
    if (text[0] == '/' || text[0] == '\0') return false;
    const char *part = text;
    for (const char *p = text;; ++p) {
        if (*p == '/' || *p == '\0') {
            size_t n = (size_t)(p - part);
            if (n == 0U || (n == 1U && part[0] == '.') ||
                (n == 2U && part[0] == '.' && part[1] == '.')) return false;
            if (*p == '\0') break;
            part = p + 1;
        } else if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
            (*p >= '0' && *p <= '9') || *p == '.' || *p == '-' || *p == '_')) return false;
    }
    return true;
}
static UmiStatus TextValid(const char *text, size_t length)
{
    if (text == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_RELEASE_TEXT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == '\r') {
            if (i + 1U >= length || text[i + 1U] != '\n') return UMI_STATUS_PARSE_ERROR;
        } else if (c != '\n' && c != '\t' && (c < 32U || c > 126U)) return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
static UmiStatus Next(const char *text, size_t length, size_t *offset,
    char line[LINE_CAPACITY], char *fields[FIELD_LIMIT], size_t *count)
{
    size_t end = *offset;
    while (end < length && text[end] != '\n') ++end;
    size_t n = end - *offset;
    if (n > 0U && text[*offset + n - 1U] == '\r') --n;
    if (n >= LINE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(line, text + *offset, n); line[n] = '\0';
    *offset = end < length ? end + 1U : end;
    *count = 0U;
    if (n == 0U || line[0] == '#') return UMI_STATUS_OK;
    fields[(*count)++] = line;
    for (size_t i = 0U; i < n; ++i) if (line[i] == '\t') {
        if (*count == FIELD_LIMIT) return UMI_STATUS_PARSE_ERROR;
        line[i] = '\0'; fields[(*count)++] = line + i + 1U;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseContractParse(const char *text, size_t length, UmiReleaseContract **outContract)
{
    if (outContract == NULL || *outContract != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = TextValid(text, length);
    if (status != UMI_STATUS_OK) return status;
    UmiReleaseContract *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    unsigned stage = 0U, categories = 0U;
    size_t offset = 0U;
    while (offset < length) {
        char line[LINE_CAPACITY], *field[FIELD_LIMIT]; size_t count;
        status = Next(text,length,&offset,line,field,&count);
        if (status != UMI_STATUS_OK) break;
        if (count == 0U) continue;
        if (stage < 3U) {
            if (count != 2U) { status = UMI_STATUS_PARSE_ERROR; break; }
            if (stage == 0U && (strcmp(field[0],"UMICOM-RELEASE-CONTRACT") != 0 || strcmp(field[1],"1") != 0)) status = UMI_STATUS_PARSE_ERROR;
            if (stage == 1U && (strcmp(field[0],"candidate") != 0 || !Token(field[1]) || !Copy(candidate->candidate,sizeof(candidate->candidate),field[1]))) status = UMI_STATUS_PARSE_ERROR;
            if (stage == 2U && (strcmp(field[0],"source") != 0 || !Hex(field[1],40U) || !Copy(candidate->source,sizeof(candidate->source),field[1]))) status = UMI_STATUS_PARSE_ERROR;
            if (status != UMI_STATUS_OK) break;
            ++stage; continue;
        }
        if (count != 8U || strcmp(field[0],"require") != 0) { status = UMI_STATUS_PARSE_ERROR; break; }
        if (candidate->count == UMI_RELEASE_EVIDENCE_LIMIT) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        UmiReleaseRequirement *r = &candidate->rows[candidate->count];
        const char *names[] = {"signature","checksum","compatibility","tests","frontend","other"};
        size_t category = 0U;
        while (category < 6U && strcmp(field[2],names[category]) != 0) ++category;
        if (category == 6U || !Token(field[1]) || !Token(field[3]) || !Token(field[4]) || !Kind(field[5]) || !Token(field[6]) ||
            !Copy(r->id,sizeof(r->id),field[1]) || !Copy(r->profile,sizeof(r->profile),field[3]) ||
            !Copy(r->configuration,sizeof(r->configuration),field[4]) || !Copy(r->kind,sizeof(r->kind),field[5]) ||
            !Copy(r->ownerBatch,sizeof(r->ownerBatch),field[6]) || !Copy(r->title,sizeof(r->title),field[7])) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        r->category = (UmiReleaseEvidenceCategory)category;
        for (size_t i = 0U; i < candidate->count; ++i) if (strcmp(r->id,candidate->rows[i].id) == 0) status = UMI_STATUS_ALREADY_EXISTS;
        if (status != UMI_STATUS_OK) break;
        categories |= 1U << category;
        ++candidate->count;
    }
    if (status == UMI_STATUS_OK && (stage != 3U || (categories & 31U) != 31U)) status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    *outContract = candidate; return UMI_STATUS_OK;
}
UmiStatus UmiReleaseEvidenceParse(const char *text, size_t length, UmiReleaseEvidence **outEvidence)
{
    if (outEvidence == NULL || *outEvidence != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = TextValid(text,length);
    if (status != UMI_STATUS_OK) return status;
    UmiReleaseEvidence *candidate = calloc(1U,sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    bool header = false; size_t offset = 0U;
    while (offset < length) {
        char line[LINE_CAPACITY], *field[FIELD_LIMIT]; size_t count;
        status = Next(text,length,&offset,line,field,&count);
        if (status != UMI_STATUS_OK) break;
        if (count == 0U) continue;
        if (!header) {
            if (count != 2U || strcmp(field[0],"UMICOM-RELEASE-EVIDENCE") != 0 || strcmp(field[1],"1") != 0) { status = UMI_STATUS_PARSE_ERROR; break; }
            header = true; continue;
        }
        if (count != 16U || strcmp(field[0],"result") != 0) { status = UMI_STATUS_PARSE_ERROR; break; }
        if (candidate->count == UMI_RELEASE_EVIDENCE_LIMIT) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        UmiReleaseObservation *r = &candidate->rows[candidate->count];
        if (!Token(field[1]) || !Token(field[2]) || !Hex(field[3],40U) || !Token(field[4]) || !Token(field[5]) || !Kind(field[6]) || !Outcome(field[7]) ||
            !(strcmp(field[14],"-") == 0 || Hex(field[14],64U)) || !Reference(field[15]) ||
            !Copy(r->id,sizeof(r->id),field[1]) || !Copy(r->candidate,sizeof(r->candidate),field[2]) || !Copy(r->source,sizeof(r->source),field[3]) ||
            !Copy(r->profile,sizeof(r->profile),field[4]) || !Copy(r->configuration,sizeof(r->configuration),field[5]) || !Copy(r->kind,sizeof(r->kind),field[6]) ||
            !Copy(r->outcome,sizeof(r->outcome),field[7]) || !Number(field[8],&r->total) || !Number(field[9],&r->passed) || !Number(field[10],&r->failed) ||
            !Number(field[11],&r->skipped) || !Number(field[12],&r->notRun) || strcmp(field[13],"asserted") != 0 ||
            !Copy(r->digest,sizeof(r->digest),field[14]) || !Copy(r->reference,sizeof(r->reference),field[15])) { status = UMI_STATUS_PARSE_ERROR; break; }
        uint64_t sum = r->passed;
        const uint64_t rest[] = {r->failed,r->skipped,r->notRun};
        for (size_t i = 0U; i < 3U; ++i) {
            if (sum > UINT64_MAX - rest[i]) { status = UMI_STATUS_PARSE_ERROR; break; }
            sum += rest[i];
        }
        if (status != UMI_STATUS_OK || sum != r->total) { status = UMI_STATUS_PARSE_ERROR; break; }
        for (size_t i = 0U; i < candidate->count; ++i) if (strcmp(r->id,candidate->rows[i].id) == 0) status = UMI_STATUS_ALREADY_EXISTS;
        if (status != UMI_STATUS_OK) break;
        ++candidate->count;
    }
    if (status == UMI_STATUS_OK && !header) status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    *outEvidence = candidate; return UMI_STATUS_OK;
}
