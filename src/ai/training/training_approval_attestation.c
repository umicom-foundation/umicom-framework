/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_approval_attestation.c
 *
 * PURPOSE:
 *   Ed25519 authorization of a previously approved ordered native LLM
 *   release identity. The publisher trust policy is verified separately.
 *
 * ARCHITECTURE AND SECURITY:
 *   Reuse canonical release-approval parsing, ordered batch verification and
 *   Framework SHA-256. OpenSSL 3 provides Ed25519; no independent signature
 *   algorithm is implemented here. The EXACT reviewed approval bytes are
 *   domain-separated from ordinary model-publication signatures. The detached
 *   signature alone cannot select or authorize its trust anchor. This does
 *   not establish a trusted timestamp, filesystem snapshot, or secure host.
 *   All pathname creation is bounded; the caller approves external files.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_approval_attestation.h"
#include "umicom/ai/training/training_attestation.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#define UMI_APPROVAL_SIGNATURE_DOMAIN "UMICOM-AI-RELEASE-APPROVAL-ED25519-1\n"
#define UMI_APPROVAL_SIGNATURE_HEADER "UMICOM-AI-RELEASE-APPROVAL-SIGNATURE-1\n"
#define UMI_APPROVAL_SIGNATURE_KEY_LABEL "public_key_sha256="
#define UMI_APPROVAL_SIGNATURE_VALUE_LABEL "signature="
#define UMI_APPROVAL_SIGNATURE_BYTES 64U
#define UMI_APPROVAL_KEY_BYTES 32U
#define UMI_APPROVAL_MAX_PEM_BYTES 16384U
#define UMI_APPROVAL_MAX_PATH 4096U

/* Fixed record grammar; no JSON or certificate parser is involved. */
#define UMI_APPROVAL_SIGNATURE_RECORD_BYTES \
    (sizeof(UMI_APPROVAL_SIGNATURE_HEADER) - 1U + \
     sizeof(UMI_APPROVAL_SIGNATURE_KEY_LABEL) - 1U + 64U + 1U + \
     sizeof(UMI_APPROVAL_SIGNATURE_VALUE_LABEL) - 1U + 128U + 1U)

