/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_attestation.c
 *
 * PURPOSE:
 *   Provide optional, domain-separated Ed25519 signing and strict offline
 *   verification for the existing four-file training publication marker.
 *
 * IMPLEMENTATION BOUNDARIES:
 *   Never implement cryptographic primitives in application code. OpenSSL's
 *   reviewed Ed25519 implementation is used only when this optional target
 *   is explicitly enabled. The existing Framework SHA-256 and publication
 *   ownership remain unchanged. The signed message is a fixed domain label
 *   followed by the EXACT original .publication bytes (not a JSON object
 *   with ambiguous serialisation). The detached record uses canonical ASCII.
 *
 *   This verifies data consistency and possession of the signing key; it is
 *   NOT a timestamp, certification chain, transparency log, or proof of who
 *   owns the externally supplied trust anchor.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_attestation.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
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

#define UMI_ATTESTATION_DOMAIN "UMICOM-AI-ATTESTATION-CONTEXT-1\n"
#define UMI_ATTESTATION_MAGIC "UMICOM-AI-PUBLICATION-ED25519-1\n"
#define UMI_ATTESTATION_KEY_LABEL "public_key_sha256="
#define UMI_ATTESTATION_SIGNATURE_LABEL "signature="
#define UMI_ATTESTATION_STAGE_SUFFIX ".partial"
#define UMI_ATTESTATION_MAX_KEY_BYTES 16384U
#define UMI_ATTESTATION_SIGNATURE_BYTES 64U
#define UMI_ATTESTATION_KEY_BYTES 32U

_Static_assert(sizeof(UMI_ATTESTATION_MAGIC) - 1U +
               sizeof(UMI_ATTESTATION_KEY_LABEL) - 1U +
               (UMI_SHA256_BYTES * 2U) + 1U +
               sizeof(UMI_ATTESTATION_SIGNATURE_LABEL) - 1U +
               (UMI_ATTESTATION_SIGNATURE_BYTES * 2U) + 1U ==
               UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES,
               "Attestation record length changed; review the format contract");

