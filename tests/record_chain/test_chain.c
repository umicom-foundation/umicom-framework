/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_chain/test_chain.c
 * PURPOSE: Exercise integrity, ordering, anchored completeness and atomic failure contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/record_chain/archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static void Start(UmiRecordChainCheckpoint *out)
{
    unsigned char id[32];
    for (size_t i = 0U; i < sizeof id; ++i) id[i] = (unsigned char)(i + 1U);
    OK(umi_record_chain_start(id, out));
}
static void Same(const UmiRecordChainCheckpoint *left, const UmiRecordChainCheckpoint *right)
{ CHECK(umi_record_chain_checkpoint_equal(left, right)); }
static void Fixture(UmiRecordChainCheckpoint *start, UmiRecordChainCheckpoint *tip,
    UmiRecordChainRecord records[2], UmiRecordChainInput inputs[2])
{
    Start(start); *tip = *start;
    OK(umi_record_chain_append(tip, 7U, "abc", 3U, &records[0]));
    OK(umi_record_chain_append(tip, 8U, "def", 3U, &records[1]));
    inputs[0] = (UmiRecordChainInput){&records[0], "abc", 3U};
    inputs[1] = (UmiRecordChainInput){&records[1], "def", 3U};
}
int main(int argc, char **argv)
{
    UmiRecordChainCheckpoint start, tip, checkpoint, saved;
    UmiRecordChainRecord records[2], record;
    UmiRecordChainInput inputs[2];
    unsigned char bytes[UMI_RECORD_CHAIN_RECORD_BYTES + 1U];
    const char *name;
    CHECK(argc == 2); name = argv[1];
    Fixture(&start, &tip, records, inputs); checkpoint = saved = start;
    if (!strcmp(name, "vectors")) {
        char hex[65]; UmiSha256Hex(start.digest, hex);
        CHECK(!strcmp(hex, "ca3f8831be7b7f5b288a19307420c9e7818454af00c681ccb24b6b505468d483"));
        UmiSha256Hex(records[0].digest, hex);
        CHECK(!strcmp(hex, "94c8dbe5538ce7170e5af3af67a21802fb81da57dc02f7108ef077f527bfc2a6"));
        OK(umi_record_chain_record_encode(&records[0], bytes, sizeof bytes));
        CHECK(bytes[47] == 1U && bytes[51] == 7U && bytes[59] == 3U);
    } else if (!strcmp(name, "roundtrip")) {
        OK(umi_record_chain_verify(&checkpoint, inputs, 2U, &tip)); Same(&checkpoint, &tip);
        OK(umi_record_chain_checkpoint_encode(&tip, bytes, sizeof bytes));
        OK(umi_record_chain_checkpoint_decode(bytes, UMI_RECORD_CHAIN_CHECKPOINT_BYTES, &checkpoint));
        Same(&checkpoint, &tip);
        OK(umi_record_chain_record_encode(&records[0], bytes, sizeof bytes));
        OK(umi_record_chain_record_decode(bytes, UMI_RECORD_CHAIN_RECORD_BYTES, &record));
        checkpoint = start; OK(umi_record_chain_accept(&checkpoint, &record, "abc", 3U));
        CHECK(checkpoint.sequence == 1U);
    } else if (!strcmp(name, "binary-empty")) {
        const unsigned char data[] = {0, 255, 128, 0, 65};
        OK(umi_record_chain_append(&checkpoint, 1U, NULL, 0U, &record));
        OK(umi_record_chain_accept(&saved, &record, NULL, 0U)); Same(&saved, &checkpoint);
        OK(umi_record_chain_append(&checkpoint, 2U, data, sizeof data, &record));
        OK(umi_record_chain_accept(&saved, &record, data, sizeof data)); Same(&saved, &checkpoint);
    } else if (!strcmp(name, "tamper-payload")) {
        inputs[1].payload = "dEf";
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 2U, &tip) == UMI_STATUS_PARSE_ERROR);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "tamper-fields")) {
        OK(umi_record_chain_record_encode(&records[0], bytes, sizeof bytes));
        for (size_t i = 8U; i < UMI_RECORD_CHAIN_RECORD_BYTES; ++i) {
            UmiRecordChainRecord before;
            memset(&record, 0x6a, sizeof record); memcpy(&before, &record, sizeof record);
            bytes[i] ^= 1U;
            CHECK(umi_record_chain_record_decode(bytes, UMI_RECORD_CHAIN_RECORD_BYTES, &record) == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&before, &record, sizeof record) == 0); bytes[i] ^= 1U;
        }
    } else if (!strcmp(name, "reorder")) {
        UmiRecordChainInput swap = inputs[0]; inputs[0] = inputs[1]; inputs[1] = swap;
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 2U, &tip) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "duplicate")) {
        inputs[1] = inputs[0];
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 2U, &tip) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "truncated")) {
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 1U, &tip) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_verify(&checkpoint, NULL, 0U, &tip) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "anchor")) {
        tip.digest[3] ^= 1U;
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 2U, &tip) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_verify(&checkpoint, inputs, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "namespace")) {
        unsigned char id[32] = {1U}; OK(umi_record_chain_start(id, &checkpoint)); saved = checkpoint;
        CHECK(umi_record_chain_accept(&checkpoint, &records[0], "abc", 3U) == UMI_STATUS_INVALID_STATE);
        Same(&checkpoint, &saved);
        CHECK(!umi_record_chain_checkpoint_equal(&checkpoint, &start));
    } else if (!strcmp(name, "kind")) {
        UmiRecordChainCheckpoint other = start;
        OK(umi_record_chain_append(&checkpoint, 7U, "abc", 3U, &record));
        OK(umi_record_chain_append(&other, 8U, "abc", 3U, &record));
        CHECK(!umi_record_chain_checkpoint_equal(&checkpoint, &other));
        saved = checkpoint;
        CHECK(umi_record_chain_append(&checkpoint, 0U, "abc", 3U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        Same(&checkpoint, &saved);
    } else if (!strcmp(name, "invalid")) {
        unsigned char zero[32] = {0};
        CHECK(umi_record_chain_start(zero, &checkpoint) == UMI_STATUS_INVALID_ARGUMENT);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_append(&checkpoint, 1U, NULL, 1U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_accept(&checkpoint, &records[0], "abc", 2U) == UMI_STATUS_PARSE_ERROR);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_verify(&checkpoint, NULL, 1U, &tip) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_record_chain_checkpoint_equal(NULL, &start));
    } else if (!strcmp(name, "overflow")) {
        checkpoint.sequence = UINT64_MAX; saved = checkpoint;
        memset(&record, 0x5c, sizeof record); UmiRecordChainRecord before;
        memcpy(&before, &record, sizeof record);
        CHECK(umi_record_chain_append(&checkpoint, 1U, "x", 1U, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        Same(&checkpoint, &saved); CHECK(memcmp(&before, &record, sizeof record) == 0);
    } else if (!strcmp(name, "limits")) {
        CHECK(umi_record_chain_append(&checkpoint, 1U, "", UMI_RECORD_CHAIN_PAYLOAD_LIMIT + 1U, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        Same(&checkpoint, &start);
        CHECK(umi_record_chain_verify(&checkpoint, inputs, UMI_RECORD_CHAIN_BATCH_LIMIT + 1U, &tip) == UMI_STATUS_CAPACITY_EXCEEDED);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "short-buffer")) {
        memset(bytes, 0x5a, sizeof bytes);
        CHECK(umi_record_chain_record_encode(&records[0], bytes, UMI_RECORD_CHAIN_RECORD_BYTES - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_record_chain_checkpoint_encode(&tip, bytes, UMI_RECORD_CHAIN_CHECKPOINT_BYTES - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i < sizeof bytes; ++i) CHECK(bytes[i] == 0x5aU);
    } else if (!strcmp(name, "archive-boundaries")) {
        OK(umi_record_chain_record_encode(&records[0], bytes, sizeof bytes));
        for (size_t i = 0U; i < UMI_RECORD_CHAIN_RECORD_BYTES; ++i)
            CHECK(umi_record_chain_record_decode(bytes, i, &record) == UMI_STATUS_PARSE_ERROR);
        CHECK(umi_record_chain_record_decode(bytes, sizeof bytes, &record) == UMI_STATUS_PARSE_ERROR);
        bytes[7] = 2U; CHECK(umi_record_chain_record_decode(bytes, UMI_RECORD_CHAIN_RECORD_BYTES, &record) == UMI_STATUS_PARSE_ERROR);
        OK(umi_record_chain_checkpoint_encode(&start, bytes, sizeof bytes));
        for (size_t i = 0U; i < UMI_RECORD_CHAIN_CHECKPOINT_BYTES; ++i) {
            CHECK(umi_record_chain_checkpoint_decode(bytes, i, &checkpoint) == UMI_STATUS_PARSE_ERROR);
            Same(&checkpoint, &start);
        }
        bytes[79] ^= 1U;
        CHECK(umi_record_chain_checkpoint_decode(bytes, UMI_RECORD_CHAIN_CHECKPOINT_BYTES, &checkpoint) == UMI_STATUS_PARSE_ERROR);
        Same(&checkpoint, &start);
    } else if (!strcmp(name, "resume")) {
        OK(umi_record_chain_accept(&checkpoint, &records[0], "abc", 3U));
        OK(umi_record_chain_checkpoint_encode(&checkpoint, bytes, sizeof bytes));
        OK(umi_record_chain_checkpoint_decode(bytes, UMI_RECORD_CHAIN_CHECKPOINT_BYTES, &saved));
        OK(umi_record_chain_verify(&saved, &inputs[1], 1U, &tip)); Same(&saved, &tip);
    } else if (!strcmp(name, "empty")) {
        OK(umi_record_chain_verify(&checkpoint, NULL, 0U, &start)); Same(&checkpoint, &start);
    } else return 2;
    return 0;
}