static UmiStatus MakeApprovalPath(const char *first, const char *suffix,
                                   char out[UMI_APPROVAL_MAX_PATH])
{
    size_t firstLength, suffixLength;
    if (first == NULL || first[0] == '\0' || suffix == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    firstLength = strlen(first);
    suffixLength = strlen(suffix);
    if (firstLength >= UMI_APPROVAL_MAX_PATH ||
        suffixLength >= UMI_APPROVAL_MAX_PATH - firstLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, first, firstLength);
    memcpy(out + firstLength, suffix, suffixLength + 1U);
    return UMI_STATUS_OK;
}

/* One exact bounded read is shared by parsing and signing/verification;
 * there is no independent, potentially inconsistent second plan parse. */
static UmiStatus ReadApprovalBytes(const char *path, unsigned char *out,
                                   size_t capacity, size_t *outLength)
{
    FILE *file;
    size_t count;
    int extra, hadError;
    if (path == NULL || path[0] == '\0' || out == NULL ||
        outLength == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    count = fread(out, 1U, capacity, file);
    extra = fgetc(file);
    hadError = ferror(file);
    if (fclose(file) != 0) hadError = 1;
    if (hadError != 0) return UMI_STATUS_IO_ERROR;
    if (count == 0U) return UMI_STATUS_PARSE_ERROR;
    if (extra != EOF) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outLength = count;
    return UMI_STATUS_OK;
}

/* Accept precisely one native Ed25519 key; securely clear the temporary PEM
 * copy even on parse failure. The caller controls filesystem permissions. */
static UmiStatus ReadApprovalPem(const char *path, int privateKey,
                                 EVP_PKEY **outKey)
{
    unsigned char pem[UMI_APPROVAL_MAX_PEM_BYTES + 1U];
    size_t count = 0U;
    BIO *bio = NULL;
    EVP_PKEY *key = NULL;
    UmiStatus status;
    if (outKey == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReadApprovalBytes(path, pem, sizeof(pem), &count);
    if (status != UMI_STATUS_OK) {
        OPENSSL_cleanse(pem, sizeof(pem));
        return status;
    }
    if (count > UMI_APPROVAL_MAX_PEM_BYTES) {
        OPENSSL_cleanse(pem, sizeof(pem));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    bio = BIO_new_mem_buf(pem, (int)count);
    if (bio != NULL) {
        if (privateKey != 0)
            key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
        else
            key = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        BIO_free(bio);
    }
    OPENSSL_cleanse(pem, sizeof(pem));
    if (bio == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (key == NULL) return UMI_STATUS_PARSE_ERROR;
    if (EVP_PKEY_base_id(key) != EVP_PKEY_ED25519) {
        EVP_PKEY_free(key);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outKey = key;
    return UMI_STATUS_OK;
}

static UmiStatus ApprovalKeyFingerprint(EVP_PKEY *key,
                                        char out[UMI_SHA256_HEX_CAPACITY])
{
    unsigned char publicBytes[UMI_APPROVAL_KEY_BYTES];
    size_t byteCount = sizeof(publicBytes);
    UmiStatus status;
    if (key == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (EVP_PKEY_get_raw_public_key(key, publicBytes, &byteCount) != 1 ||
        byteCount != sizeof(publicBytes))
        return UMI_STATUS_PARSE_ERROR;
    status = UmiSha256Buffer(publicBytes, sizeof(publicBytes), out);
    OPENSSL_cleanse(publicBytes, sizeof(publicBytes));
    return status;
}

static void EncodeHex(const unsigned char *data, size_t count, char *out)
{
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0U; i < count; ++i) {
        out[i * 2U] = digits[data[i] >> 4U];
        out[i * 2U + 1U] = digits[data[i] & 15U];
    }
    out[count * 2U] = '\0';
}

static int HexNibble(unsigned char value)
{
    if (value >= (unsigned char)'0' && value <= (unsigned char)'9')
        return (int)(value - (unsigned char)'0');
    if (value >= (unsigned char)'a' && value <= (unsigned char)'f')
        return 10 + (int)(value - (unsigned char)'a');
    return -1;
}

static UmiStatus DecodeApprovalSignature(const unsigned char *bytes,
                                         size_t byteCount,
                                         char signer[UMI_SHA256_HEX_CAPACITY],
                                         unsigned char signature[UMI_APPROVAL_SIGNATURE_BYTES])
{
    const char prefix[] = UMI_APPROVAL_SIGNATURE_HEADER UMI_APPROVAL_SIGNATURE_KEY_LABEL;
    const char middle[] = "\n" UMI_APPROVAL_SIGNATURE_VALUE_LABEL;
    size_t at = 0U;
    if (bytes == NULL || signer == NULL || signature == NULL ||
        byteCount != UMI_APPROVAL_SIGNATURE_RECORD_BYTES ||
        memcmp(bytes, prefix, sizeof(prefix) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    at += sizeof(prefix) - 1U;
    for (size_t i = 0U; i < 64U; ++i) {
        if (HexNibble(bytes[at + i]) < 0) return UMI_STATUS_PARSE_ERROR;
        signer[i] = (char)bytes[at + i];
    }
    signer[64] = '\0';
    at += 64U;
    if (memcmp(bytes + at, middle, sizeof(middle) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    at += sizeof(middle) - 1U;
    for (size_t i = 0U; i < UMI_APPROVAL_SIGNATURE_BYTES; ++i) {
        int high = HexNibble(bytes[at + i * 2U]);
        int low = HexNibble(bytes[at + i * 2U + 1U]);
        if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
        signature[i] = (unsigned char)((high << 4) | low);
    }
    at += UMI_APPROVAL_SIGNATURE_BYTES * 2U;
    if (at + 1U != byteCount || bytes[at] != (unsigned char)'\n')
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

static UmiStatus BuildApprovalMessage(const unsigned char *approval,
                                      size_t byteCount,
                                      unsigned char *message,
                                      size_t messageCapacity,
                                      size_t *outCount)
{
    const size_t prefixLength = sizeof(UMI_APPROVAL_SIGNATURE_DOMAIN) - 1U;
    if (approval == NULL || byteCount == 0U || message == NULL ||
        outCount == NULL || prefixLength > messageCapacity ||
        byteCount > messageCapacity - prefixLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(message, UMI_APPROVAL_SIGNATURE_DOMAIN, prefixLength);
    memcpy(message + prefixLength, approval, byteCount);
    *outCount = prefixLength + byteCount;
    return UMI_STATUS_OK;
}

#if defined(_WIN32)
static int ApprovalCloseHandle(int handle) { return _close(handle); }
static int ApprovalSyncHandle(int handle) { return _commit(handle); }
#else
static int ApprovalCloseHandle(int handle) { return close(handle); }
static int ApprovalSyncHandle(int handle) { return fsync(handle); }
#endif

/* Same-directory staging + O_EXCL publication, following existing Framework
 * attestation practices. The final signature path is never overwritten. */
static UmiStatus SaveApprovalSignatureNew(
    const char *approvalPath,
    const unsigned char bytes[UMI_APPROVAL_SIGNATURE_RECORD_BYTES])
{
    char finalPath[UMI_APPROVAL_MAX_PATH];
    char temporaryPath[UMI_APPROVAL_MAX_PATH];
    size_t offset = 0U;
    int descriptor;
    UmiStatus status;
    status = MakeApprovalPath(approvalPath,
                              UMI_AI_TRAINING_APPROVAL_SIGNATURE_SUFFIX,
                              finalPath);
    if (status != UMI_STATUS_OK) return status;
    status = MakeApprovalPath(approvalPath,
                              UMI_AI_TRAINING_APPROVAL_SIGNATURE_SUFFIX ".partial",
                              temporaryPath);
    if (status != UMI_STATUS_OK) return status;
#if defined(_WIN32)
    descriptor = _open(temporaryPath,
                       _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(temporaryPath, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0)
        return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    while (offset < UMI_APPROVAL_SIGNATURE_RECORD_BYTES) {
#if defined(_WIN32)
        int count = _write(descriptor, bytes + offset,
                           (unsigned int)(UMI_APPROVAL_SIGNATURE_RECORD_BYTES - offset));
#else
        ssize_t count = write(descriptor, bytes + offset,
                              UMI_APPROVAL_SIGNATURE_RECORD_BYTES - offset);
#endif
        if (count <= 0) { status = UMI_STATUS_IO_ERROR; goto failure; }
        offset += (size_t)count;
    }
    {
        int syncStatus = ApprovalSyncHandle(descriptor);
        int closeStatus = ApprovalCloseHandle(descriptor);
        descriptor = -1;
        if (syncStatus != 0 || closeStatus != 0) {
            status = UMI_STATUS_IO_ERROR;
            goto failure;
        }
    }
#if defined(_WIN32)
    if (!MoveFileExA(temporaryPath, finalPath, MOVEFILE_WRITE_THROUGH)) {
        DWORD error = GetLastError();
        status = (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS) ?
                 UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto failure;
    }
#else
    if (link(temporaryPath, finalPath) != 0) {
        status = errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto failure;
    }
    (void)remove(temporaryPath);
#endif
    return UMI_STATUS_OK;
failure:
    if (descriptor >= 0) (void)ApprovalCloseHandle(descriptor);
    (void)remove(temporaryPath);
    return status;
}

UmiStatus UmiAiTrainingApprovalAttestationSignNewFile(
    const char *approvalPath, const char *privateEd25519PemPath)
{
    EVP_PKEY *key = NULL;
    EVP_MD_CTX *context = NULL;
    UmiAiTrainingReleaseApproval parsed;
    unsigned char approval[UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES + 1U];
    unsigned char message[sizeof(UMI_APPROVAL_SIGNATURE_DOMAIN) - 1U +
                          UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES];
    unsigned char signature[UMI_APPROVAL_SIGNATURE_BYTES];
    unsigned char record[UMI_APPROVAL_SIGNATURE_RECORD_BYTES];
    char formatted[UMI_APPROVAL_SIGNATURE_RECORD_BYTES + 1U];
    char fingerprint[UMI_SHA256_HEX_CAPACITY];
    char signatureHex[UMI_APPROVAL_SIGNATURE_BYTES * 2U + 1U];
    size_t approvalLength = 0U, messageLength = 0U;
    size_t signatureLength = sizeof(signature);
    UmiStatus status;
    int written;
    if (approvalPath == NULL || privateEd25519PemPath == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ReadApprovalBytes(approvalPath, approval, sizeof(approval),
                               &approvalLength);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingReleaseApprovalParse(
        (const char *)approval, approvalLength, &parsed);
    if (status != UMI_STATUS_OK) return status;
    status = BuildApprovalMessage(approval, approvalLength, message,
                                  sizeof(message), &messageLength);
    if (status != UMI_STATUS_OK) return status;
    status = ReadApprovalPem(privateEd25519PemPath, 1, &key);
    if (status != UMI_STATUS_OK) return status;
    status = ApprovalKeyFingerprint(key, fingerprint);
    if (status != UMI_STATUS_OK) goto cleanup;
    context = EVP_MD_CTX_new();
    if (context == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    if (EVP_DigestSignInit(context, NULL, NULL, NULL, key) != 1 ||
        EVP_DigestSign(context, signature, &signatureLength,
                       message, messageLength) != 1 ||
        signatureLength != UMI_APPROVAL_SIGNATURE_BYTES) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    EncodeHex(signature, UMI_APPROVAL_SIGNATURE_BYTES, signatureHex);
    written = snprintf(formatted, sizeof(formatted),
                       UMI_APPROVAL_SIGNATURE_HEADER
                       UMI_APPROVAL_SIGNATURE_KEY_LABEL "%s\n"
                       UMI_APPROVAL_SIGNATURE_VALUE_LABEL "%s\n",
                       fingerprint, signatureHex);
    if (written != (int)sizeof(record)) {
        status = UMI_STATUS_INTERNAL_ERROR;
        goto cleanup;
    }
    memcpy(record, formatted, sizeof(record));
    status = SaveApprovalSignatureNew(approvalPath, record);
cleanup:
    EVP_MD_CTX_free(context);
    EVP_PKEY_free(key);
    OPENSSL_cleanse(signature, sizeof(signature));
    OPENSSL_cleanse(signatureHex, sizeof(signatureHex));
    return status;
}

UmiStatus UmiAiTrainingApprovalAttestationVerifyFile(
    const char *approvalPath, const char *trustedApprovalPublicKeyPemPath,
    UmiAiTrainingReleaseApproval *outApproval,
    char outSignerSha256[UMI_SHA256_HEX_CAPACITY])
{
    EVP_PKEY *key = NULL;
    EVP_MD_CTX *context = NULL;
    UmiAiTrainingReleaseApproval parsed;
    unsigned char approval[UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES + 1U];
    unsigned char message[sizeof(UMI_APPROVAL_SIGNATURE_DOMAIN) - 1U +
                          UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES];
    unsigned char record[UMI_APPROVAL_SIGNATURE_RECORD_BYTES + 1U];
    unsigned char signature[UMI_APPROVAL_SIGNATURE_BYTES];
    char claimed[UMI_SHA256_HEX_CAPACITY];
    char actual[UMI_SHA256_HEX_CAPACITY];
    char signaturePath[UMI_APPROVAL_MAX_PATH];
    size_t approvalLength = 0U, messageLength = 0U, recordLength = 0U;
    UmiStatus status;
    if (approvalPath == NULL || trustedApprovalPublicKeyPemPath == NULL ||
        outApproval == NULL || outSignerSha256 == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = MakeApprovalPath(approvalPath,
                              UMI_AI_TRAINING_APPROVAL_SIGNATURE_SUFFIX,
                              signaturePath);
    if (status != UMI_STATUS_OK) return status;
    status = ReadApprovalBytes(signaturePath, record, sizeof(record),
                               &recordLength);
    if (status != UMI_STATUS_OK) return status;
    status = DecodeApprovalSignature(record, recordLength, claimed, signature);
    if (status != UMI_STATUS_OK) return status;
    status = ReadApprovalBytes(approvalPath, approval, sizeof(approval),
                               &approvalLength);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingReleaseApprovalParse(
        (const char *)approval, approvalLength, &parsed);
    if (status != UMI_STATUS_OK) return status;
    status = BuildApprovalMessage(approval, approvalLength, message,
                                  sizeof(message), &messageLength);
    if (status != UMI_STATUS_OK) return status;
    status = ReadApprovalPem(trustedApprovalPublicKeyPemPath, 0, &key);
    if (status != UMI_STATUS_OK) return status;
    status = ApprovalKeyFingerprint(key, actual);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (strcmp(actual, claimed) != 0) {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto cleanup;
    }
    context = EVP_MD_CTX_new();
    if (context == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
    if (EVP_DigestVerifyInit(context, NULL, NULL, NULL, key) != 1 ||
        EVP_DigestVerify(context, signature, sizeof(signature),
                         message, messageLength) != 1) {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto cleanup;
    }
    /* Publishing outputs is one logical commit after all parsing and
     * cryptographic checks have passed. */
    *outApproval = parsed;
    memcpy(outSignerSha256, actual, sizeof(actual));
    status = UMI_STATUS_OK;
cleanup:
    EVP_MD_CTX_free(context);
    EVP_PKEY_free(key);
    return status;
}

UmiStatus UmiAiTrainingSignedReleaseApprovalVerifyJson(
    const char *const *checkpointPaths,
    size_t checkpointCount,
    const char *trustedPublisherPolicyFile,
    const char *approvalPath,
    const char *trustedApprovalPublicKeyPemPath,
    char *outJson, size_t outputCapacity, size_t *outWritten)
{
    static const char ORIGINAL[] = "\"approval_record_signed\":false";
    static const char SIGNED[] = "\"approval_record_signed\":true";
    static const char FOLLOWING[] = ",\"approval_signature\":{\"algorithm\":\"Ed25519\"," 
        "\"verification_key_sha256\":\"";
    static const char END[] = "\",\"independently_trusted_key\":true}}";
    UmiAiTrainingReleaseApproval approval;
    char signer[UMI_SHA256_HEX_CAPACITY];
    char *unsignedJson = NULL;
    char *candidate = NULL;
    char *field;
    size_t unsignedLength = 0U, written = 0U;
    UmiStatus status;
    if (checkpointPaths == NULL || trustedPublisherPolicyFile == NULL ||
        approvalPath == NULL || trustedApprovalPublicKeyPemPath == NULL ||
        outJson == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingApprovalAttestationVerifyFile(
        approvalPath, trustedApprovalPublicKeyPemPath, &approval, signer);
    if (status != UMI_STATUS_OK) return status;
    unsignedJson = malloc(UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY);
    candidate = malloc(UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY);
    if (unsignedJson == NULL || candidate == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }
    status = UmiAiTrainingReleaseApprovalVerifyJson(
        checkpointPaths, checkpointCount, trustedPublisherPolicyFile,
        &approval, unsignedJson, UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY,
        &unsignedLength);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (unsignedLength < 2U || unsignedLength >=
        UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY ||
        unsignedJson[unsignedLength] != '\0' ||
        unsignedJson[unsignedLength - 1U] != '}' ||
        strstr(unsignedJson, ORIGINAL) == NULL ||
        strstr(strstr(unsignedJson, ORIGINAL) + 1U, ORIGINAL) != NULL) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    field = strstr(unsignedJson, ORIGINAL);
    {
        const size_t start = (size_t)(field - unsignedJson);
        const size_t firstLength = sizeof(ORIGINAL) - 1U;
        const size_t newLength = sizeof(SIGNED) - 1U;
        const size_t frontLength = start + newLength;
        const size_t tailLength = unsignedLength - start - firstLength;
        /* Retain BOTH original object-closing braces here: the one for
         * release_approval and the outer one. Drop only the outer brace
         * after copying, before appending a sibling approval_signature.
         * Use the old service's vetted JSON and replace one exact Boolean.
         * Leave all other previously published fields unchanged. */
        if (frontLength > UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY ||
            tailLength > UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY - frontLength)
        { status = UMI_STATUS_CAPACITY_EXCEEDED; goto cleanup; }
        memcpy(candidate, unsignedJson, start);
        memcpy(candidate + start, SIGNED, newLength);
        memcpy(candidate + frontLength, field + firstLength, tailLength);
        written = frontLength + tailLength;
        if (written == 0U || candidate[written - 1U] != '}') {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        /* The outer object remains valid: remove its closing brace and
         * append the signed-approval observation as a separate field. */
        --written;
    }
    if (written + sizeof(FOLLOWING) - 1U + 64U + sizeof(END) >
        UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto cleanup;
    }
    memcpy(candidate + written, FOLLOWING, sizeof(FOLLOWING) - 1U);
    written += sizeof(FOLLOWING) - 1U;
    memcpy(candidate + written, signer, 64U);
    written += 64U;
    memcpy(candidate + written, END, sizeof(END));
    written += sizeof(END) - 1U;
    if (written >= outputCapacity) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto cleanup;
    }
    memcpy(outJson, candidate, written + 1U);
    *outWritten = written;
    status = UMI_STATUS_OK;
cleanup:
    free(unsignedJson);
    free(candidate);
    return status;
}
