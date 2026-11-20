/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_mfa_database.c
 *
 * PURPOSE:
 *   Keep encrypted native authenticator seeds and replay transitions in one SQLite owner.
 *   Bind one-use recovery credentials to a specific verified password registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/local_mfa_database.h"
#include "umicom/base/sha256.h"
#include "umicom/security/profile_secrets.h"
#include "umicom/security/secrets.h"
#include "umicom/security/token.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/secure_random.h"
#include "local_profile_proof_internal.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
#include <stdio.h>
#include <sqlite3.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

enum { MFA_SEED_HEADER_BYTES = 4, MFA_PROTECTED_BYTES = 4096, RECOVERY_RANDOM_BYTES = 16 };
static_assert(UMI_LOCAL_MFA_RECOVERY_COUNT == 8U && UMI_SHA256_BYTES == 32U);
static_assert(UMI_LOCAL_PROFILE_BINDING_BYTES == 32U);
static_assert(UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH == 2U * RECOVERY_RANDOM_BYTES);

typedef struct RecoveryRecord {
    uint64_t credential;
    uint64_t serial;
    unsigned available;
    unsigned char digests[UMI_LOCAL_MFA_RECOVERY_COUNT][UMI_SHA256_BYTES];
} RecoveryRecord;

struct UmiLocalMfaDatabase {
    sqlite3 *handle;
    char application[96];
    bool usable;
};

static const char schema[] =
    "PRAGMA trusted_schema=OFF; PRAGMA secure_delete=ON; PRAGMA synchronous=FULL;"
    "CREATE TABLE IF NOT EXISTS local_mfa_v1("
    "application TEXT NOT NULL, username TEXT NOT NULL,"
    "generation INTEGER NOT NULL CHECK(generation>0),"
    "active INTEGER NOT NULL CHECK(active>=0 AND active<=generation),"
    "active_seed BLOB, replay INTEGER NOT NULL,"
    "pending INTEGER NOT NULL CHECK(pending>=0 AND pending<=generation),"
    "pending_seed BLOB, expires INTEGER NOT NULL,"
    "CHECK((active=0 AND active_seed IS NULL AND replay=-1) OR "
    "(active>0 AND typeof(active_seed)='blob' AND replay>=0)),"
    "CHECK((pending=0 AND pending_seed IS NULL AND expires=0) OR "
    "(pending>0 AND pending=generation AND active=0 AND typeof(pending_seed)='blob' AND expires>0)),"
    "PRIMARY KEY(application,username)) WITHOUT ROWID;"
    "CREATE TABLE IF NOT EXISTS local_mfa_recovery_v1("
    "application TEXT NOT NULL, username TEXT NOT NULL, credential INTEGER NOT NULL CHECK(credential>0),"
    "binding BLOB NOT NULL CHECK(typeof(binding)='blob' AND length(binding)=32),"
    "serial INTEGER NOT NULL CHECK(serial>=-1), available INTEGER NOT NULL CHECK(available BETWEEN 0 AND 255),"
    "digests BLOB, CHECK((serial=-1 AND available=0 AND digests IS NULL) OR "
    "(serial>=0 AND typeof(digests)='blob' AND length(digests)=256)),"
    "PRIMARY KEY(application,username)) WITHOUT ROWID;";

static UmiStatus StorageStatus(int result)
{
    switch (result & 255) {
    case SQLITE_OK: case SQLITE_DONE: case SQLITE_ROW: return UMI_STATUS_OK;
    case SQLITE_BUSY: case SQLITE_LOCKED: return UMI_STATUS_BUSY;
    case SQLITE_NOMEM: return UMI_STATUS_OUT_OF_MEMORY;
    case SQLITE_READONLY: case SQLITE_PERM: return UMI_STATUS_PERMISSION_DENIED;
    case SQLITE_CORRUPT: case SQLITE_NOTADB: return UMI_STATUS_PARSE_ERROR;
    default: return UMI_STATUS_IO_ERROR;
    }
}

static bool ParametersValid(UmiTotpAlgorithm algorithm, unsigned digits)
{
    return (algorithm == UMI_TOTP_SHA1 || algorithm == UMI_TOTP_SHA256) &&
        (digits == 6U || digits == 8U);
}

static int PrepareIdentity(const UmiLocalMfaDatabase *database, const char *profile,
    const char *sql, sqlite3_stmt **statement)
{
    int result = sqlite3_prepare_v2(database->handle, sql, -1, statement, NULL);
    if (result == SQLITE_OK)
        result = sqlite3_bind_text(*statement, 1, database->application, -1, SQLITE_STATIC);
    if (result == SQLITE_OK)
        result = sqlite3_bind_text(*statement, 2, profile, -1, SQLITE_STATIC);
    return result;
}

static UmiStatus FinishStatement(sqlite3_stmt *statement, UmiStatus status)
{
    const int finalized = sqlite3_finalize(statement);
    return status == UMI_STATUS_OK ? StorageStatus(finalized) : status;
}

