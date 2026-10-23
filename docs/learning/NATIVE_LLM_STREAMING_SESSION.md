# Pull-Based Native LLM Generation in C23

The Umicom Framework decoder can produce tokens with a deterministic top-p sampler and an inference-only key/value (KV) cache. A pull-based generation session retains the already seeded random state and the cache between calls. A caller receives **one complete token per successful request**, without retaining a borrowed pointer into the decoder.

This interface is designed for applications that need to display partial output, pause between tokens, or cancel generation. It is a teaching-scale interface and does not guarantee faster token throughput.

## Lifecycle

1. Create a decoder from reviewed configuration or load an independently approved checkpoint.
2. Create a generation session with a tokenised prompt, nucleus options and a seed.
3. Request tokens with `UmiAiTrainingGenerationSessionNext` or raw bytes with `UmiAiTrainingGenerationSessionNextByte`.
4. Optionally inspect generated-token and KV-work counters.
5. Destroy the session before changing or destroying the decoder.

The decoder remains owned by its creator. A session owns a separate cache snapshot and sampling state. Calls are synchronous; callers must serialise access to the decoder and session. Updating the decoder after session creation invalidates the session, which must be destroyed and recreated.

## Example: pull a fixed number of tokens

```c
#include "umicom/ai/training/generation_session.h"

UmiAiTrainingGenerationSession *session = NULL;
UmiAiTrainingNucleusOptions policy = {
    .temperature = 0.85F, .topK = 12U, .topP = 0.90F
};
/* model is a previously created, caller-owned decoder. */
UmiStatus status = UmiAiTrainingGenerationSessionCreate(
    model, promptIds, promptCount, &policy, 1234U, &session);
if (status == UMI_STATUS_OK) {
    for (size_t i = 0U; i < 12U; ++i) {
        size_t tokenId = 0U;
        status = UmiAiTrainingGenerationSessionNext(session, &tokenId);
        if (status != UMI_STATUS_OK) break;
        /* Display or buffer tokenId under the application's own policy. */
    }
}
UmiAiTrainingGenerationSessionDestroy(session);
```

A caller can cancel by destroying the session. There is no background worker, callback or network service in this API. Retrieving a token does not execute it as code.

## Mathematical compatibility

The initial context has the same full-length, BOS-left-padded representation as the existing one-shot cached generator. Every later pull first appends the previously emitted token to the cache, then samples from the updated last-token logits. The random generator is **seeded once per session**, not once per call. Splitting a 24-token request into 3, 7 and 14 pulls therefore produces the same 24 token IDs as one 24-token request with the same initial inputs.

The current decoder uses learned absolute position embeddings. When a full context window moves, older keys and values correspond to different absolute positions. The qualified cache therefore rebuilds the window. Although a generation session avoids recreating the *session and weight snapshot* on each UI pull, it is not proof of a full-window inference speedup.

## Failure, output and resource boundaries

- A failed `Create` leaves the caller's output pointer unchanged. Invalid arguments and exhausted token budgets leave successful session output untouched.
- A numerical fault after the KV cache has advanced makes that session terminal; destroy it rather than retrying and risking a skipped position.
- `NextByte` is supported only for the canonical reversible byte vocabulary (260 IDs). Its output may include embedded NUL or invalid UTF-8, so it is not a C string.
- The educational budget is at most 256 generated tokens per session, with the existing bounded decoder context and vocabulary limits.
- Metrics are *computed rows*, *window rebuilds* and *retained tokens*. They are not CPU or wall-clock performance measurements.

## Regression tests

From the native laboratory's CMake build directory, run:

```text
ctest --test-dir <native-llm-build> -R "^framework\\.ai\\.training\\.generation_session_" --output-on-failure --no-tests=error
```

The parity regression uses the **real decoder and real KV cache** and compares greedy, top-K and nucleus generation with prompts of varying lengths. The rejection regression covers invalid input, missing outputs, a stale decoder and the 256-token ceiling. Neither test authorises generated code to run.
