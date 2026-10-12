/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/evidence_trace.c
 *
 * PURPOSE:
 *   Verify deterministic driver/service state transitions and SHA-256 links
 *   in a bounded, read-only educational diagnostic file. Failed inspection
 *   must never publish a partly accepted result or modify existing evidence.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/evidence_trace.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum EvidenceDriverStage {
    EVIDENCE_EMPTY = 0,
    EVIDENCE_ATTACHED,
    EVIDENCE_READY_STAGE,
    EVIDENCE_FAULTED_STAGE,
    EVIDENCE_QUIESCING,
    EVIDENCE_STOPPED_STAGE
} EvidenceDriverStage;

typedef struct EvidenceDriver {
    EvidenceDriverStage stage;
    uint32_t owner;
    uint32_t generation;
    uint32_t lastGeneration;
    uint64_t lastObservation;
    uint64_t checkpoint;
} EvidenceDriver;

typedef struct EvidenceModel {
    EvidenceDriver drivers[UMI_EDUCATION_EVIDENCE_MAX_DRIVERS];
    uint64_t lastTick;
    uint64_t lastObservation;
    uint32_t live;
} EvidenceModel;

static uint16_t Read16(const unsigned char *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8U));
}
static uint32_t Read32(const unsigned char *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8U) |
           ((uint32_t)bytes[2] << 16U) | ((uint32_t)bytes[3] << 24U);
}
static uint64_t Read64(const unsigned char *bytes)
{
    return (uint64_t)Read32(bytes) | ((uint64_t)Read32(bytes + 4U) << 32U);
}

/* A regular record is never allowed to manufacture a diagnostic identifier or
 * silently introduce a checkpoint. Only OBSERVE/FAULT and CHECKPOINT/RECOVER
 * own those fields, respectively. */
static int EmptyFields(uint32_t code, uint64_t observation, uint64_t checkpoint,
                       uint8_t severity)
{
    return code == 0U && observation == 0U && checkpoint == 0U && severity == 0U;
}