static UmiStatus FinishUpdate(const UmiLocalMfaDatabase *database, sqlite3_stmt *statement, int result)
{
    if (result == SQLITE_OK) result = sqlite3_step(statement);
    UmiStatus status = StorageStatus(result);
    if (status == UMI_STATUS_OK && (result != SQLITE_DONE || sqlite3_changes(database->handle) != 1))
        status = UMI_STATUS_BUSY;
    return FinishStatement(statement, status);
}

/* Completion and rollback have identical rules for enrollment and recovery.
 * Retire the handle if SQLite cannot end a failed transaction cleanly. */
static UmiStatus FinishTransaction(UmiLocalMfaDatabase *database, UmiStatus status)
{
    if (status == UMI_STATUS_OK)
        status = StorageStatus(sqlite3_exec(database->handle, "COMMIT", NULL, NULL, NULL));
    if (status != UMI_STATUS_OK && !sqlite3_get_autocommit(database->handle)) {
        (void)sqlite3_exec(database->handle, "ROLLBACK", NULL, NULL, NULL);
        database->usable = sqlite3_get_autocommit(database->handle) != 0;
    }
    return status;
}

/* Bind every protected record to its app, canonical user and credential
 * generation. Swapping a ciphertext into another row cannot reuse a key. */
static DATA_BLOB ProtectionScope(const UmiLocalMfaDatabase *database, const char *profile,
    uint64_t revision, char *bytes, size_t capacity)
{
    const int length = snprintf(bytes, capacity, "umicom-mfa/v1|%s|%s|%llu",
        database->application, profile, (unsigned long long)revision);
    return (length > 0 && (size_t)length < capacity)
        ? (DATA_BLOB){(DWORD)length, (BYTE *)bytes} : (DATA_BLOB){0};
}

static void ReleaseProtected(DATA_BLOB *blob)
{
    if (blob->pbData != NULL) {
        umi_secret_clear(blob->pbData, blob->cbData);
        (void)LocalFree(blob->pbData);
    }
    *blob = (DATA_BLOB){0};
}

