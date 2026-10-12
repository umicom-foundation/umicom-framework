/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_document.c
 * PURPOSE: Reject lossy recipe imports and preserve existing edits on malformed input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  UmiVideoSequence sequence = Sequence(), loaded = Sequence();
  UmiCreativeAsset *asset = NULL;
  CHECK(UmiVideoSequenceExport(&sequence, NULL, &asset) == UMI_STATUS_OK);
  const void *bytes = NULL;
  size_t size = 0U;
  CHECK(UmiCreativeAssetBytes(asset, &bytes, &size) == UMI_STATUS_OK);
  if (strcmp(argv[1], "roundtrip") == 0) {
    CHECK(UmiVideoSequenceImport(bytes, size, NULL, &loaded) == UMI_STATUS_OK);
    CHECK(strcmp(loaded.title, sequence.title) == 0 && loaded.clip_count == 1U &&
          strcmp(loaded.clips[0].path, VIDEO_PATH) == 0 && loaded.clips[0].begin_ms == 1000U);
  } else if (strcmp(argv[1], "strict") == 0) {
    const char *invalid[] = {"{\"format\":\"umicom-video-sequence\",\"title\":\"x\",\"width\":1280,"
                             "\"height\":720,\"clips\":[],\"extra\":0}",
                             "{\"format\":\"umicom-video-sequence\",\"title\":\"x\",\"width\":1280,"
                             "\"height\":720,\"height\":720}",
                             "{\"format\":\"umicom-video-sequence\",\"title\":\"x\",\"width\":1.5,"
                             "\"height\":720,\"clips\":[]}",
                             "{\"format\":\"umicom-video-sequence\",\"title\":\"x\",\"width\":-2,"
                             "\"height\":720,\"clips\":[]}",
                             "{\"format\":\"umicom-video-sequence\",\"title\":\"x\",\"width\":"
                             "4294967296,\"height\":720,\"clips\":[]}"};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
      CHECK(UmiVideoSequenceImport(invalid[i], strlen(invalid[i]), NULL, &loaded) != UMI_STATUS_OK);
      CHECK(memcmp(&loaded, &sequence, sizeof(sequence)) == 0);
    }
  } else if (strcmp(argv[1], "unchanged") == 0) {
    CHECK(UmiVideoSequenceImport(bytes, size - 3U, NULL, &loaded) != UMI_STATUS_OK);
    CHECK(memcmp(&loaded, &sequence, sizeof(sequence)) == 0);
  } else if (strcmp(argv[1], "cancelled") == 0) {
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    CHECK(UmiVideoSequenceImport(bytes, size, cancel, &loaded) == UMI_STATUS_CANCELLED);
    CHECK(memcmp(&loaded, &sequence, sizeof(sequence)) == 0);
    umi_cancellation_token_destroy(cancel);
  } else
    return 2;
  UmiCreativeAssetDestroy(asset);
  return 0;
}
