/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_authenticated_frame.c
 *
 * PURPOSE:
 *   Exercise RFC 4231 HMAC, authenticated portable frames, independent job
 *   ticket replay rejection, and failure-atomic outputs using native C23.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_authenticated_frame.h"
#include "umicom/security/hmac_sha256.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

static const uint8_t TEST_SECRET[32] = {
    0x53,0x59,0x4e,0x54,0x48,0x45,0x54,0x49,
    0x43,0x2d,0x4c,0x45,0x41,0x52,0x4e,0x49,
    0x4e,0x47,0x2d,0x4e,0x4f,0x54,0x2d,0x46,
    0x4f,0x52,0x2d,0x52,0x45,0x41,0x4c,0x21
};

static UmiAiTrainingFederatedWireRecord Fixture(
    UmiAiTrainingFederatedWireKind kind, const float *delta, size_t count)
{
    UmiAiTrainingFederatedWireRecord record = {0};
    record.kind = kind;
    record.roundId = UINT64_C(84);
    record.sequence = UINT64_C(3);
    memset(record.assignment.contributorId, 0x31,
           sizeof(record.assignment.contributorId));
    memset(record.assignment.baseCheckpointSha256, 0x32,
           sizeof(record.assignment.baseCheckpointSha256));
    memset(record.assignment.datasetCommitmentSha256, 0x33,
           sizeof(record.assignment.datasetCommitmentSha256));
    memset(record.assignment.dataPolicySha256, 0x34,
           sizeof(record.assignment.dataPolicySha256));
    record.assignment.distinctSampleCount = UINT64_C(100);
    record.assignment.localSteps = UINT64_C(2);
    record.assignment.declaredTransferBytes = 512U;
    record.delta = delta;
    record.deltaCount = count;
    return record;
}

static UmiAiTrainingFederatedJobConfig JobConfig(void)
{
    UmiAiTrainingFederatedJobConfig cfg = {0};
    cfg.round.parameterCount = 2U;
    cfg.round.maximumContributors = 1U;
    cfg.round.maximumDeltaL2Norm = 2.0F;
    memset(cfg.round.baseCheckpointSha256, 0x32,
           sizeof(cfg.round.baseCheckpointSha256));
    cfg.roundId = UINT64_C(84);
    cfg.maximumLocalSteps = 10U;
    cfg.maximumSamplesPerAssignment = 1000U;
    cfg.maximumTransferBytesPerAssignment = 1024U;
    cfg.maximumReservedTransferBytes = 1024U;
    memset(cfg.dataPolicySha256, 0x34, sizeof(cfg.dataPolicySha256));
    return cfg;
}

static int ParseHex(const char *hex, uint8_t bytes[32])
{
    for (size_t i=0U; i<32U; ++i) {
        unsigned value = 0U;
        if (sscanf(hex + 2U * i, "%2x", &value) != 1) return 0;
        bytes[i] = (uint8_t)value;
    }
    return 1;
}

