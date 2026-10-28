/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_wire.c
 *
 * PURPOSE:
 *   Qualify the canonical C23 federated wire codec, unchanged-on-rejection
 *   semantics, Framework message envelope and real job arithmetic integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_wire.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

static UmiAiTrainingFederatedWireRecord Fixture(unsigned worker,
                                                 UmiAiTrainingFederatedWireKind kind,
                                                 const float *delta)
{
    UmiAiTrainingFederatedWireRecord record = {0};
    record.kind = kind;
    record.roundId = UINT64_C(41);
    record.sequence = (uint64_t)worker;
    record.assignment.contributorId[0] = (uint8_t)worker;
    record.assignment.baseCheckpointSha256[0] = 88U;
    record.assignment.datasetCommitmentSha256[0] = (uint8_t)(100U + worker);
    record.assignment.dataPolicySha256[0] = 99U;
    record.assignment.distinctSampleCount = worker == 1U ? 100U :
        (worker == 2U ? 250U : 150U);
    record.assignment.localSteps = UINT64_C(2);
    record.assignment.declaredTransferBytes = 256U;
    record.delta = delta;
    record.deltaCount = kind == UMI_AI_FEDERATED_WIRE_RESULT ? 1U : 0U;
    return record;
}
static UmiAiTrainingFederatedJobConfig Config(void)
{
    UmiAiTrainingFederatedJobConfig config = {0};
    config.round.parameterCount = 1U;
    config.round.maximumContributors = 3U;
    config.round.maximumDeltaL2Norm = 1.0F;
    config.round.baseCheckpointSha256[0] = 88U;
    config.roundId = UINT64_C(41);
    config.maximumLocalSteps = UINT64_C(8);
    config.maximumSamplesPerAssignment = UINT64_C(1000);
    config.maximumTransferBytesPerAssignment = 1024U;
    config.maximumReservedTransferBytes = 1024U;
    config.dataPolicySha256[0] = 99U;
    return config;
}
static int EqualAssignment(const UmiAiTrainingFederatedAssignment *a,
                           const UmiAiTrainingFederatedAssignment *b)
{
    return memcmp(a->contributorId, b->contributorId, 32U) == 0 &&
           memcmp(a->baseCheckpointSha256, b->baseCheckpointSha256, 32U) == 0 &&
           memcmp(a->datasetCommitmentSha256, b->datasetCommitmentSha256, 32U) == 0 &&
           memcmp(a->dataPolicySha256, b->dataPolicySha256, 32U) == 0 &&
           a->distinctSampleCount == b->distinctSampleCount &&
           a->localSteps == b->localSteps &&
           a->declaredTransferBytes == b->declaredTransferBytes;
}
static int TestRoundtrip(void)
{
    const float delta[] = {0.05F};
    for (unsigned kind = 1U; kind <= 2U; ++kind) {
        UmiAiTrainingFederatedWireRecord src = Fixture(1U,
            (UmiAiTrainingFederatedWireKind)kind,
            kind == 2U ? delta : NULL);
        uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
        size_t needed = 0U, written = 0U;
        REQUIRE(UmiAiTrainingFederatedWireMeasure(&src, &needed) == UMI_STATUS_OK);
        REQUIRE(needed == (kind == 1U ? 216U : 220U));
        REQUIRE(UmiAiTrainingFederatedWireEncode(&src, frame,
            sizeof(frame), &written) == UMI_STATUS_OK);
        REQUIRE(written == needed);
        REQUIRE(frame[0] == (uint8_t)'U' && frame[3] == (uint8_t)'W');
        float output[] = {-99.0F};
        UmiAiTrainingFederatedWireRecord readback = {0};
        REQUIRE(UmiAiTrainingFederatedWireDecode(frame, written,
            &readback, output, 1U) == UMI_STATUS_OK);
        REQUIRE(readback.kind == src.kind && readback.roundId == 41U &&
                readback.sequence == 1U && readback.deltaCount == src.deltaCount);
        REQUIRE(EqualAssignment(&readback.assignment, &src.assignment));
        if (kind == 2U) {
            REQUIRE(output[0] == delta[0]);
            REQUIRE(readback.delta == output);
        } else REQUIRE(output[0] == -99.0F && readback.delta == NULL);
    }
    return 0;
}
static int TestTamper(void)
{
    float delta = 0.08F;
    UmiAiTrainingFederatedWireRecord src = Fixture(3U,
        UMI_AI_FEDERATED_WIRE_RESULT, &delta);
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    size_t count = 0U;
    REQUIRE(UmiAiTrainingFederatedWireEncode(&src, frame,
        sizeof(frame), &count) == UMI_STATUS_OK);
    float output = -99.0F;
    UmiAiTrainingFederatedWireRecord record;
    memset(&record, 0x5a, sizeof(record));
    UmiAiTrainingFederatedWireRecord original = record;
    frame[92] ^= 1U; /* Dataset commitment modified after hashing. */
    REQUIRE(UmiAiTrainingFederatedWireDecode(frame, count, &record,
        &output, 1U) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(output == -99.0F && memcmp(&record,&original,sizeof(record)) == 0);
    return 0;
}
static int TestCapacity(void)
{
    float delta[2] = {0.05F, 0.10F};
    UmiAiTrainingFederatedWireRecord src = Fixture(1U,
        UMI_AI_FEDERATED_WIRE_RESULT, delta);
    src.deltaCount = 2U;
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES];
    memset(frame, 0x77, sizeof(frame));
    size_t written = 12345U;
    REQUIRE(UmiAiTrainingFederatedWireEncode(&src, frame, 1U,
        &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(frame[0] == 0x77U && written == 12345U);
    REQUIRE(UmiAiTrainingFederatedWireEncode(&src, frame, sizeof(frame),
        &written) == UMI_STATUS_OK);
    float one = -11.0F;
    UmiAiTrainingFederatedWireRecord decoded = {0};
    decoded.roundId = 888U;
    REQUIRE(UmiAiTrainingFederatedWireDecode(frame, written,
        &decoded, &one, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(decoded.roundId == 888U && one == -11.0F);
    for (size_t len = 0U; len < written; ++len) {
        REQUIRE(UmiAiTrainingFederatedWireDecode(frame,len,&decoded,
            delta,2U) != UMI_STATUS_OK);
        REQUIRE(decoded.roundId == 888U);
    }
    return 0;
}
static int TestInvalidFields(void)
{
    const float invalid = NAN;
    UmiAiTrainingFederatedWireRecord src = Fixture(1U,
        UMI_AI_FEDERATED_WIRE_RESULT, &invalid);
    size_t unchanged = 19U;
    REQUIRE(UmiAiTrainingFederatedWireMeasure(&src,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(unchanged == 19U);
    src.delta = NULL;
    REQUIRE(UmiAiTrainingFederatedWireMeasure(&src,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    src.kind = UMI_AI_FEDERATED_WIRE_ASSIGNMENT;
    src.deltaCount = 0U;
    REQUIRE(UmiAiTrainingFederatedWireMeasure(&src,
        &unchanged) == UMI_STATUS_OK);
    src.sequence = 0U;
    REQUIRE(UmiAiTrainingFederatedWireMeasure(&src,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    src.sequence = 1U;
    src.assignment.dataPolicySha256[0] = 0U;
    REQUIRE(UmiAiTrainingFederatedWireMeasure(&src,
        &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static int TestFinite(void)
{
    float delta = 0.08F;
    UmiAiTrainingFederatedWireRecord src = Fixture(3U,
        UMI_AI_FEDERATED_WIRE_RESULT, &delta);
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    size_t count = 0U;
    REQUIRE(UmiAiTrainingFederatedWireEncode(&src, frame,
        sizeof(frame), &count) == UMI_STATUS_OK);
    /* Deliberately make a NaN but leave its old checksum. Must reject. */
    frame[184] = 0x7fU; frame[185] = 0xc0U;
    float output = -4.0F;
    UmiAiTrainingFederatedWireRecord decoded = {0};
    REQUIRE(UmiAiTrainingFederatedWireDecode(frame,count,&decoded,
        &output,1U) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(output == -4.0F);
    return 0;
}
static int TestIdentity(void)
{
    float delta = 0.05F;
    UmiAiTrainingFederatedWireRecord src = Fixture(1U,
        UMI_AI_FEDERATED_WIRE_RESULT, &delta);
    REQUIRE(UmiAiTrainingFederatedWireMatchResult(&src,
        &src.assignment,41U,1U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingFederatedWireMatchResult(&src,
        &src.assignment,41U,2U) == UMI_STATUS_INVALID_STATE);
    REQUIRE(UmiAiTrainingFederatedWireMatchResult(&src,
        &src.assignment,42U,1U) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingFederatedAssignment other = src.assignment;
    other.dataPolicySha256[0] ^= 1U;
    REQUIRE(UmiAiTrainingFederatedWireMatchResult(&src,
        &other,41U,1U) == UMI_STATUS_INVALID_STATE);
    return 0;
}
static int TestEnvelope(void)
{
    UmiAiTrainingFederatedWireRecord src = Fixture(2U,
        UMI_AI_FEDERATED_WIRE_ASSIGNMENT, NULL);
    uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    size_t count = 0U;
    REQUIRE(UmiAiTrainingFederatedWireEncode(&src,frame,sizeof(frame),
        &count) == UMI_STATUS_OK);
    UmiMessageEnvelope env = {0};
    REQUIRE(UmiAiTrainingFederatedWireBindEnvelope(frame,count,
        &env) == UMI_STATUS_OK);
    REQUIRE(env.kind == UMI_MESSAGE_JOB && env.payload_data == frame &&
            env.payload_size == count && env.sequence == 2U &&
            env.correlation_id == 41U && env.schema_id != NULL &&
            env.structure_size == (uint32_t)sizeof(env));
    return 0;
}
static int TestAggregation(void)
{
    UmiAiTrainingFederatedJobConfig config = Config();
    UmiAiTrainingFederatedJob *job = NULL;
    REQUIRE(UmiAiTrainingFederatedJobCreate(&config,&job) == UMI_STATUS_OK);
    const float values[3] = {0.05F,-0.01F,0.08F};
    UmiAiTrainingFederatedTicket ticket[3];
    for (unsigned worker=1U;worker<=3U;++worker) {
        UmiAiTrainingFederatedWireRecord assign = Fixture(worker,
            UMI_AI_FEDERATED_WIRE_ASSIGNMENT,NULL);
        REQUIRE(UmiAiTrainingFederatedJobAdmit(job,&assign.assignment,
            &ticket[worker-1U]) == UMI_STATUS_OK);
        UmiAiTrainingFederatedWireRecord result = Fixture(worker,
            UMI_AI_FEDERATED_WIRE_RESULT,&values[worker-1U]);
        uint8_t frame[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
        size_t count = 0U;
        REQUIRE(UmiAiTrainingFederatedWireEncode(&result,frame,
            sizeof(frame),&count) == UMI_STATUS_OK);
        float delta = 0.0F;
        UmiAiTrainingFederatedWireRecord decoded = {0};
        REQUIRE(UmiAiTrainingFederatedWireDecode(frame,count,&decoded,
            &delta,1U) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingFederatedWireMatchResult(&decoded,
            &assign.assignment,41U,ticket[worker-1U].serial) == UMI_STATUS_OK);
        UmiAiTrainingFederatedContribution contribution = {0};
        REQUIRE(UmiAiTrainingFederatedWireContribution(&decoded,
            &contribution) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingFederatedJobComplete(job,
            ticket[worker-1U],&contribution) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingFederatedJobComplete(job,
            ticket[worker-1U],&contribution) == UMI_STATUS_INVALID_STATE);
    }
    float mean = -8.0F;
    REQUIRE(UmiAiTrainingFederatedJobFinalise(job,&mean,1U,
        NULL) == UMI_STATUS_OK);
    REQUIRE(fabsf(mean-0.029F) <= 0.000001F);
    UmiAiTrainingFederatedJobReport report = {0};
    REQUIRE(UmiAiTrainingFederatedJobInspect(job,&report) == UMI_STATUS_OK);
    REQUIRE(report.completedAssignments == 3U && report.pendingAssignments == 0U &&
            report.reservedTransferBytes == 0U && report.acceptedDistinctSamples == 500U);
    UmiAiTrainingFederatedJobDestroy(job);
    return 0;
}
int main(int argc,char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    int outcome = 2;
    if (strcmp(name,"roundtrip") == 0) outcome=TestRoundtrip();
    else if (strcmp(name,"tamper") == 0) outcome=TestTamper();
    else if (strcmp(name,"short_capacity") == 0) outcome=TestCapacity();
    else if (strcmp(name,"invalid_fields") == 0) outcome=TestInvalidFields();
    else if (strcmp(name,"finite") == 0) outcome=TestFinite();
    else if (strcmp(name,"identity") == 0) outcome=TestIdentity();
    else if (strcmp(name,"envelope") == 0) outcome=TestEnvelope();
    else if (strcmp(name,"aggregation") == 0) outcome=TestAggregation();
    if (outcome == 0) printf("federated wire %s: passed\n", name);
    return outcome;
}