static UmiStatus ProtectSeed(const UmiLocalMfaDatabase *database, const char *profile,
    const UmiLocalMfaCredential *credential, DATA_BLOB *protected)
{
    char scope[192];
    DATA_BLOB entropy = ProtectionScope(database, profile, credential->credentialRevision, scope, sizeof scope);
    if (entropy.pbData == NULL) return UMI_STATUS_INTERNAL_ERROR;
    uint8_t bytes[MFA_SEED_HEADER_BYTES + UMI_TOTP_MAX_SECRET_BYTES] = {1U,
        (uint8_t)credential->algorithm, (uint8_t)credential->digits, (uint8_t)credential->secretBytes};
    memcpy(bytes + MFA_SEED_HEADER_BYTES, credential->secret, credential->secretBytes);
    DATA_BLOB plain = {(DWORD)(MFA_SEED_HEADER_BYTES + credential->secretBytes), bytes};
    const BOOL encrypted = CryptProtectData(&plain, L"Umicom authenticator", &entropy,
        NULL, NULL, CRYPTPROTECT_UI_FORBIDDEN, protected);
    umi_secret_clear(bytes, sizeof bytes);
    return encrypted && protected->cbData > 0U && protected->cbData <= MFA_PROTECTED_BYTES
        ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

static UmiStatus UnprotectSeed(const UmiLocalMfaDatabase *database, const char *profile, uint64_t revision,
    const void *bytes, size_t length, UmiLocalMfaCredential *out)
{
    char scope[192];
    DATA_BLOB entropy = ProtectionScope(database, profile, revision, scope, sizeof scope);
    if (entropy.pbData == NULL || bytes == NULL || length == 0U || length > MFA_PROTECTED_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    DATA_BLOB encrypted = {(DWORD)length, (BYTE *)bytes}, plain = {0};
    const BOOL decoded = CryptUnprotectData(&encrypted, NULL, &entropy, NULL, NULL,
        CRYPTPROTECT_UI_FORBIDDEN, &plain);
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    if (decoded && plain.pbData != NULL && plain.cbData >= MFA_SEED_HEADER_BYTES) {
        const uint8_t *data = plain.pbData;
        const UmiTotpAlgorithm algorithm = (UmiTotpAlgorithm)data[1];
        const size_t size = data[3];
        if (data[0] == 1U && ParametersValid(algorithm, data[2]) &&
            size >= UMI_TOTP_MIN_SECRET_BYTES && size <= UMI_TOTP_MAX_SECRET_BYTES &&
            plain.cbData == MFA_SEED_HEADER_BYTES + size) {
            out->credentialRevision = revision;
            out->algorithm = algorithm;
            out->digits = data[2];
            out->secretBytes = size;
            memcpy(out->secret, data + MFA_SEED_HEADER_BYTES, size);
            status = UMI_STATUS_OK;
        }
    }
    ReleaseProtected(&plain);
    return status;
}

static UmiStatus ReadSeed(const UmiLocalMfaDatabase *database, const char *profile, bool pending,
    UmiLocalMfaPendingEnrollment *record)
{
    if (database == NULL || UmiProfileSecretsScopeValidate(database->application, profile) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    const char *sql = pending
        ? "SELECT pending, pending_seed, expires FROM local_mfa_v1 WHERE application=?1 AND username=?2 AND pending>0"
        : "SELECT active, active_seed, replay FROM local_mfa_v1 WHERE application=?1 AND username=?2 AND active>0";
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile, sql, &statement);
    if (result == SQLITE_OK) result = sqlite3_step(statement);
    UmiStatus status = result == SQLITE_DONE ? UMI_STATUS_NOT_FOUND : StorageStatus(result);
    if (result == SQLITE_ROW) {
        const sqlite3_int64 revision = sqlite3_column_int64(statement, 0);
        const sqlite3_int64 data = sqlite3_column_int64(statement, 2);
        if (sqlite3_column_type(statement, 0) != SQLITE_INTEGER || revision <= 0 ||
            sqlite3_column_type(statement, 1) != SQLITE_BLOB ||
            sqlite3_column_type(statement, 2) != SQLITE_INTEGER || data < (pending ? 1 : 0)) {
            status = UMI_STATUS_PARSE_ERROR;
        } else {
            const void *seed = sqlite3_column_blob(statement, 1);
            const size_t length = (size_t)sqlite3_column_bytes(statement, 1);
            status = UnprotectSeed(database, profile, (uint64_t)revision,
                seed, length, &record->credential);
            if (status == UMI_STATUS_OK && pending) {
                record->enrollmentRevision = (uint64_t)revision;
                record->expiresAtNs = (uint64_t)data;
            } else if (status == UMI_STATUS_OK) {
                record->credential.replay = (UmiTotpReplay){(uint64_t)data, true};
            }
        }
    }
    return FinishStatement(statement, status);
}

static UmiStatus ReadActive(void *context, const char *profile, UmiLocalMfaCredential *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalMfaPendingEnrollment record = {0};
    UmiStatus status = ReadSeed(context, profile, false, &record);
    if (status == UMI_STATUS_OK) *out = record.credential;
    umi_secret_clear(&record, sizeof record);
    return status;
}

static UmiStatus ReadPending(void *context, const char *profile, UmiLocalMfaPendingEnrollment *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLocalMfaPendingEnrollment record = {0};
    UmiStatus status = ReadSeed(context, profile, true, &record);
    if (status == UMI_STATUS_OK) *out = record;
    umi_secret_clear(&record, sizeof record);
    return status;
}

static UmiStatus CommitReplay(void *context, const char *profile, uint64_t revision,
    UmiTotpReplay previous, UmiTotpReplay next)
{
    const UmiLocalMfaDatabase *database = context;
    if (database == NULL || UmiProfileSecretsScopeValidate(database->application, profile) != UMI_STATUS_OK ||
        revision == 0U || revision > INT64_MAX || !previous.has_last_step || !next.has_last_step ||
        next.last_step <= previous.last_step || next.last_step > INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "UPDATE local_mfa_v1 SET replay=?5 WHERE application=?1 AND username=?2 AND active=?3 AND replay=?4",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)revision);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 4, (sqlite3_int64)previous.last_step);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 5, (sqlite3_int64)next.last_step);
    return FinishUpdate(database, statement, result);
}

static UmiStatus Promote(void *context, const char *profile, uint64_t revision,
    uint64_t replaced, const UmiLocalMfaCredential *credential)
{
    const UmiLocalMfaDatabase *database = context;
    if (database == NULL || UmiProfileSecretsScopeValidate(database->application, profile) != UMI_STATUS_OK ||
        credential == NULL || revision == 0U || revision > INT64_MAX || replaced != 0U ||
        credential->credentialRevision != revision || !credential->replay.has_last_step ||
        credential->replay.last_step > INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    /* Move the exact protected pending bytes in one durable update. A second
     * confirmer or an older revision cannot overwrite an active factor. */
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "UPDATE local_mfa_v1 SET active=pending, active_seed=pending_seed, replay=?4,"
        "pending=0, pending_seed=NULL, expires=0 WHERE application=?1 AND username=?2 AND active=0 AND pending=?3",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)revision);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 4, (sqlite3_int64)credential->replay.last_step);
    return FinishUpdate(database, statement, result);
}

static UmiStatus NextGeneration(UmiLocalMfaDatabase *database, const char *profile, uint64_t *out)
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "SELECT generation, active FROM local_mfa_v1 WHERE application=?1 AND username=?2", &statement);
    if (result == SQLITE_OK) result = sqlite3_step(statement);
    UmiStatus status = StorageStatus(result);
    uint64_t revision = 1U;
    if (result == SQLITE_ROW) {
        const sqlite3_int64 stored = sqlite3_column_int64(statement, 0);
        const sqlite3_int64 active = sqlite3_column_int64(statement, 1);
        if (sqlite3_column_type(statement, 0) != SQLITE_INTEGER ||
            sqlite3_column_type(statement, 1) != SQLITE_INTEGER || stored <= 0 || active < 0 || active > stored)
            status = UMI_STATUS_PARSE_ERROR;
        else if (active != 0) status = UMI_STATUS_ALREADY_EXISTS;
        else if (stored == INT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else revision = (uint64_t)stored + 1U;
    }
    status = FinishStatement(statement, status);
    if (status == UMI_STATUS_OK) *out = revision;
    return status;
}

