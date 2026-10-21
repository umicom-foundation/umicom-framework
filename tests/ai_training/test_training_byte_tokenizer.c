/* Umicom Framework | Sammy Hegab / Umicom Foundation | MIT
 * Verify the native tokenizer contract, reversibility and failure atomicity. */
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdio.h>
#include <string.h>
#define CHECK(p) do { if (!(p)) { fprintf(stderr, "FAIL line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    uint8_t bytes[256], decoded[256];
    uint32_t ids[256];
    size_t count = 999U;
    for (size_t i = 0U; i < 256U; ++i) bytes[i] = (uint8_t)i;
    CHECK(UmiAiByteEncode(bytes, 256U, ids, 256U, &count) == UMI_STATUS_OK);
    CHECK(count == 256U && ids[0] == 4U && ids[255] == 259U);
    CHECK(UmiAiByteDecode(ids, 256U, decoded, 256U, &count) == UMI_STATUS_OK);
    CHECK(memcmp(bytes, decoded, 256U) == 0);
    count = 998U;
    CHECK(UmiAiByteEncode(bytes, 256U, ids, 255U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(count == 998U);
    decoded[0] = 42U; ids[2] = UMI_AI_BYTE_BOS;
    CHECK(UmiAiByteDecode(ids, 256U, decoded, 256U, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(decoded[0] == 42U && count == 998U);
    CHECK(UmiAiByteEncode(NULL, 0U, NULL, 0U, &count) == UMI_STATUS_OK);
    CHECK(count == 0U);
    CHECK(UmiAiByteDecode(NULL, 0U, NULL, 0U, &count) == UMI_STATUS_OK);
    CHECK(count == 0U);
    CHECK(UmiAiByteDecode(ids, 1U, NULL, 10U, &count) == UMI_STATUS_INVALID_ARGUMENT);
    puts("PASS: byte tokenizer round-trip and rejection");
    return 0;
}
