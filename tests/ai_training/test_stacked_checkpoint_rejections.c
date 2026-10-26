/* Umicom Framework | portable checkpoint invalid-byte rejection | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL, *decoded = NULL;
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedCheckpointInfo info, saved;
    size_t bytesNeeded = 0U, written = 891U;
    uint8_t *bytes;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedCheckpointSize(model, &bytesNeeded) == UMI_STATUS_OK);
    bytes = malloc(bytesNeeded);
    CHECK(bytes != NULL);
    memset(bytes, 0xa5, bytesNeeded);
    CHECK(UmiAiTrainingStackedCheckpointExport(model, bytes, 2U,
          &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == 891U && bytes[0] == 0xa5U);
    CHECK(UmiAiTrainingStackedCheckpointExport(model, bytes, bytesNeeded,
          &written) == UMI_STATUS_OK);
    CHECK(written == bytesNeeded);
    memset(&info, 0xa5, sizeof(info));
    saved = info;
    CHECK(UmiAiTrainingStackedCheckpointInspect(bytes, bytesNeeded - 1U,
          &info) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&info, &saved, sizeof(info)) == 0);
    bytes[bytesNeeded / 2U] ^= 0x01U;
    CHECK(UmiAiTrainingStackedCheckpointInspect(bytes, bytesNeeded,
          &info) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiAiTrainingStackedCheckpointDecode(bytes, bytesNeeded,
          &decoded) == UMI_STATUS_PARSE_ERROR);
    CHECK(decoded == NULL);
    bytes[bytesNeeded / 2U] ^= 0x01U;
    CHECK(UmiAiTrainingStackedCheckpointInspect(bytes, bytesNeeded,
          &info) == UMI_STATUS_OK);
    bytes[8U] = 99U;
    CHECK(UmiAiTrainingStackedCheckpointInspect(bytes, bytesNeeded,
          &info) == UMI_STATUS_PARSE_ERROR);
    UmiAiTrainingStackedDecoderDestroy(model);
    free(bytes);
    puts("stacked checkpoint length, checksum, format and output rejection passed");
    return 0;
}