static UmiStatus WritePending(UmiLocalMfaDatabase *database, const char *profile, uint64_t revision,
    uint64_t expiresAtNs, const DATA_BLOB *protected)
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "INSERT INTO local_mfa_v1(application,username,generation,active,active_seed,replay,pending,pending_seed,expires)"
        "VALUES(?1,?2,?3,0,NULL,-1,?3,?4,?5) ON CONFLICT(application,username) DO UPDATE SET"
        " generation=excluded.generation,pending=excluded.pending,pending_seed=excluded.pending_seed,expires=excluded.expires"
        " WHERE active=0 AND generation+1=excluded.generation",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)revision);
    if (result == SQLITE_OK)
        result = sqlite3_bind_blob(statement, 4, protected->pbData, (int)protected->cbData, SQLITE_STATIC);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 5, (sqlite3_int64)expiresAtNs);
    return FinishUpdate(database, statement, result);
}

/* Enrollment records the owner without any recovery codes. Older on-disk
 * authenticator records still work through their original sign-in path. A
 * password record recreated with the same name obtains a different binding. */
static UmiStatus WriteRecoveryOwner(UmiLocalMfaDatabase *database, const char *profile, uint64_t credential,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES])
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "INSERT INTO local_mfa_recovery_v1(application,username,credential,binding,serial,available,digests)"
        " VALUES(?1,?2,?3,?4,-1,0,NULL) ON CONFLICT(application,username) DO UPDATE SET"
        " credential=excluded.credential,binding=excluded.binding,serial=-1,available=0,digests=NULL",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)credential);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 4, binding, UMI_LOCAL_PROFILE_BINDING_BYTES, SQLITE_STATIC);
    return FinishUpdate(database, statement, result);
}

static UmiStatus RecoveryOwnerMatches(UmiLocalMfaDatabase *database, const char *profile, uint64_t credential,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES])
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "SELECT 1 FROM local_mfa_recovery_v1 WHERE application=?1 AND username=?2 AND credential=?3 AND binding=?4",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)credential);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 4, binding, UMI_LOCAL_PROFILE_BINDING_BYTES, SQLITE_STATIC);
    if (result == SQLITE_OK) result = sqlite3_step(statement);
    return FinishStatement(statement, result == SQLITE_DONE ? UMI_STATUS_PERMISSION_DENIED : StorageStatus(result));
}

/* Holding the native writer lock for both the ownership recheck and the
 * existing transition keeps a retargeted enrollment out of the commit. */
static UmiStatus BeginOwnerWrite(UmiLocalMfaDatabase *database, const char *profile, uint64_t credential,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES])
{
    UmiStatus status = StorageStatus(sqlite3_exec(database->handle, "BEGIN IMMEDIATE", NULL, NULL, NULL));
    if (status == UMI_STATUS_OK) {
        status = RecoveryOwnerMatches(database, profile, credential, binding);
        if (status != UMI_STATUS_OK) status = FinishTransaction(database, status);
    }
    return status;
}

typedef struct ActiveAuthenticatorProof {
    uint64_t revision;
    UmiTotpReplay previous;
    UmiTotpReplay accepted;
} ActiveAuthenticatorProof;

/* The native login and recovery paths prove the same active enrollment using
 * the shared MAC and bounded time window, then consume it in their own write. */
static UmiStatus InspectActiveAuthenticator(UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], const char *code, size_t codeBytes,
    uint64_t nowNs, ActiveAuthenticatorProof *out)
{
    UmiLocalMfaCredential credential = {0};
    UmiStatus status = ReadActive(database, profile, &credential);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK)
        status = RecoveryOwnerMatches(database, profile, credential.credentialRevision, binding);
    ActiveAuthenticatorProof proof = {credential.credentialRevision, credential.replay, credential.replay};
    if (status == UMI_STATUS_OK)
        status = UmiLocalMfaVerifyCredentialCode(&credential, NULL, NULL, code, codeBytes, nowNs, &proof.accepted);
    umi_secret_clear(&credential, sizeof credential);
    if (status == UMI_STATUS_OK) *out = proof;
    return status;
}

static UmiStatus ReplaceRecovery(UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], const RecoveryRecord *record)
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "UPDATE local_mfa_recovery_v1 SET serial=?5,available=255,digests=?6"
        " WHERE application=?1 AND username=?2 AND credential=?3 AND binding=?4 AND serial<?5",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)record->credential);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 4, binding, UMI_LOCAL_PROFILE_BINDING_BYTES, SQLITE_STATIC);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 5, (sqlite3_int64)record->serial);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 6, record->digests, (int)sizeof record->digests, SQLITE_STATIC);
    return FinishUpdate(database, statement, result);
}