/* Use only fixed suffixes; a reviewed checkpoint path is not a sandbox root. */
static UmiStatus MakePath(const char *prefix, const char *suffix,
                          char out[UMI_AI_TRAINING_PUBLICATION_MAX_PATH])
{
    size_t first, second;
    if (prefix == NULL || suffix == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    first = strlen(prefix);
    second = strlen(suffix);
    if (first == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (first >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH ||
        second >= UMI_AI_TRAINING_PUBLICATION_MAX_PATH - first)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, prefix, first);
    memcpy(out + first, suffix, second + 1U);
    return UMI_STATUS_OK;
}

/* All input sizes are bounded independently of the underlying filesystem.
 * This function does not retain the file's path, descriptor or contents. */
static UmiStatus ReadFile(const char *path, unsigned char *out,
                          size_t capacity, size_t *outLength)
{
    FILE *input;
    size_t used;
    int extra, failure;
    if (path == NULL || path[0] == '\0' || out == NULL || outLength == NULL ||
        capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    input = fopen(path, "rb");
    if (input == NULL) return UMI_STATUS_IO_ERROR;
    used = fread(out, 1U, capacity, input);
    extra = fgetc(input);
    failure = ferror(input);
    if (fclose(input) != 0) failure = 1;
    if (failure != 0) return UMI_STATUS_IO_ERROR;
    if (used == capacity || extra != EOF) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (used == 0U) return UMI_STATUS_PARSE_ERROR;
    *outLength = used;
    return UMI_STATUS_OK;
}

static UmiStatus ReadPublication(const char *checkpointPath,
                                 unsigned char bytes[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY],
                                 size_t *outLength)
{
    char path[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    UmiStatus status = MakePath(checkpointPath, ".publication", path);
    if (status != UMI_STATUS_OK) return status;
    return ReadFile(path, bytes, UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY,
                    outLength);
}

/* A UTF-8/plain-ASCII public-key file is a caller-approved trust anchor; a
 * signature's embedded key fingerprint is never accepted as its own anchor. */
static UmiStatus ReadPem(const char *path, int privateKey, EVP_PKEY **outKey)
{
    unsigned char bytes[UMI_ATTESTATION_MAX_KEY_BYTES];
    size_t count = 0U;
    BIO *bio;
    EVP_PKEY *key;
    UmiStatus status;
    if (outKey == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReadFile(path, bytes, sizeof(bytes), &count);
    if (status != UMI_STATUS_OK) return status;
    if (count > INT_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    bio = BIO_new_mem_buf(bytes, (int)count);
    if (bio == NULL) {
        OPENSSL_cleanse(bytes, sizeof(bytes));
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (privateKey != 0)
        key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    else
        key = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    /* Remove the copied private PEM from stack storage even after failure.
     * The OpenSSL key object owns its internal key material and is later freed. */
    BIO_free(bio);
    OPENSSL_cleanse(bytes, sizeof(bytes));
    if (key == NULL) return UMI_STATUS_PARSE_ERROR;
    if (EVP_PKEY_base_id(key) != EVP_PKEY_ED25519) {
        EVP_PKEY_free(key);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outKey = key;
    return UMI_STATUS_OK;
}

/* The anchor fingerprint identifies the raw 32-byte Ed25519 public key, not
 * its particular PEM encoding (line breaks and headers can vary). */
static UmiStatus KeyFingerprint(EVP_PKEY *key,
                                char out[UMI_SHA256_HEX_CAPACITY])
{
    unsigned char publicBytes[UMI_ATTESTATION_KEY_BYTES];
    size_t length = sizeof(publicBytes);
    if (key == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (EVP_PKEY_get_raw_public_key(key, publicBytes, &length) != 1 ||
        length != sizeof(publicBytes)) return UMI_STATUS_PARSE_ERROR;
    return UmiSha256Buffer(publicBytes, sizeof(publicBytes), out);
}

static void HexEncode(const unsigned char *source, size_t size, char *destination)
{
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0U; i < size; ++i) {
        destination[i * 2U] = digits[source[i] >> 4U];
        destination[i * 2U + 1U] = digits[source[i] & 15U];
    }
    destination[size * 2U] = '\0';
}

static int HexDigit(unsigned char ch)
{
    if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9')
        return (int)(ch - (unsigned char)'0');
    if (ch >= (unsigned char)'a' && ch <= (unsigned char)'f')
        return 10 + (int)(ch - (unsigned char)'a');
    return -1;
}

static UmiStatus DecodeSignatureRecord(
    const unsigned char *record, size_t length,
    char fingerprint[UMI_SHA256_HEX_CAPACITY],
    unsigned char signature[UMI_ATTESTATION_SIGNATURE_BYTES])
{
    const char prefix[] = UMI_ATTESTATION_MAGIC UMI_ATTESTATION_KEY_LABEL;
    const char middle[] = "\n" UMI_ATTESTATION_SIGNATURE_LABEL;
    size_t at = 0U;
    if (record == NULL || fingerprint == NULL || signature == NULL ||
        length != UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES ||
        memcmp(record, prefix, sizeof(prefix) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    at = sizeof(prefix) - 1U;
    for (size_t i = 0U; i < UMI_SHA256_BYTES * 2U; ++i) {
        if (HexDigit(record[at + i]) < 0) return UMI_STATUS_PARSE_ERROR;
        fingerprint[i] = (char)record[at + i];
    }
    fingerprint[UMI_SHA256_BYTES * 2U] = '\0';
    at += UMI_SHA256_BYTES * 2U;
    if (memcmp(record + at, middle, sizeof(middle) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    at += sizeof(middle) - 1U;
    for (size_t i = 0U; i < UMI_ATTESTATION_SIGNATURE_BYTES; ++i) {
        int high = HexDigit(record[at + i * 2U]);
        int low = HexDigit(record[at + i * 2U + 1U]);
        if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
        signature[i] = (unsigned char)((high << 4) | low);
    }
    at += UMI_ATTESTATION_SIGNATURE_BYTES * 2U;
    if (at + 1U != length || record[at] != (unsigned char)'\n')
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

static UmiStatus FormatSignatureRecord(
    const char keyFingerprint[UMI_SHA256_HEX_CAPACITY],
    const unsigned char signature[UMI_ATTESTATION_SIGNATURE_BYTES],
    unsigned char record[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES])
{
    char signatureHex[UMI_ATTESTATION_SIGNATURE_BYTES * 2U + 1U];
    char candidate[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES + 1U];
    int written;
    if (keyFingerprint == NULL || signature == NULL || record == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    HexEncode(signature, UMI_ATTESTATION_SIGNATURE_BYTES, signatureHex);
    written = snprintf(candidate, sizeof(candidate),
                       UMI_ATTESTATION_MAGIC
                       UMI_ATTESTATION_KEY_LABEL "%s\n"
                       UMI_ATTESTATION_SIGNATURE_LABEL "%s\n",
                       keyFingerprint, signatureHex);
    if (written != (int)UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES)
        return UMI_STATUS_INTERNAL_ERROR;
    memcpy(record, candidate, (size_t)written);
    return UMI_STATUS_OK;
}

/* Ed25519 signs the entire domain-separated canonical marker. No prehash or
 * algorithm negotiation is performed; this avoids cross-protocol ambiguity. */
static UmiStatus SignMessage(EVP_PKEY *privateKey,
                             const unsigned char *message, size_t length,
                             unsigned char outSignature[UMI_ATTESTATION_SIGNATURE_BYTES])
{
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    size_t signatureLength = UMI_ATTESTATION_SIGNATURE_BYTES;
    int succeeded;
    if (context == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    succeeded = EVP_DigestSignInit(context, NULL, NULL, NULL, privateKey) == 1 &&
                EVP_DigestSign(context, outSignature, &signatureLength,
                               message, length) == 1;
    EVP_MD_CTX_free(context);
    return succeeded && signatureLength == UMI_ATTESTATION_SIGNATURE_BYTES ?
           UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

static UmiStatus VerifyMessage(EVP_PKEY *publicKey,
                               const unsigned char *message, size_t length,
                               const unsigned char signature[UMI_ATTESTATION_SIGNATURE_BYTES])
{
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    int verified;
    if (context == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    verified = EVP_DigestVerifyInit(context, NULL, NULL, NULL, publicKey) == 1 &&
               EVP_DigestVerify(context, signature,
                                UMI_ATTESTATION_SIGNATURE_BYTES,
                                message, length) == 1;
    EVP_MD_CTX_free(context);
    return verified ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

static UmiStatus BuildMessage(const unsigned char *publication,
                              size_t publicationLength,
                              unsigned char message[sizeof(UMI_ATTESTATION_DOMAIN) - 1U +
                                                    UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY],
                              size_t *outLength)
{
    const size_t prefix = sizeof(UMI_ATTESTATION_DOMAIN) - 1U;
    if (publication == NULL || outLength == NULL || publicationLength == 0U ||
        publicationLength >= UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(message, UMI_ATTESTATION_DOMAIN, prefix);
    memcpy(message + prefix, publication, publicationLength);
    *outLength = prefix + publicationLength;
    return UMI_STATUS_OK;
}

#if defined(_WIN32)
static int CloseFile(int handle) { return _close(handle); }
static int SyncFile(int handle) { return _commit(handle); }
#else
static int CloseFile(int handle) { return close(handle); }
static int SyncFile(int handle) { return fsync(handle); }
#endif

/* An attestation is immutable once visible. The original publication service
 * keeps its writer private to preserve its existing ABI, so this optional
 * detached evidence writer implements the same reviewed no-clobber discipline. */
static UmiStatus SaveNewRecord(
    const char *checkpointPath,
    const unsigned char record[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES])
{
    char finalPath[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    char staging[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    size_t offset = 0U;
    int descriptor;
    UmiStatus status;
    status = MakePath(checkpointPath, UMI_AI_TRAINING_ATTESTATION_SUFFIX,
                      finalPath);
    if (status != UMI_STATUS_OK) return status;
    status = MakePath(checkpointPath, UMI_AI_TRAINING_ATTESTATION_SUFFIX
                      UMI_ATTESTATION_STAGE_SUFFIX, staging);
    if (status != UMI_STATUS_OK) return status;
#if defined(_WIN32)
    descriptor = _open(staging, _O_CREAT | _O_EXCL | _O_WRONLY | _O_BINARY,
                       _S_IREAD | _S_IWRITE);
#else
    descriptor = open(staging, O_CREAT | O_EXCL | O_WRONLY, 0600);
#endif
    if (descriptor < 0)
        return errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
    while (offset < UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES) {
#if defined(_WIN32)
        int amount = _write(descriptor, record + offset,
            (unsigned int)(UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES - offset));
#else
        ssize_t amount = write(descriptor, record + offset,
            UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES - offset);
#endif
        if (amount <= 0) { status = UMI_STATUS_IO_ERROR; goto reject; }
        offset += (size_t)amount;
    }
    {
        int flushResult = SyncFile(descriptor);
        int closeResult = CloseFile(descriptor);
        descriptor = -1;
        if (flushResult != 0 || closeResult != 0) {
            status = UMI_STATUS_IO_ERROR;
            goto reject;
        }
    }
#if defined(_WIN32)
    if (!MoveFileExA(staging, finalPath, MOVEFILE_WRITE_THROUGH)) {
        DWORD error = GetLastError();
        status = error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS ?
                 UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto reject;
    }
#else
    if (link(staging, finalPath) != 0) {
        status = errno == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
        goto reject;
    }
    (void)remove(staging);
#endif
    return UMI_STATUS_OK;
reject:
    if (descriptor >= 0) (void)CloseFile(descriptor);
    (void)remove(staging);
    return status;
}

UmiStatus UmiAiTrainingAttestationSignNewFile(
    const char *checkpointPath, const char *privateKeyPemPath)
{
    EVP_PKEY *privateKey = NULL;
    unsigned char publication[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    unsigned char message[sizeof(UMI_ATTESTATION_DOMAIN) - 1U +
                          UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    unsigned char signature[UMI_ATTESTATION_SIGNATURE_BYTES];
    unsigned char record[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES];
    char fingerprint[UMI_SHA256_HEX_CAPACITY];
    size_t publicationLength = 0U, messageLength = 0U;
    UmiStatus status;
    if (checkpointPath == NULL || privateKeyPemPath == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Never endorse a bundle whose four files do not agree with its record. */
    status = UmiAiTrainingPublicationVerifyFile(checkpointPath);
    if (status != UMI_STATUS_OK) return status;
    status = ReadPublication(checkpointPath, publication, &publicationLength);
    if (status != UMI_STATUS_OK) return status;
    status = BuildMessage(publication, publicationLength,
                          message, &messageLength);
    if (status != UMI_STATUS_OK) return status;
    status = ReadPem(privateKeyPemPath, 1, &privateKey);
    if (status != UMI_STATUS_OK) return status;
    status = KeyFingerprint(privateKey, fingerprint);
    if (status == UMI_STATUS_OK)
        status = SignMessage(privateKey, message, messageLength, signature);
    EVP_PKEY_free(privateKey);
    if (status != UMI_STATUS_OK) return status;
    status = FormatSignatureRecord(fingerprint, signature, record);
    if (status != UMI_STATUS_OK) return status;
    return SaveNewRecord(checkpointPath, record);
}

UmiStatus UmiAiTrainingAttestationVerifyAndInspectFile(
    const char *checkpointPath, const char *trustedPublicKeyPemPath,
    UmiAiTrainingPublicationEvidence *outEvidence)
{
    EVP_PKEY *trustedKey = NULL;
    unsigned char publication[UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    unsigned char message[sizeof(UMI_ATTESTATION_DOMAIN) - 1U +
                          UMI_AI_TRAINING_PUBLICATION_RECORD_CAPACITY];
    unsigned char actual[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES + 1U];
    unsigned char signature[UMI_ATTESTATION_SIGNATURE_BYTES];
    char signedFingerprint[UMI_SHA256_HEX_CAPACITY];
    char trustedFingerprint[UMI_SHA256_HEX_CAPACITY];
    char signaturePath[UMI_AI_TRAINING_PUBLICATION_MAX_PATH];
    UmiAiTrainingPublicationEvidence candidate;
    size_t actualLength = 0U, publicationLength = 0U, messageLength = 0U;
    UmiStatus status;
    if (outEvidence == NULL || checkpointPath == NULL ||
        trustedPublicKeyPemPath == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = MakePath(checkpointPath, UMI_AI_TRAINING_ATTESTATION_SUFFIX,
                      signaturePath);
    if (status != UMI_STATUS_OK) return status;
    status = ReadFile(signaturePath, actual, sizeof(actual), &actualLength);
    if (status != UMI_STATUS_OK) return status;
    status = DecodeSignatureRecord(actual, actualLength, signedFingerprint,
                                   signature);
    if (status != UMI_STATUS_OK) return status;
    status = ReadPem(trustedPublicKeyPemPath, 0, &trustedKey);
    if (status != UMI_STATUS_OK) return status;
    status = KeyFingerprint(trustedKey, trustedFingerprint);
    if (status == UMI_STATUS_OK &&
        strcmp(signedFingerprint, trustedFingerprint) != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = ReadPublication(checkpointPath, publication, &publicationLength);
    if (status == UMI_STATUS_OK)
        status = BuildMessage(publication, publicationLength,
                              message, &messageLength);
    if (status == UMI_STATUS_OK)
        status = VerifyMessage(trustedKey, message, messageLength, signature);
    EVP_PKEY_free(trustedKey);
    if (status != UMI_STATUS_OK) return status;
    /* Signature identity precedes underlying file integrity; publish an
     * evidence snapshot only after both independent checks succeed. */
    status = UmiAiTrainingPublicationVerifyAndInspectFile(checkpointPath,
                                                           &candidate);
    if (status != UMI_STATUS_OK) return status;
    *outEvidence = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingAttestationVerifyFile(
    const char *checkpointPath, const char *trustedPublicKeyPemPath)
{
    UmiAiTrainingPublicationEvidence ignored;
    return UmiAiTrainingAttestationVerifyAndInspectFile(
        checkpointPath, trustedPublicKeyPemPath, &ignored);
}

/* Public-key onboarding must compute the SAME canonical raw-key identity
 * used by the signature verifier; hashing PEM text would be incorrect.
 * Retain all caller output bytes on a malformed or inaccessible key. */
UmiStatus UmiAiTrainingAttestationPublicKeyFingerprint(
    const char *publicKeyPemPath,
    char outFingerprint[UMI_SHA256_HEX_CAPACITY])
{
    EVP_PKEY *key = NULL;
    char candidate[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status;
    if (publicKeyPemPath == NULL || outFingerprint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ReadPem(publicKeyPemPath, 0, &key);
    if (status != UMI_STATUS_OK) return status;
    status = KeyFingerprint(key, candidate);
    EVP_PKEY_free(key);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outFingerprint, candidate, sizeof(candidate));
    return UMI_STATUS_OK;
}
