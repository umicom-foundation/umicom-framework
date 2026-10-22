/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/corpus.h
 * PURPOSE: Explicitly bounded, read-only corpus import and token-window reads.
 *
 * This laboratory exposes a borrowed immutable byte corpus. It deliberately
 * does not build a vocabulary, train models or replace the existing dataset
 * split and checkpoint components. The caller controls the trust boundary.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_CORPUS_H
#define UMICOM_AI_TRAINING_CORPUS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_CORPUS_MAX_BYTES 1048576U

typedef struct UmiAiCorpus UmiAiCorpus;

/* Creates corpus by copying caller bytes, retaining no external pointer.
 * Zero-length corpora are rejected. Failure leaves *outCorpus unchanged. */
UmiStatus UmiAiCorpusCreate(const uint8_t *bytes, size_t length,
                            UmiAiCorpus **outCorpus);

/* Imports local bytes exactly. Rejects files larger than MAX_BYTES and
 * empty files. No mmap, file mutation, network, text decoding or path search.
 * Caller must select/trust the pathname before using this API. */
UmiStatus UmiAiCorpusLoadFile(const char *path, UmiAiCorpus **outCorpus);
void UmiAiCorpusDestroy(UmiAiCorpus *corpus);
UmiStatus UmiAiCorpusLength(const UmiAiCorpus *corpus, size_t *outLength);

/* Returns a next-token training window at byte offset `start`:
 * inputs[i] = corpus[start+i], targets[i] = corpus[start+i+1].
 * Rejects start/count that would cross the end. Writes no partial output.
 * Tokens use the canonical byte-tokenizer mapping. */
UmiStatus UmiAiCorpusWindow(const UmiAiCorpus *corpus,
                            size_t start, size_t count,
                            uint32_t *inputs, uint32_t *targets,
                            size_t capacity);
/* Stable FNV-1a64 of all immutable imported corpus bytes. Used to detect
 * obvious duplicate documents; NOT a cryptographic signature or security
 * boundary. On failure outFingerprint is unchanged. */
UmiStatus UmiAiCorpusFingerprint(const UmiAiCorpus *corpus,
                                 uint64_t *outFingerprint);
/* SHA-256 over the exact IMMUTABLE bytes already copied into this corpus.
 * Never reopens the path, exposing neither byte pointers nor mutable storage.
 * The caller owns the 32-byte output, which is untouched on failure.
 * This is an identity digest, not evidence of a trusted data publisher. */
#define UMI_AI_CORPUS_SHA256_BYTES 32U
UmiStatus UmiAiCorpusSha256(const UmiAiCorpus *corpus,
                            unsigned char outDigest[UMI_AI_CORPUS_SHA256_BYTES]);
#ifdef __cplusplus
}
#endif
#endif