static int TestHmacVectors(void)
{
    /* RFC 4231 test cases 1, 2, 3 and 6 (long-key prehash). */
    const uint8_t key1[20] = {
        0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,
        0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b};
    const char *txt1 = "Hi There";
    uint8_t result[32] = {0}, expected[32] = {0};
    REQUIRE(ParseHex("b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7", expected));
    REQUIRE(UmiSecurityHmacSha256Compute(key1,sizeof(key1),
        (const uint8_t *)txt1, strlen(txt1), result) == UMI_STATUS_OK);
    REQUIRE(memcmp(result,expected,sizeof(result)) == 0);
    REQUIRE(UmiSecurityHmacSha256Verify(key1,sizeof(key1),
        (const uint8_t *)txt1,strlen(txt1),expected) == UMI_STATUS_OK);
    expected[0] ^= 1U;
    REQUIRE(UmiSecurityHmacSha256Verify(key1,sizeof(key1),
        (const uint8_t *)txt1,strlen(txt1),expected) == UMI_STATUS_PERMISSION_DENIED);

    const char *key2 = "Jefe", *txt2 = "what do ya want for nothing?";
    REQUIRE(ParseHex("5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843",expected));
    REQUIRE(UmiSecurityHmacSha256Compute((const uint8_t *)key2,strlen(key2),
        (const uint8_t *)txt2,strlen(txt2),result) == UMI_STATUS_OK);
    REQUIRE(memcmp(result,expected,32U) == 0);
    uint8_t key3[20], msg3[50];
    memset(key3, 0xaa, sizeof(key3));
    memset(msg3, 0xdd, sizeof(msg3));
    REQUIRE(ParseHex("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe",expected));
    REQUIRE(UmiSecurityHmacSha256Compute(key3,sizeof(key3),msg3,sizeof(msg3),
                                         result) == UMI_STATUS_OK);
    REQUIRE(memcmp(result,expected,32U) == 0);
    uint8_t key6[131];
    memset(key6, 0xaa, sizeof(key6));
    const char *msg6 = "Test Using Larger Than Block-Size Key - Hash Key First";
    REQUIRE(ParseHex("60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54",expected));
    REQUIRE(UmiSecurityHmacSha256Compute(key6,sizeof(key6),
        (const uint8_t *)msg6,strlen(msg6),result) == UMI_STATUS_OK);
    REQUIRE(memcmp(result,expected,32U) == 0);
    memset(result,0xa5,sizeof(result));
    REQUIRE(UmiSecurityHmacSha256Compute(NULL,0U,(const uint8_t *)txt1,
        strlen(txt1),result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(result[0] == 0xa5U);
    return 0;
}

static int MakeAuth(UmiAiTrainingFederatedWireKind kind, float *values,
                    size_t count, uint8_t *frame, size_t *outBytes)
{
    uint8_t wire[UMI_AI_FEDERATED_WIRE_MAX_BYTES];
    size_t wireBytes=0U;
    const UmiAiTrainingFederatedWireRecord record = Fixture(kind,values,count);
    REQUIRE(UmiAiTrainingFederatedWireEncode(&record,wire,sizeof(wire),
        &wireBytes) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingFederatedAuthSeal(wire,wireBytes,5U,TEST_SECRET,
        sizeof(TEST_SECRET),frame,UMI_AI_FEDERATED_AUTH_MAX_BYTES,
        outBytes) == UMI_STATUS_OK);
    REQUIRE(*outBytes == 20U + wireBytes + 32U);
    return 0;
}

static int TestRoundtrip(void)
{
    const float vals[2] = {0.25F,-0.125F};
    uint8_t frame[UMI_AI_FEDERATED_AUTH_MAX_BYTES];
    size_t bytes=0U;
    REQUIRE(MakeAuth(UMI_AI_FEDERATED_WIRE_RESULT,(float *)vals,2U,
                     frame,&bytes) == 0);
    UmiAiTrainingFederatedWireRecord decoded = {0};
    float got[2] = {0};
    REQUIRE(UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
        sizeof(TEST_SECRET),&decoded,got,2U) == UMI_STATUS_OK);
    REQUIRE(decoded.roundId == 84U && decoded.sequence == 3U &&
            decoded.delta == got && decoded.deltaCount == 2U &&
            got[0] == vals[0] && got[1] == vals[1]);
    REQUIRE(decoded.assignment.distinctSampleCount == 100U);
    REQUIRE(MakeAuth(UMI_AI_FEDERATED_WIRE_ASSIGNMENT,NULL,0U,
                     frame,&bytes) == 0);
    got[0] = -9.0F;
    REQUIRE(UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
        sizeof(TEST_SECRET),&decoded,got,2U) == UMI_STATUS_OK);
    REQUIRE(decoded.kind == UMI_AI_FEDERATED_WIRE_ASSIGNMENT &&
            decoded.deltaCount == 0U && got[0] == -9.0F);
    return 0;
}

