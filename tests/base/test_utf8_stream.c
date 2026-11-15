/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/base/test_utf8_stream.c
 *
 * PURPOSE:
 *   Verify incremental UTF-8 scalar ranges, counters and failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/utf8_stream.h"

#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "utf8 assertion on line %d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

static int CheckComplete(const uint8_t *bytes, size_t length)
{
    UmiUtf8Stream stream = {0};
    UmiUtf8StreamInfo info = {0};
    uint8_t output[4] = {0};
    size_t produced = 99U;
    CHECK(UmiUtf8StreamReset(&stream) == UMI_STATUS_OK);
    for (size_t i = 0U; i < length; ++i) {
        memset(output, 0xa5, sizeof(output));
        CHECK(UmiUtf8StreamAppend(&stream, bytes[i], output,
              sizeof(output), &produced) == UMI_STATUS_OK);
        if (i + 1U == length) {
            CHECK(produced == length);
            CHECK(memcmp(output, bytes, length) == 0);
        } else {
            CHECK(produced == 0U);
            CHECK(output[0] == 0xa5U);
        }
    }
    CHECK(UmiUtf8StreamFinish(&stream) == UMI_STATUS_OK);
    CHECK(UmiUtf8StreamInspect(&stream, &info) == UMI_STATUS_OK);
    CHECK(info.completedScalars == 1U && info.completedBytes == length);
    CHECK(info.bytesNeeded == 0U);
    return 0;
}

int main(void)
{
    const uint8_t one[] = {0U};
    const uint8_t two[] = {0xc2U, 0xa2U};
    const uint8_t three[] = {0xe2U, 0x82U, 0xacU};
    const uint8_t four[] = {0xf0U, 0x9fU, 0x92U, 0xa1U};
    UmiUtf8Stream stream = {0};
    uint8_t out[4] = {0xaaU,0xaaU,0xaaU,0xaaU};
    size_t count = 234U;
    UmiUtf8StreamInfo info = {0};
    CHECK(CheckComplete(one, 1U) == 0);
    CHECK(CheckComplete(two, 2U) == 0);
    CHECK(CheckComplete(three, 3U) == 0);
    CHECK(CheckComplete(four, 4U) == 0);

    CHECK(UmiUtf8StreamAppend(&stream, 0xe2U, out, 4U, &count) == UMI_STATUS_OK);
    CHECK(count == 0U && out[0] == 0xaaU);
    CHECK(UmiUtf8StreamFinish(&stream) == UMI_STATUS_INVALID_STATE);
    UmiUtf8Stream before = stream;
    count = 765U;
    CHECK(UmiUtf8StreamAppend(&stream, 0x20U, out, 4U, &count) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&before, &stream, sizeof(stream)) == 0);
    CHECK(count == 765U && out[0] == 0xaaU);
    CHECK(UmiUtf8StreamAppend(&stream, 0x82U, out, 4U, &count) == UMI_STATUS_OK);
    before=stream;count=777U;
    CHECK(UmiUtf8StreamAppend(&stream, 0xacU, out, 2U, &count) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before,&stream,sizeof(stream)) == 0 && count == 777U);
    CHECK(UmiUtf8StreamAppend(&stream, 0xacU, out, 4U, &count) == UMI_STATUS_OK);
    CHECK(count == 3U && UmiUtf8StreamFinish(&stream) == UMI_STATUS_OK);

    CHECK(UmiUtf8StreamReset(&stream) == UMI_STATUS_OK);
    const uint8_t invalidStarts[] = {0x80U,0xc0U,0xc1U,0xf5U,0xffU};
    for (size_t i = 0U; i < sizeof(invalidStarts); ++i) {
        before=stream;
        CHECK(UmiUtf8StreamAppend(&stream, invalidStarts[i], out, 4U,
              &count) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&stream,&before,sizeof(stream)) == 0);
    }
    /* Reject overlong three/four-byte values, surrogates and >U+10FFFF. */
    const uint8_t prefixes[] = {0xe0U,0xedU,0xf0U,0xf4U};
    const uint8_t badSeconds[] = {0x80U,0xa0U,0x80U,0x90U};
    for (size_t i = 0U; i < sizeof(prefixes); ++i) {
        CHECK(UmiUtf8StreamReset(&stream) == UMI_STATUS_OK);
        CHECK(UmiUtf8StreamAppend(&stream, prefixes[i], out, 4U,
              &count) == UMI_STATUS_OK);
        before=stream;
        CHECK(UmiUtf8StreamAppend(&stream, badSeconds[i], out, 4U,
              &count) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&stream,&before,sizeof(stream)) == 0);
    }
    CHECK(UmiUtf8StreamReset(&stream) == UMI_STATUS_OK);
    CHECK(UmiUtf8StreamInspect(&stream,&info) == UMI_STATUS_OK);
    CHECK(info.completedScalars == 0U && info.completedBytes == 0U);

    /* Failure outputs and public-state bounds are part of the shared contract. */
    CHECK(UmiUtf8StreamReset(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamFinish(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamInspect(NULL,&info)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamInspect(&stream,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamAppend(NULL,0U,out,4U,&count)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamAppend(&stream,0U,out,4U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiUtf8StreamAppend(&stream,0U,NULL,1U,&count)==UMI_STATUS_INVALID_ARGUMENT);
    before=stream;count=909U;
    CHECK(UmiUtf8StreamAppend(&stream,'A',NULL,0U,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(count==909U && memcmp(&before,&stream,sizeof stream)==0);
    stream.completedScalars=UINT64_MAX;before=stream;
    CHECK(UmiUtf8StreamAppend(&stream,'A',out,4U,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(count==909U && memcmp(&before,&stream,sizeof stream)==0);
    CHECK(UmiUtf8StreamReset(&stream)==UMI_STATUS_OK);
    stream.completedBytes=UINT64_MAX;before=stream;
    CHECK(UmiUtf8StreamAppend(&stream,'A',out,4U,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before,&stream,sizeof stream)==0);
    CHECK(UmiUtf8StreamReset(&stream)==UMI_STATUS_OK);
    stream.pendingCount=SIZE_MAX;before=stream;
    CHECK(UmiUtf8StreamAppend(&stream,'A',out,4U,&count)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiUtf8StreamInspect(&stream,&info)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiUtf8StreamFinish(&stream)==UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before,&stream,sizeof stream)==0);
    CHECK(UmiUtf8StreamReset(&stream)==UMI_STATUS_OK);
    CHECK(UmiUtf8StreamAppend(&stream,0xe2U,NULL,0U,&count)==UMI_STATUS_OK && count==0U);
    CHECK(UmiUtf8StreamInspect(&stream,&info)==UMI_STATUS_OK && info.bytesNeeded==2U);
    CHECK(UmiUtf8StreamAppend(&stream,0x82U,NULL,0U,&count)==UMI_STATUS_OK && count==0U);
    CHECK(UmiUtf8StreamAppend(&stream,0xacU,out,3U,&count)==UMI_STATUS_OK && count==3U);

    puts("Strict streaming UTF-8 scalar reconstruction and invalid-input atomicity verified");
    return 0;
}
