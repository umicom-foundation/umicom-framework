/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_evidence_anchor.c
 *
 * PURPOSE:
 *   Check canonical federated evidence, externally anchored inclusion,
 *   altered contents, replay rejection and failure-atomic output handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_evidence_anchor.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "evidence test line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

typedef struct EvidenceFixture {
    UmiRecordChainCheckpoint prior;
    UmiRecordChainCheckpoint tip;
    UmiRecordChainRecord record;
    UmiAiTrainingFederatedEvidenceSummary summary;
    uint8_t payload[160];
    uint8_t digests[96];
    UmiRecordChainMerkleProof proof;
} EvidenceFixture;

static int MakeFixture(EvidenceFixture *f)
{
    uint8_t chainId[32] = {0};
    chainId[0] = 0xA5U;
    chainId[31] = 0x11U;
    for (size_t i = 0U; i < 3U; ++i)
        for (size_t j = 0U; j < 32U; ++j)
            f->digests[i * 32U + j] = (uint8_t)(i * 31U + j + 1U);
    f->summary.roundId = UINT64_C(71);
    f->summary.submittedCount = UINT64_C(4);
    f->summary.acceptedCount = 3U;
    f->summary.baseCheckpointSha256[0] = 0x11U;
    f->summary.datasetPolicySha256[0] = 0x22U;
    f->summary.heldOutEvaluationSha256[0] = 0x33U;
    CHECK(umi_record_chain_start(chainId, &f->prior) == UMI_STATUS_OK);
    f->tip = f->prior;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorCommit(&f->tip,
        &f->summary, f->digests, 3U, &f->record, f->payload) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedEvidenceAnchorProve(f->digests,
        3U, 2U, f->payload, &f->proof) == UMI_STATUS_OK);
    return 0;
}

static int TestHappy(void)
{
    EvidenceFixture f = {0};
    UmiAiTrainingFederatedEvidenceView view = {0};
    UmiRecordChainCheckpoint replay;
    CHECK(MakeFixture(&f) == 0);
    CHECK(f.record.sequence == 1U && f.tip.sequence == 1U);
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, f.payload, 160U, &f.tip, f.digests + 64U,
        &f.proof, &view) == UMI_STATUS_OK);
    CHECK(view.summary.acceptedCount == 3U && view.summary.submittedCount == 4U);
    CHECK(view.summary.roundId == UINT64_C(71));
    CHECK(memcmp(view.merkleRoot, f.payload + 128U, 32U) == 0);
    /* Applying a record at the already accepted tip is a replay, not append. */
    replay = f.tip;
    CHECK(umi_record_chain_accept(&replay, &f.record, f.payload, 160U) != UMI_STATUS_OK);
    CHECK(umi_record_chain_checkpoint_equal(&replay, &f.tip));
    return 0;
}

static int TestTamper(void)
{
    EvidenceFixture f = {0};
    UmiAiTrainingFederatedEvidenceView view;
    UmiAiTrainingFederatedEvidenceView old;
    UmiRecordChainMerkleProof proof;
    uint8_t changed[160], wrongLeaf[32], changedDigests[96];
    CHECK(MakeFixture(&f) == 0);
    memset(&view, 0x5a, sizeof view);
    old = view;
    memcpy(changed, f.payload, sizeof changed);
    changed[96U] ^= 0x80U; /* Evaluation commitment tampering. */
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, changed, 160U, &f.tip, f.digests + 64U,
        &f.proof, &view) != UMI_STATUS_OK);
    CHECK(memcmp(&old, &view, sizeof view) == 0);
    memcpy(wrongLeaf, f.digests + 64U, sizeof wrongLeaf);
    wrongLeaf[0] ^= 0x01U;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, f.payload, 160U, &f.tip, wrongLeaf,
        &f.proof, &view) != UMI_STATUS_OK);
    CHECK(memcmp(&old, &view, sizeof view) == 0);
    proof = f.proof;
    proof.siblings[0][0] ^= 1U;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, f.payload, 160U, &f.tip, f.digests + 64U,
        &proof, &view) != UMI_STATUS_OK);
    CHECK(memcmp(&old, &view, sizeof view) == 0);
    memcpy(changedDigests, f.digests, sizeof changedDigests);
    changedDigests[0] ^= 0x10U;
    proof = f.proof;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorProve(changedDigests, 3U,
        2U, f.payload, &proof) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&proof, &f.proof, sizeof proof) == 0);
    return 0;
}

static int TestRejection(void)
{
    EvidenceFixture f = {0};
    UmiRecordChainCheckpoint before, candidate;
    UmiRecordChainRecord recorded;
    UmiRecordChainRecord preserved;
    uint8_t payload[160], copy[160];
    UmiAiTrainingFederatedEvidenceView view, old;
    CHECK(MakeFixture(&f) == 0);
    candidate = f.prior;
    before = candidate;
    memset(&recorded, 0x6b, sizeof recorded);
    preserved = recorded;
    memset(payload, 0x7d, sizeof payload);
    memcpy(copy, payload, sizeof copy);
    f.summary.acceptedCount = 4U;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorCommit(&candidate,
        &f.summary, f.digests, 3U, &recorded, payload) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&candidate, &before, sizeof before) == 0);
    CHECK(memcmp(&recorded, &preserved, sizeof recorded) == 0);
    CHECK(memcmp(payload, copy, sizeof payload) == 0);
    f.summary.acceptedCount = 3U;
    f.summary.heldOutEvaluationSha256[0] = 0U;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorCommit(&candidate,
        &f.summary, f.digests, 3U, &recorded, payload) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&candidate, &before, sizeof before) == 0);
    CHECK(memcmp(&recorded, &preserved, sizeof recorded) == 0);
    CHECK(memcmp(payload, copy, sizeof payload) == 0);
    memset(&view, 0x17, sizeof view);
    old = view;
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, f.payload, 159U, &f.tip, f.digests + 64U,
        &f.proof, &view) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&old, &view, sizeof view) == 0);
    CHECK(UmiAiTrainingFederatedEvidenceAnchorVerify(&f.prior,
        &f.record, f.payload, 160U, &f.prior, f.digests + 64U,
        &f.proof, &view) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&old, &view, sizeof view) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    int result;
    if (argc != 2) return 2;
    if (strcmp(argv[1], "happy") == 0) result = TestHappy();
    else if (strcmp(argv[1], "tamper") == 0) result = TestTamper();
    else if (strcmp(argv[1], "rejection") == 0) result = TestRejection();
    else return 2;
    if (result == 0) printf("Native federated evidence: %s passed\n", argv[1]);
    return result;
}