static UmiStatus ReadRecovery(UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], RecoveryRecord *out)
{
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "SELECT r.credential,r.serial,r.available,r.digests FROM local_mfa_recovery_v1 AS r"
        " JOIN local_mfa_v1 AS m ON m.application=r.application AND m.username=r.username AND m.active=r.credential"
        " WHERE r.application=?1 AND r.username=?2 AND r.binding=?3 AND r.available>0",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 3, binding, UMI_LOCAL_PROFILE_BINDING_BYTES, SQLITE_STATIC);
    if (result == SQLITE_OK) result = sqlite3_step(statement);
    UmiStatus status = result == SQLITE_DONE ? UMI_STATUS_PERMISSION_DENIED : StorageStatus(result);
    if (result == SQLITE_ROW) {
        const sqlite3_int64 credential = sqlite3_column_int64(statement, 0);
        const sqlite3_int64 serial = sqlite3_column_int64(statement, 1);
        const sqlite3_int64 available = sqlite3_column_int64(statement, 2);
        const int digestType = sqlite3_column_type(statement, 3);
        const void *digests = digestType == SQLITE_BLOB ? sqlite3_column_blob(statement, 3) : NULL;
        if (sqlite3_column_type(statement, 0) != SQLITE_INTEGER || credential <= 0 ||
            sqlite3_column_type(statement, 1) != SQLITE_INTEGER || serial < 0 ||
            sqlite3_column_type(statement, 2) != SQLITE_INTEGER || available < 1 || available > 255 ||
            digests == NULL ||
            sqlite3_column_bytes(statement, 3) != (int)sizeof out->digests) {
            status = UMI_STATUS_PARSE_ERROR;
        } else {
            out->credential = (uint64_t)credential;
            out->serial = (uint64_t)serial;
            out->available = (unsigned)available;
            memcpy(out->digests, digests, sizeof out->digests);
        }
    }
    return FinishStatement(statement, status);
}

static UmiStatus ConsumeRecovery(UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], const RecoveryRecord *record, unsigned bit)
{
    /* A spent slot or a newer recovery set cannot be accepted on a second
     * handle. Two different valid codes may still be consumed independently. */
    sqlite3_stmt *statement = NULL;
    int result = PrepareIdentity(database, profile,
        "UPDATE local_mfa_recovery_v1 SET available=available & ~?6"
        " WHERE application=?1 AND username=?2 AND credential=?3 AND serial=?4 AND binding=?5 AND (available & ?6)<>0"
        " AND EXISTS(SELECT 1 FROM local_mfa_v1 WHERE application=?1 AND username=?2 AND active=?3)",
        &statement);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 3, (sqlite3_int64)record->credential);
    if (result == SQLITE_OK) result = sqlite3_bind_int64(statement, 4, (sqlite3_int64)record->serial);
    if (result == SQLITE_OK) result = sqlite3_bind_blob(statement, 5, binding, UMI_LOCAL_PROFILE_BINDING_BYTES, SQLITE_STATIC);
    if (result == SQLITE_OK) result = sqlite3_bind_int(statement, 6, (int)bit);
    return FinishUpdate(database, statement, result);
}

/* The input has 128 random bits. A domain-separated digest binds it to the
 * database's app, canonical profile and exact password registration. Only
 * the digest survives the call, so a database reader cannot use it as a code. */
static UmiStatus RecoveryDigest(const UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], const unsigned char code[RECOVERY_RANDOM_BYTES],
    unsigned char out[UMI_SHA256_BYTES])
{
    const char domain[] = "umicom.local-mfa-recovery/v1";
    UmiSha256 hash;
    UmiSha256Init(&hash);
    UmiStatus status = UmiSha256Update(&hash, domain, sizeof domain);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&hash, database->application, strlen(database->application) + 1U);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&hash, profile, strlen(profile) + 1U);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&hash, binding, UMI_LOCAL_PROFILE_BINDING_BYTES);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&hash, code, RECOVERY_RANDOM_BYTES);
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, out);
    umi_secret_clear(&hash, sizeof hash);
    return status;
}

static UmiStatus GenerateRecovery(UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], RecoveryRecord *record,
    UmiLocalMfaRecoveryCodes *codes)
{
    static const char hex[] = "0123456789abcdef";
    UmiStatus status = UMI_STATUS_OK;
    unsigned char value[RECOVERY_RANDOM_BYTES] = {0};
    for (size_t index = 0U; status == UMI_STATUS_OK && index < UMI_LOCAL_MFA_RECOVERY_COUNT; ++index) {
        status = UmiSecureRandomBytes(value, sizeof value);
        if (status == UMI_STATUS_OK) status = RecoveryDigest(database, profile, binding, value, record->digests[index]);
        for (size_t prior = 0U; status == UMI_STATUS_OK && prior < index; ++prior)
            if (umi_token_constant_time_equal(record->digests[index], record->digests[prior], UMI_SHA256_BYTES))
                status = UMI_STATUS_BUSY;
        if (status == UMI_STATUS_OK) {
            for (size_t byte = 0U; byte < sizeof value; ++byte) {
                codes->codes[index][2U * byte] = hex[value[byte] >> 4U];
                codes->codes[index][2U * byte + 1U] = hex[value[byte] & 15U];
            }
            codes->codes[index][UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH] = '\0';
        }
    }
    umi_secret_clear(value, sizeof value);
    return status;
}

