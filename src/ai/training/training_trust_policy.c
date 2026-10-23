/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_trust_policy.c
 *
 * PURPOSE:
 *   Apply independently reviewed, bounded publisher allowlist and revocation
 *   policy before canonical native Ed25519 publication verification.
 *
 * ENGINEERING NOTES:
 *   No scripts, dynamic code, signature primitives, checkpoint decoders or
 *   network calls are present. Parse and verification are failure-atomic to
 *   caller outputs. No path component supplied by a policy may traverse its
 *   directory lexically; symlinks/permissions still require external review.
 *   This implementation retains a small fixed set of trusted keys and uses
 *   the existing attestation service to authenticate the selected PEM key.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_policy.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define UMI_TRUST_MAGIC "UMICOM-AI-TRUST-1"
#define UMI_TRUST_ENTRY_PREFIX "key="
#define UMI_TRUST_SIGNATURE_PREFIX \
    "UMICOM-AI-PUBLICATION-ED25519-1\npublic_key_sha256="
#define UMI_TRUST_SIGNATURE_MIDDLE "\nsignature="
#define UMI_TRUST_LINE_CAPACITY 512U

static bool IsLowerHex(char value)
{
    return (value >= '0' && value <= '9') ||
           (value >= 'a' && value <= 'f');
}

static bool IsValidFingerprint(const char *value)
{
    if (value == NULL || strlen(value) != UMI_SHA256_BYTES * 2U)
        return false;
    for (size_t i = 0U; i < UMI_SHA256_BYTES * 2U; ++i) {
        if (!IsLowerHex(value[i])) return false;
    }
    return true;
}

/* Relative paths may include a nested, ASCII-only key directory. Deliberately
 * disallow backslashes, drive prefixes, whitespace and traversal components. */
static bool IsValidPemPath(const char *path)
{
    size_t length;
    size_t start = 0U;
    if (path == NULL) return false;
    length = strlen(path);
    if (length == 0U || length >= UMI_AI_TRAINING_TRUST_MAX_RELATIVE_KEY_PATH)
        return false;
    for (size_t i = 0U; i <= length; ++i) {
        unsigned char ch = (unsigned char)path[i];
        if (ch == '/' || ch == '\0') {
            size_t count = i - start;
            if (count == 0U ||
                (count == 1U && path[start] == '.') ||
                (count == 2U && path[start] == '.' &&
                 path[start + 1U] == '.')) return false;
            start = i + 1U;
        } else if (!((ch >= 'a' && ch <= 'z') ||
                     (ch >= 'A' && ch <= 'Z') ||
                     (ch >= '0' && ch <= '9') ||
                     ch == '-' || ch == '_' || ch == '.')) {
            return false;
        }
    }
    return true;
}

/* Reject signs, fractions, exponent syntax and overflow. Time is an explicit
 * scalar input so callers can audit exactly which instant was evaluated. */
static bool ParseUnixSeconds(const char *value, uint64_t *outSeconds)
{
    uint64_t number = 0U;
    if (value == NULL || outSeconds == NULL || value[0] == '\0') return false;
    if (value[0] == '0' && value[1] != '\0') return false;
    for (const char *cursor = value; *cursor != '\0'; ++cursor) {
        uint64_t digit;
        if (*cursor < '0' || *cursor > '9') return false;
        digit = (uint64_t)(unsigned)(*cursor - '0');
        if (number > (UINT64_MAX - digit) / UINT64_C(10)) return false;
        number = number * UINT64_C(10) + digit;
    }
    /* Keep accepted intervals in signed 64-bit time for common C runtimes. */
    if (number > (uint64_t)INT64_MAX) return false;
    *outSeconds = number;
    return true;
}

