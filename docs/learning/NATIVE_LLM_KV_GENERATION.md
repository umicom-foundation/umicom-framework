# Native C23 generation with a qualified KV cache

Umicom Framework provides an introductory decoder-only language model and a reusable inference-only key/value (KV) cache. The optional cached-generation adapter connects this already-qualified cache to the existing top-p nucleus sampler. It does not create another model implementation, alter training, or change checkpoint bytes.

## Why the first cache prefill uses a full context

The original decoder generation service uses a fixed context length and pads short prompts on the **left** with the special beginning-of-sequence (`BOS`) token. If the context length is eight and the prompt consists of three token IDs `A, B, C`, the first decoder query receives:

```
BOS BOS BOS BOS BOS A B C
```

The transformer uses *learned absolute position embeddings*. Prefilling only `A, B, C` would place them at positions 0–2 instead of 5–7, changing their representations. To retain the original output contract, cached generation first prefills the complete eight-token context.

## Rolling-window correctness

After selecting a new token, the oldest token leaves a full window. The remaining tokens move to different absolute positions. Keys and values calculated at the old positions cannot be reused unchanged. The existing Framework KV cache therefore rebuilds the window on rollover.

The adapter is intended to establish **correctness and a stable reusable API**, not a performance improvement. With an eight-token context and 20 generated tokens, the expected work is eight prefill rows plus 19 complete eight-row rebuilds: 160 computed rows. The reported work counters are not elapsed time or a guarantee of faster inference.

## Public C23 interfaces

Include `umicom/ai/training/generation_cache.h`, then use one of:

- `UmiAiTrainingDecoderGenerateCachedNucleus` for token IDs.
- `UmiAiTrainingDecoderGenerateCachedBytesNucleus` for reversible binary bytes.

Pass the model, prompt, `UmiAiTrainingNucleusOptions`, output capacity, and a deterministic seed. Optionally obtain `UmiAiTrainingGenerationCacheStats`: `computedRows`, `windowRebuilds`, and `retainedTokens`. All buffers remain caller-owned and are not modified when a request fails. The decoder must not be trained concurrently with generation.

The existing non-cached generation functions remain available. Use their results as the reference for regression testing. Neither raw bytes nor generated source text are automatically executed.

## Verify before adopting

Build the native learning laboratory with CMake and CTest. Run `framework.ai.training.generation_cache_parity` and `framework.ai.training.generation_cache_rejections`, which link the actual Framework decoder and KV cache. The tests compare token sequences, byte output, checkpoint preservation, and rejection behaviour.

A CTest result of `Not Run` or `BAD_COMMAND` indicates that the test executable did not execute; it is not proof of a mathematical failure. Check the build, the executable, runtime dependencies and endpoint-security findings before changing the inference algorithm.

## Further work

Future inference acceleration may use a position encoding that supports correct cache reuse after eviction, or a separately specified prefix/position policy. Such changes must remain opt-in and must be qualified against the relevant mathematical reference, rather than silently changing existing decoder outputs.