static int RecoveryNibble(unsigned char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'a' && character <= 'f') return character - 'a' + 10;
    return -1;
}

static UmiStatus RecoveryCandidate(const UmiLocalMfaDatabase *database, const char *profile,
    const unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES], const char *code, size_t codeBytes,
    unsigned char digest[UMI_SHA256_BYTES])
{
    if (codeBytes != UMI_LOCAL_MFA_RECOVERY_CODE_LENGTH) return UMI_STATUS_PERMISSION_DENIED;
    unsigned char value[RECOVERY_RANDOM_BYTES] = {0};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; index < sizeof value; ++index) {
        const int high = RecoveryNibble((unsigned char)code[2U * index]);
        const int low = RecoveryNibble((unsigned char)code[2U * index + 1U]);
        if (high < 0 || low < 0) {
            status = UMI_STATUS_PERMISSION_DENIED;
            break;
        }
        value[index] = (unsigned char)((high << 4) | low);
    }
    if (status == UMI_STATUS_OK) status = RecoveryDigest(database, profile, binding, value, digest);
    umi_secret_clear(value, sizeof value);
    return status;
}
#endif

UmiStatus UmiLocalMfaDatabaseOpen(const char *path, const char *application,
    UmiLocalMfaDatabase **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (UmiProfileSecretsScopeValidate(application, "profile") != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    UmiLocalMfaDatabase *database = calloc(1U, sizeof *database);
    if (database == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(database->application, application, strlen(application) + 1U);
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
#ifdef SQLITE_OPEN_NOFOLLOW
    flags |= SQLITE_OPEN_NOFOLLOW;
#endif
    int result = sqlite3_open_v2(path, &database->handle, flags, NULL);
    if (result == SQLITE_OK) result = sqlite3_extended_result_codes(database->handle, 1);
    if (result == SQLITE_OK) result = sqlite3_exec(database->handle, schema, NULL, NULL, NULL);
    if (result == SQLITE_OK) result = sqlite3_busy_timeout(database->handle, 5000);
    status = StorageStatus(result);
    if (status == UMI_STATUS_OK) {
        database->usable = true;
        *out = database;
    } else {
        UmiLocalMfaDatabaseClose(database);
    }
    return status;
#else
    return UMI_STATUS_UNAVAILABLE;
#endif
}

void UmiLocalMfaDatabaseClose(UmiLocalMfaDatabase *database)
{
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (database != NULL) {
        if (database->handle != NULL) (void)sqlite3_close_v2(database->handle);
        free(database);
    }
#else
    (void)database;
#endif
}

UmiStatus UmiLocalMfaDatabaseBackends(UmiLocalMfaDatabase *database,
    UmiLocalMfaBackend *signIn, UmiLocalMfaEnrollmentBackend *enrollment)
{
    if (database == NULL || signIn == NULL || enrollment == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    *signIn = (UmiLocalMfaBackend){.context = database, .read = ReadActive, .commitReplay = CommitReplay};
    *enrollment = (UmiLocalMfaEnrollmentBackend){.context = database, .readPending = ReadPending, .promote = Promote};
    return UMI_STATUS_OK;
#else
    return UMI_STATUS_UNAVAILABLE;
#endif
}

UmiStatus UmiLocalMfaDatabasePrepare(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    const char *issuer, UmiTotpAlgorithm algorithm, unsigned digits,
    uint64_t nowNs, uint64_t expiresAtNs, UmiLocalMfaPreparedEnrollment *out)
{
    if (database == NULL || profiles == NULL || password == NULL || out == NULL ||
        nowNs >= expiresAtNs || expiresAtNs > INT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (!ParametersValid(algorithm, digits)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES] = {0};
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerifyBound(profiles, canonical, password, binding);
    if (status != UMI_STATUS_OK) return status;

    UmiLocalMfaPreparedEnrollment prepared = {0};
    UmiLocalMfaCredential credential = {.algorithm = algorithm, .digits = digits,
        .secretBytes = algorithm == UMI_TOTP_SHA1 ? 20U : 32U};
    status = UmiSecureRandomBytes(credential.secret, credential.secretBytes);
    if (status == UMI_STATUS_OK)
        status = UmiTotpBuildEnrollmentUri(issuer, canonical, algorithm, digits, credential.secret,
            credential.secretBytes, prepared.uri, sizeof prepared.uri, &prepared.uriLength);

    /* Take the writer lock before assigning a generation. Publish the URI
     * only after the corresponding protected key is durably committed. */
    if (status == UMI_STATUS_OK)
        status = StorageStatus(sqlite3_exec(database->handle, "BEGIN IMMEDIATE", NULL, NULL, NULL));
    if (status == UMI_STATUS_OK) {
        DATA_BLOB protected = {0};
        status = NextGeneration(database, canonical, &credential.credentialRevision);
        if (status == UMI_STATUS_OK) status = ProtectSeed(database, canonical, &credential, &protected);
        if (status == UMI_STATUS_OK)
            status = WritePending(database, canonical, credential.credentialRevision, expiresAtNs, &protected);
        if (status == UMI_STATUS_OK)
            status = WriteRecoveryOwner(database, canonical, credential.credentialRevision, binding);
        ReleaseProtected(&protected);
        /* Recovery and enrollment share transaction completion. Preserve the
         * earlier completion block for review of its failure semantics. */
#if 0
        if (status == UMI_STATUS_OK)
            status = StorageStatus(sqlite3_exec(database->handle, "COMMIT", NULL, NULL, NULL));
        if (status != UMI_STATUS_OK && !sqlite3_get_autocommit(database->handle)) {
            (void)sqlite3_exec(database->handle, "ROLLBACK", NULL, NULL, NULL);
            /* A handle with an unresolved transaction cannot report a later
             * durable result. The caller can close it and reopen the store. */
            database->usable = sqlite3_get_autocommit(database->handle) != 0;
        }
#endif
        status = FinishTransaction(database, status);
    }
    if (status == UMI_STATUS_OK) {
        prepared.revision = credential.credentialRevision;
        *out = prepared;
    }
    umi_secret_clear(&prepared, sizeof prepared);
    umi_secret_clear(&credential, sizeof credential);
    umi_secret_clear(binding, sizeof binding);
    return status;
#else
    (void)profile; (void)issuer; (void)algorithm; (void)digits;
    return UMI_STATUS_UNAVAILABLE;
#endif
}

UmiStatus UmiLocalMfaDatabaseConfirmEnrollment(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    uint64_t enrollmentRevision, const char *code, size_t codeBytes, uint64_t nowNs)
{
    if (database == NULL || profiles == NULL || password == NULL || code == NULL || enrollmentRevision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES] = {0};
    UmiLocalMfaPendingEnrollment pending = {0};
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerifyBound(profiles, canonical, password, binding);
    if (status == UMI_STATUS_OK) {
        status = ReadPending(database, canonical, &pending);
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    }
    if (status == UMI_STATUS_OK && pending.enrollmentRevision != enrollmentRevision) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && nowNs >= pending.expiresAtNs) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK)
        status = RecoveryOwnerMatches(database, canonical, enrollmentRevision, binding);
    if (status == UMI_STATUS_OK)
        status = UmiLocalMfaVerifyCredentialCode(&pending.credential, NULL, NULL, code, codeBytes, nowNs,
                                                 &pending.credential.replay);
    if (status == UMI_STATUS_OK) status = BeginOwnerWrite(database, canonical, enrollmentRevision, binding);
    if (status == UMI_STATUS_OK) {
        status = Promote(database, canonical, enrollmentRevision, pending.replacesCredentialRevision, &pending.credential);
        status = FinishTransaction(database, status);
    }
    umi_secret_clear(&pending, sizeof pending);
    umi_secret_clear(binding, sizeof binding);
    return status;
#else
    (void)profile; (void)codeBytes; (void)nowNs;
    return UMI_STATUS_UNAVAILABLE;
#endif
}

UmiStatus UmiLocalMfaDatabaseSignIn(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions, const char *profile,
    const char *password, const char *code, size_t codeBytes, uint64_t nowNs,
    uint64_t lifetimeNs, UmiSessionRecord *out)
{
    if (database == NULL || profiles == NULL || sessions == NULL || password == NULL || code == NULL ||
        out == NULL || lifetimeNs == 0U || lifetimeNs > UINT64_MAX - nowNs)
        return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES] = {0};
    ActiveAuthenticatorProof proof = {0};
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerifyBound(profiles, canonical, password, binding);
    if (status == UMI_STATUS_OK)
        status = InspectActiveAuthenticator(database, canonical, binding, code, codeBytes, nowNs, &proof);
    if (status == UMI_STATUS_OK) status = BeginOwnerWrite(database, canonical, proof.revision, binding);
    if (status == UMI_STATUS_OK) {
        status = CommitReplay(database, canonical, proof.revision, proof.previous, proof.accepted);
        status = FinishTransaction(database, status);
    }
    umi_secret_clear(binding, sizeof binding);
    return status == UMI_STATUS_OK ? umi_session_registry_issue(sessions, canonical, nowNs, lifetimeNs, out) : status;
#else
    (void)profile; (void)codeBytes;
    return UMI_STATUS_UNAVAILABLE;
#endif
}

UmiStatus UmiLocalMfaDatabaseIssueRecovery(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, const char *profile, const char *password,
    const char *authenticatorCode, size_t codeBytes, uint64_t nowNs, UmiLocalMfaRecoveryCodes *out)
{
    if (database == NULL || profiles == NULL || password == NULL || authenticatorCode == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES] = {0};
    UmiLocalMfaRecoveryCodes codes = {0};
    RecoveryRecord record = {0};
    ActiveAuthenticatorProof proof = {0};
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerifyBound(profiles, canonical, password, binding);
    /* Keep the previous inline proof for review. The native sign-in path and
     * recovery issuance now share the same account-bound authenticator proof. */
#if 0
    UmiLocalMfaCredential credential = {0};
    if (status == UMI_STATUS_OK) {
        status = ReadActive(database, canonical, &credential);
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PERMISSION_DENIED;
    }
    if (status == UMI_STATUS_OK)
        status = RecoveryOwnerMatches(database, canonical, credential.credentialRevision, binding);
    const UmiTotpReplay previous = credential.replay;
    UmiTotpReplay accepted = previous;
    if (status == UMI_STATUS_OK)
        status = UmiLocalMfaVerifyCredentialCode(&credential, NULL, NULL, authenticatorCode, codeBytes, nowNs, &accepted);
    record.credential = credential.credentialRevision;
    record.serial = accepted.last_step;
    proof = (ActiveAuthenticatorProof){record.credential, previous, accepted};
    umi_secret_clear(&credential, sizeof credential);
#else
    if (status == UMI_STATUS_OK)
        status = InspectActiveAuthenticator(database, canonical, binding, authenticatorCode, codeBytes, nowNs, &proof);
    record.credential = proof.revision;
    record.serial = proof.accepted.last_step;
#endif
    if (status == UMI_STATUS_OK) status = GenerateRecovery(database, canonical, binding, &record, &codes);

    /* Publish the code set only alongside the durable consumption of the
     * current authenticator step. Either update can lose a race and roll back. */
    if (status == UMI_STATUS_OK)
        status = StorageStatus(sqlite3_exec(database->handle, "BEGIN IMMEDIATE", NULL, NULL, NULL));
    if (status == UMI_STATUS_OK) {
        status = CommitReplay(database, canonical, proof.revision, proof.previous, proof.accepted);
        if (status == UMI_STATUS_OK) status = ReplaceRecovery(database, canonical, binding, &record);
        status = FinishTransaction(database, status);
    }
    if (status == UMI_STATUS_OK) *out = codes;
    umi_secret_clear(&record, sizeof record);
    umi_secret_clear(&codes, sizeof codes);
    umi_secret_clear(binding, sizeof binding);
    return status;
#else
    (void)profile; (void)codeBytes; (void)nowNs;
    return UMI_STATUS_UNAVAILABLE;
#endif
}

UmiStatus UmiLocalMfaDatabaseRecoverySignIn(UmiLocalMfaDatabase *database,
    UmiLocalProfileStore *profiles, UmiSessionRegistry *sessions, const char *profile,
    const char *password, const char *recoveryCode, size_t codeBytes, uint64_t nowNs,
    uint64_t lifetimeNs, UmiSessionRecord *out)
{
    if (database == NULL || profiles == NULL || sessions == NULL || password == NULL ||
        recoveryCode == NULL || out == NULL || lifetimeNs == 0U || lifetimeNs > UINT64_MAX - nowNs)
        return UMI_STATUS_INVALID_ARGUMENT;
#if defined(_WIN32) && defined(UMI_LOCAL_PROFILE_HAS_SQLITE)
    if (!database->usable) return UMI_STATUS_INVALID_STATE;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    unsigned char binding[UMI_LOCAL_PROFILE_BINDING_BYTES] = {0};
    unsigned char digest[UMI_SHA256_BYTES] = {0};
    RecoveryRecord record = {0};
    UmiStatus status = UmiLocalProfileName(profile, canonical);
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerifyBound(profiles, canonical, password, binding);
    if (status == UMI_STATUS_OK) status = RecoveryCandidate(database, canonical, binding, recoveryCode, codeBytes, digest);
    if (status == UMI_STATUS_OK) status = ReadRecovery(database, canonical, binding, &record);
    if (status == UMI_STATUS_OK) {
        unsigned matches = 0U;
        for (size_t index = 0U; index < UMI_LOCAL_MFA_RECOVERY_COUNT; ++index) {
            const unsigned equal = (unsigned)umi_token_constant_time_equal(digest, record.digests[index], sizeof digest);
            matches |= equal << index;
        }
        matches &= record.available;
        if (matches == 0U) status = UMI_STATUS_PERMISSION_DENIED;
        else if ((matches & (matches - 1U)) != 0U) status = UMI_STATUS_PARSE_ERROR;
        else status = ConsumeRecovery(database, canonical, binding, &record, matches);
    }
    umi_secret_clear(&record, sizeof record);
    umi_secret_clear(digest, sizeof digest);
    umi_secret_clear(binding, sizeof binding);
    /* Session issuance has the same ordering as ordinary authenticator login.
     * A later failure never returns a durable code to the available set. */
    return status == UMI_STATUS_OK ? umi_session_registry_issue(sessions, canonical, nowNs, lifetimeNs, out) : status;
#else
    (void)profile; (void)codeBytes;
    return UMI_STATUS_UNAVAILABLE;
#endif
}