static int IsUnchanged(UmiAiTrainingFederatedWireRecord *record,
                       const UmiAiTrainingFederatedWireRecord *before,
                       const float *delta)
{
    return delta[0] == -7.0F && delta[1] == -8.0F &&
           memcmp(record,before,sizeof(*record)) == 0;
}
static int TestReject(const char *mode)
{
    float values[2] = {0.125F,0.5F};
    uint8_t frame[UMI_AI_FEDERATED_AUTH_MAX_BYTES];
    size_t bytes=0U;
    REQUIRE(MakeAuth(UMI_AI_FEDERATED_WIRE_RESULT,values,2U,
                     frame,&bytes) == 0);
    float delta[2] = {-7.0F,-8.0F};
    UmiAiTrainingFederatedWireRecord decoded;
    memset(&decoded,0x5a,sizeof(decoded));
    UmiAiTrainingFederatedWireRecord original = decoded;
    UmiStatus status = UMI_STATUS_OK;
    if (strcmp(mode,"mac_tamper") == 0) {
        frame[UMI_AI_FEDERATED_AUTH_HEADER_BYTES + 90U] ^= 1U;
        status = UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
            sizeof(TEST_SECRET),&decoded,delta,2U);
        REQUIRE(status == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(mode,"wrong_key") == 0) {
        uint8_t wrong[32];
        memcpy(wrong,TEST_SECRET,sizeof(wrong)); wrong[0]^=1U;
        status = UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,wrong,
            sizeof(wrong),&decoded,delta,2U);
        REQUIRE(status == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(mode,"wrong_epoch") == 0) {
        status = UmiAiTrainingFederatedAuthOpen(frame,bytes,6U,TEST_SECRET,
            sizeof(TEST_SECRET),&decoded,delta,2U);
        REQUIRE(status == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(mode,"invalid_wire") == 0) {
        /* An authorised key does not make an invalid checksum valid.
         * Forge only the MAC after changing the inner wire checksum. */
        frame[UMI_AI_FEDERATED_AUTH_HEADER_BYTES + 92U] ^= 1U;
        REQUIRE(UmiSecurityHmacSha256Compute(TEST_SECRET,sizeof(TEST_SECRET),
            frame,bytes-32U,frame+bytes-32U) == UMI_STATUS_OK);
        status = UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
            sizeof(TEST_SECRET),&decoded,delta,2U);
        REQUIRE(status == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(mode,"short_capacity") == 0) {
        status = UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
            sizeof(TEST_SECRET),&decoded,delta,1U);
        REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED);
        uint8_t output[UMI_AI_FEDERATED_AUTH_MAX_BYTES];
        memset(output,0xa5,sizeof(output));
        size_t count=987U;
        const UmiAiTrainingFederatedWireRecord source = Fixture(
            UMI_AI_FEDERATED_WIRE_RESULT, values, 2U);
        uint8_t wire[UMI_AI_FEDERATED_WIRE_MAX_BYTES];
        size_t wireBytes=0U;
        REQUIRE(UmiAiTrainingFederatedWireEncode(&source,wire,sizeof(wire),
            &wireBytes) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingFederatedAuthSeal(wire,wireBytes,5U,TEST_SECRET,
            sizeof(TEST_SECRET),output,1U,&count) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(output[0] == 0xa5U && count == 987U);
    } else return 1;
    REQUIRE(IsUnchanged(&decoded,&original,delta));
    return 0;
}
static int TestReplay(void)
{
    float values[2] = {0.25F, -0.125F};
    uint8_t frame[UMI_AI_FEDERATED_AUTH_MAX_BYTES];
    size_t bytes=0U;
    REQUIRE(MakeAuth(UMI_AI_FEDERATED_WIRE_RESULT,values,2U,
                     frame,&bytes) == 0);
    UmiAiTrainingFederatedJobConfig cfg=JobConfig();
    UmiAiTrainingFederatedJob *job=NULL;
    REQUIRE(UmiAiTrainingFederatedJobCreate(&cfg,&job) == UMI_STATUS_OK);
    UmiAiTrainingFederatedAssignment adm = Fixture(
        UMI_AI_FEDERATED_WIRE_ASSIGNMENT,NULL,0U).assignment;
    UmiAiTrainingFederatedTicket ticket = {0};
    REQUIRE(UmiAiTrainingFederatedJobAdmit(job,&adm,&ticket) == UMI_STATUS_OK);
    REQUIRE(ticket.serial == 1U);
    /* The token's local serial is independent of the published sequence.
     * Only the coordinator's private admission table joins them. */
    UmiAiTrainingFederatedWireRecord verified={0};
    float delta[2]={0};
    REQUIRE(UmiAiTrainingFederatedAuthOpen(frame,bytes,5U,TEST_SECRET,
        sizeof(TEST_SECRET),&verified,delta,2U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingFederatedWireMatchResult(&verified,&adm,84U,3U)
            == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution contribution={0};
    REQUIRE(UmiAiTrainingFederatedWireContribution(&verified,
        &contribution) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingFederatedJobComplete(job,ticket,&contribution)
            == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingFederatedJobComplete(job,ticket,&contribution)
            == UMI_STATUS_INVALID_STATE);
    float mean[2] = {0};
    REQUIRE(UmiAiTrainingFederatedJobFinalise(job,mean,2U,NULL)
            == UMI_STATUS_OK);
    REQUIRE(mean[0]==values[0] && mean[1]==values[1]);
    UmiAiTrainingFederatedJobDestroy(job);
    return 0;
}
static int TestEnvelope(void)
{
    float value[2]={0.25F,0.5F};
    uint8_t frame[UMI_AI_FEDERATED_AUTH_MAX_BYTES];
    size_t count=0U;
    REQUIRE(MakeAuth(UMI_AI_FEDERATED_WIRE_RESULT,value,2U,
        frame,&count) == 0);
    UmiMessageEnvelope envelope={0};
    REQUIRE(UmiAiTrainingFederatedAuthBindEnvelope(frame,count,5U,
        TEST_SECRET,sizeof(TEST_SECRET),&envelope) == UMI_STATUS_OK);
    REQUIRE(envelope.kind==UMI_MESSAGE_REPLY && envelope.payload_data==frame &&
        envelope.payload_size==count && envelope.correlation_id==84U &&
        envelope.sequence==3U && envelope.name!=NULL &&
        envelope.schema_id!=NULL);
    return 0;
}
int main(int argc,char **argv)
{
    if (argc != 2) return 2;
    int status=2;
    if (strcmp(argv[1],"hmac_rfc4231") == 0) status=TestHmacVectors();
    else if (strcmp(argv[1],"auth_roundtrip") == 0) status=TestRoundtrip();
    else if (strcmp(argv[1],"replay_rejection") == 0) status=TestReplay();
    else if (strcmp(argv[1],"envelope") == 0) status=TestEnvelope();
    else if (strcmp(argv[1],"mac_tamper") == 0 ||
             strcmp(argv[1],"wrong_key") == 0 ||
             strcmp(argv[1],"wrong_epoch") == 0 ||
             strcmp(argv[1],"invalid_wire") == 0 ||
             strcmp(argv[1],"short_capacity") == 0)
        status=TestReject(argv[1]);
    if (status == 0) printf("federated authenticated frame: %s passed\n",argv[1]);
    return status;
}