static UmiStatus ParseEntry(char *line,
                            UmiAiTrainingTrustPolicy *candidate)
{
    UmiAiTrainingTrustKey entry = {0};
    char *fields[5];
    char *cursor = line;
    if (strncmp(line, UMI_TRUST_ENTRY_PREFIX,
                sizeof(UMI_TRUST_ENTRY_PREFIX) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    cursor += sizeof(UMI_TRUST_ENTRY_PREFIX) - 1U;
    for (size_t i = 0U; i < 5U; ++i) {
        char *comma;
        fields[i] = cursor;
        comma = strchr(cursor, ',');
        if (i < 4U) {
            if (comma == NULL || comma == cursor) return UMI_STATUS_PARSE_ERROR;
            *comma = '\0';
            cursor = comma + 1U;
        } else if (comma != NULL || cursor[0] == '\0') {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    if (!IsValidFingerprint(fields[0]) || !IsValidPemPath(fields[1]) ||
        !ParseUnixSeconds(fields[2], &entry.validFromUnixSeconds) ||
        !ParseUnixSeconds(fields[3], &entry.validUntilUnixSeconds) ||
        entry.validUntilUnixSeconds <= entry.validFromUnixSeconds)
        return UMI_STATUS_PARSE_ERROR;
    if (strcmp(fields[4], "active") == 0)
        entry.state = UMI_AI_TRAINING_TRUST_KEY_ACTIVE;
    else if (strcmp(fields[4], "revoked") == 0)
        entry.state = UMI_AI_TRAINING_TRUST_KEY_REVOKED;
    else
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < candidate->keyCount; ++i) {
        if (strcmp(candidate->keys[i].publicKeySha256, fields[0]) == 0)
            return UMI_STATUS_PARSE_ERROR;
    }
    if (candidate->keyCount >= UMI_AI_TRAINING_TRUST_MAX_KEYS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(entry.publicKeySha256, fields[0], strlen(fields[0]) + 1U);
    memcpy(entry.relativePemPath, fields[1], strlen(fields[1]) + 1U);
    candidate->keys[candidate->keyCount++] = entry;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTrustPolicyParse(
    const char *text, size_t textLength,
    UmiAiTrainingTrustPolicy *outPolicy)
{
    UmiAiTrainingTrustPolicy candidate = {0};
    size_t at = 0U;
    bool sawHeader = false;
    if (text == NULL || outPolicy == NULL || textLength == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (textLength > UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', textLength) != NULL) return UMI_STATUS_PARSE_ERROR;
    while (at < textLength) {
        char line[UMI_TRUST_LINE_CAPACITY];
        size_t start = at;
        size_t count;
        UmiStatus status;
        while (at < textLength && text[at] != '\n') ++at;
        count = at - start;
        if (at < textLength) ++at;
        if (count != 0U && text[start + count - 1U] == '\r') --count;
        if (count == 0U) continue;
        if (count >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(line, text + start, count);
        line[count] = '\0';
        if (line[0] == '#') continue;
        if (!sawHeader) {
            if (strcmp(line, UMI_TRUST_MAGIC) != 0)
                return UMI_STATUS_PARSE_ERROR;
            sawHeader = true;
            continue;
        }
        status = ParseEntry(line, &candidate);
        if (status != UMI_STATUS_OK) return status;
    }
    if (!sawHeader || candidate.keyCount == 0U)
        return UMI_STATUS_PARSE_ERROR;
    *outPolicy = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTrustPolicyLoadFile(
    const char *policyFile, UmiAiTrainingTrustPolicy *outPolicy)
{
    char content[UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES + 1U];
    size_t length;
    int extra, readFailure;
    FILE *stream;
    if (policyFile == NULL || policyFile[0] == '\0' || outPolicy == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(policyFile, "rb");
    if (stream == NULL) return UMI_STATUS_IO_ERROR;
    length = fread(content, 1U, sizeof(content), stream);
    extra = fgetc(stream);
    readFailure = ferror(stream);
    if (fclose(stream) != 0) readFailure = 1;
    if (readFailure != 0) return UMI_STATUS_IO_ERROR;
    if (length > UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES || extra != EOF)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiAiTrainingTrustPolicyParse(content, length, outPolicy);
}

/* The policy is selected independently; the signature record contributes
 * only an identifier used to choose an already-authorised public key. The
 * existing attestation verifier validates its entire syntax and signature. */
static UmiStatus ReadClaimedFingerprint(
    const char *checkpointPath,
    char outFingerprint[UMI_SHA256_HEX_CAPACITY])
{
    const char prefix[] = UMI_TRUST_SIGNATURE_PREFIX;
    const size_t prefixLength = sizeof(prefix) - 1U;
    const size_t fingerprintLength = UMI_SHA256_BYTES * 2U;
    unsigned char bytes[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES + 1U];
    char path[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    size_t pathLength, count;
    FILE *file;
    int extra, failed;
    if (checkpointPath == NULL || outFingerprint == NULL ||
        checkpointPath[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    pathLength = strlen(checkpointPath);
    if (pathLength >= sizeof(path) ||
        sizeof(UMI_AI_TRAINING_ATTESTATION_SUFFIX) > sizeof(path) - pathLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(path, checkpointPath, pathLength);
    memcpy(path + pathLength, UMI_AI_TRAINING_ATTESTATION_SUFFIX,
           sizeof(UMI_AI_TRAINING_ATTESTATION_SUFFIX));
    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    count = fread(bytes, 1U, sizeof(bytes), file);
    extra = fgetc(file);
    failed = ferror(file);
    if (fclose(file) != 0) failed = 1;
    if (failed != 0) return UMI_STATUS_IO_ERROR;
    if (count != UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES || extra != EOF ||
        memcmp(bytes, prefix, prefixLength) != 0 ||
        prefixLength + fingerprintLength +
        sizeof(UMI_TRUST_SIGNATURE_MIDDLE) - 1U > count)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < fingerprintLength; ++i) {
        if (!IsLowerHex((char)bytes[prefixLength + i]))
            return UMI_STATUS_PARSE_ERROR;
        outFingerprint[i] = (char)bytes[prefixLength + i];
    }
    outFingerprint[fingerprintLength] = '\0';
    if (memcmp(bytes + prefixLength + fingerprintLength,
               UMI_TRUST_SIGNATURE_MIDDLE,
               sizeof(UMI_TRUST_SIGNATURE_MIDDLE) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

static UmiStatus JoinPolicyDirectory(
    const char *policyFile, const char *relative,
    char outPath[UMI_AI_TRAINING_PUBLICATION_MAX_PATH])
{
    size_t prefixLength = 0U, relativeLength;
    const char *separator = NULL;
    if (policyFile == NULL || relative == NULL || outPath == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (const char *cursor = policyFile; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') separator = cursor;
    }
    if (separator != NULL)
        prefixLength = (size_t)(separator - policyFile) + 1U;
    relativeLength = strlen(relative);
    if (prefixLength >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH ||
        relativeLength >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH - prefixLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outPath, policyFile, prefixLength);
    memcpy(outPath + prefixLength, relative, relativeLength + 1U);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAttestationVerifyWithTrustPolicy(
    const char *checkpointPath, const char *policyFile,
    uint64_t verificationUnixSeconds,
    UmiAiTrainingTrustDecision *outDecision)
{
    UmiAiTrainingTrustPolicy policy;
    UmiAiTrainingPublicationEvidence verified;
    UmiAiTrainingTrustDecision candidate = {0};
    char claimedFingerprint[UMI_SHA256_HEX_CAPACITY];
    char resolvedPem[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    UmiStatus status;
    if (checkpointPath == NULL || checkpointPath[0] == '\0' ||
        policyFile == NULL || policyFile[0] == '\0' || outDecision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTrustPolicyLoadFile(policyFile, &policy);
    if (status != UMI_STATUS_OK) return status;
    status = ReadClaimedFingerprint(checkpointPath, claimedFingerprint);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < policy.keyCount; ++i) {
        const UmiAiTrainingTrustKey *selected = &policy.keys[i];
        if (strcmp(selected->publicKeySha256, claimedFingerprint) != 0)
            continue;
        if (selected->state != UMI_AI_TRAINING_TRUST_KEY_ACTIVE ||
            verificationUnixSeconds < selected->validFromUnixSeconds ||
            verificationUnixSeconds >= selected->validUntilUnixSeconds)
            return UMI_STATUS_PERMISSION_DENIED;
        status = JoinPolicyDirectory(policyFile, selected->relativePemPath,
                                     resolvedPem);
        if (status != UMI_STATUS_OK) return status;
        /* The canonical verifier checks the claimed fingerprint against the
         * actual PEM key, the Ed25519 signature, and ALL four file digests. */
        status = UmiAiTrainingAttestationVerifyAndInspectFile(
            checkpointPath, resolvedPem, &verified);
        if (status != UMI_STATUS_OK) return status;
        memcpy(candidate.verifiedPublicKeySha256, claimedFingerprint,
               sizeof(candidate.verifiedPublicKeySha256));
        candidate.evaluatedAtUnixSeconds = verificationUnixSeconds;
        candidate.publication = verified;
        *outDecision = candidate;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PERMISSION_DENIED;
}