static UmiStatus Apply(EvidenceModel *model, UmiEducationEvidenceReport *report,
                       const unsigned char *record)
{
    const uint8_t kind = record[4U];
    const uint8_t severity = record[5U];
    const uint32_t slot = Read32(record + 8U);
    const uint32_t owner = Read32(record + 12U);
    const uint32_t generation = Read32(record + 16U);
    const uint32_t code = Read32(record + 20U);
    const uint64_t tick = Read64(record + 24U);
    const uint64_t observation = Read64(record + 32U);
    const uint64_t checkpoint = Read64(record + 40U);
    if (slot == 0U || slot > UMI_EDUCATION_EVIDENCE_MAX_DRIVERS ||
        owner == 0U || generation == 0U || severity > 2U || tick < model->lastTick)
        return UMI_STATUS_PARSE_ERROR;
    EvidenceDriver *driver = &model->drivers[(size_t)slot - 1U];
    if (kind == UMI_EDUCATION_EVIDENCE_ATTACH) {
        if (!EmptyFields(code, observation, checkpoint, severity) ||
            driver->stage != EVIDENCE_EMPTY || generation <= driver->lastGeneration)
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_ATTACHED;
        driver->owner = owner;
        driver->generation = generation;
        driver->lastGeneration = generation;
        driver->lastObservation = 0U;
        driver->checkpoint = 0U;
        ++model->live;
        ++report->attached;
        if (model->live > report->peakLive) report->peakLive = model->live;
        model->lastTick = tick;
        return UMI_STATUS_OK;
    }
    if (driver->stage == EVIDENCE_EMPTY || driver->owner != owner ||
        driver->generation != generation)
        return UMI_STATUS_PARSE_ERROR;
    switch (kind) {
    case UMI_EDUCATION_EVIDENCE_READY:
        if (driver->stage != EVIDENCE_ATTACHED ||
            !EmptyFields(code, observation, checkpoint, severity))
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_READY_STAGE;
        ++report->ready;
        break;
    case UMI_EDUCATION_EVIDENCE_OBSERVE:
        if ((driver->stage != EVIDENCE_READY_STAGE &&
             driver->stage != EVIDENCE_FAULTED_STAGE) ||
            code == 0U || observation == 0U ||
            observation <= model->lastObservation || checkpoint != 0U)
            return UMI_STATUS_PARSE_ERROR;
        model->lastObservation = observation;
        driver->lastObservation = observation;
        ++report->observations;
        break;
    case UMI_EDUCATION_EVIDENCE_CHECKPOINT:
        /* A checkpoint is a reference to real, previously seen evidence, not
         * a caller-invented promise of durable storage. */
        if (driver->stage != EVIDENCE_READY_STAGE || severity != 0U ||
            code != 0U || observation != 0U || checkpoint == 0U ||
            checkpoint != driver->lastObservation || checkpoint <= driver->checkpoint)
            return UMI_STATUS_PARSE_ERROR;
        driver->checkpoint = checkpoint;
        ++report->checkpoints;
        break;
    case UMI_EDUCATION_EVIDENCE_FAULT:
        if (driver->stage != EVIDENCE_READY_STAGE || severity != 2U ||
            code == 0U || observation == 0U ||
            observation <= model->lastObservation || checkpoint != 0U)
            return UMI_STATUS_PARSE_ERROR;
        model->lastObservation = observation;
        driver->lastObservation = observation;
        driver->stage = EVIDENCE_FAULTED_STAGE;
        ++report->faults;
        break;
    case UMI_EDUCATION_EVIDENCE_RECOVER:
        if (driver->stage != EVIDENCE_FAULTED_STAGE || severity != 0U ||
            code != 0U || observation != 0U || checkpoint == 0U ||
            checkpoint != driver->checkpoint)
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_READY_STAGE;
        ++report->recoveries;
        break;
    case UMI_EDUCATION_EVIDENCE_QUIESCE:
        if (driver->stage != EVIDENCE_READY_STAGE ||
            !EmptyFields(code, observation, checkpoint, severity))
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_QUIESCING;
        ++report->quiesced;
        break;
    case UMI_EDUCATION_EVIDENCE_STOP:
        if ((driver->stage != EVIDENCE_QUIESCING &&
             driver->stage != EVIDENCE_FAULTED_STAGE) ||
            !EmptyFields(code, observation, checkpoint, severity))
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_STOPPED_STAGE;
        ++report->stopped;
        break;
    case UMI_EDUCATION_EVIDENCE_RELEASE:
        if (driver->stage != EVIDENCE_STOPPED_STAGE ||
            !EmptyFields(code, observation, checkpoint, severity))
            return UMI_STATUS_PARSE_ERROR;
        driver->stage = EVIDENCE_EMPTY;
        driver->owner = 0U;
        driver->generation = 0U;
        driver->lastObservation = 0U;
        driver->checkpoint = 0U;
        --model->live;
        ++report->released;
        break;
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    model->lastTick = tick;
    return UMI_STATUS_OK;
}

static UmiStatus ValidateChain(const unsigned char *record,
                               unsigned char digest[UMI_EDUCATION_EVIDENCE_HASH_BYTES])
{
    unsigned char observed[UMI_EDUCATION_EVIDENCE_HASH_BYTES];
    UmiSha256 hash;
    UmiSha256Init(&hash);
    UmiStatus status = UmiSha256Update(&hash, digest, UMI_EDUCATION_EVIDENCE_HASH_BYTES);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, record, UMI_EDUCATION_EVIDENCE_METADATA_BYTES);
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&hash, observed);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(observed, record + UMI_EDUCATION_EVIDENCE_METADATA_BYTES,
               UMI_EDUCATION_EVIDENCE_HASH_BYTES) != 0)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(digest, observed, sizeof(observed));
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationEvidenceTraceInspect(const void *bytes, size_t length,
                                            UmiEducationEvidenceReport *report)
{
    static const unsigned char MAGIC[8] = {'U','M','I','E','V','D','1',0};
    if (bytes == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_EDUCATION_EVIDENCE_MAX_FILE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_EDUCATION_EVIDENCE_HEADER_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (memcmp(input, MAGIC, sizeof(MAGIC)) != 0 || Read16(input + 8U) != 1U ||
        Read16(input + 10U) != 0U || Read32(input + 16U) != 0U ||
        Read32(input + 20U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    const uint32_t count = Read32(input + 12U);
    if (count > UMI_EDUCATION_EVIDENCE_MAX_RECORDS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length != UMI_EDUCATION_EVIDENCE_HEADER_BYTES +
                  (size_t)count * UMI_EDUCATION_EVIDENCE_RECORD_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    EvidenceModel model = {0};
    UmiEducationEvidenceReport next = {0};
    unsigned char digest[UMI_EDUCATION_EVIDENCE_HASH_BYTES] = {0};
    next.recordCount = count;
    for (uint32_t i = 0U; i < count; ++i) {
        const unsigned char *record = input + UMI_EDUCATION_EVIDENCE_HEADER_BYTES +
            (size_t)i * UMI_EDUCATION_EVIDENCE_RECORD_BYTES;
        if (Read32(record) != i + 1U || Read16(record + 6U) != 0U)
            return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = ValidateChain(record, digest);
        if (status == UMI_STATUS_OK) status = Apply(&model, &next, record);
        if (status != UMI_STATUS_OK) return status;
    }
    next.lastTick = model.lastTick;
    next.lastObservationId = model.lastObservation;
    next.liveAtEnd = model.live;
    for (size_t i = 0U; i < UMI_EDUCATION_EVIDENCE_MAX_DRIVERS; ++i) {
        if (model.drivers[i].stage == EVIDENCE_FAULTED_STAGE) ++next.faultedAtEnd;
        if (model.drivers[i].stage == EVIDENCE_STOPPED_STAGE) ++next.stoppedAtEnd;
    }
    memcpy(next.lastDigest, digest, sizeof(digest));
    *report = next;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationEvidenceTraceInspectFile(const char *path,
                                                UmiEducationEvidenceReport *report)
{
    if (path == NULL || *path == '\0' || report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *stream = fopen(path, "rb");
    if (stream == NULL) return UMI_STATUS_NOT_FOUND;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationEvidenceReport pending;
    if (fseek(stream, 0L, SEEK_END) == 0) {
        const long end = ftell(stream);
        if (end >= 0L && (unsigned long)end > UMI_EDUCATION_EVIDENCE_MAX_FILE_BYTES)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (end >= 0L && fseek(stream, 0L, SEEK_SET) == 0) {
            const size_t bytes = (size_t)end;
            unsigned char *buffer = malloc(bytes != 0U ? bytes : 1U);
            if (buffer == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                if (fread(buffer, 1U, bytes, stream) == bytes)
                    status = UmiEducationEvidenceTraceInspect(buffer, bytes, &pending);
                free(buffer);
            }
        }
    }
    if (fclose(stream) != 0) return UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *report = pending;
    return status;
}
