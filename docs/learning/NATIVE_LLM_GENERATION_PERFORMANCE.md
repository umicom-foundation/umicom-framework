# Measure Native Language-Model Generation Work in C23

Umicom Framework provides a small, inspectable decoder for learning how language models are constructed and evaluated. Its inference implementation can generate text through a full-context reference path or through a persistent key/value (KV) cache.

A KV cache can be useful when previously calculated attention keys and values remain valid. However, **this model uses learned absolute-position embeddings**. Once a full context window advances, each retained token receives a different position. Reusing its old key/value representation would change the model's mathematical meaning. The current cache therefore recomputes the complete window after each advance.

## 1. Estimate the workload before running

The new `UmiAiTrainingGenerationWorkEstimateBuild` function accepts the decoder configuration, the prompt length and the number of tokens to generate. It reports the expected number of full-context reference rows, KV-cache rows and full-window rebuilds. Memory reservation comes from the existing Framework KV-cache allocation service, not a separately maintained formula.

For a decoder with a context length of 8 and 12 generated tokens:

- Reference generation computes 8 × 12 = **96 token-rows**.
- The cache initially computes 8 rows and rebuilds 11 more windows of 8 rows each: 8 + (11 × 8) = **96 token-rows**.
- The cache records 11 full-window rebuilds.

These counts are **workload units**, not CPU instructions, allocations per token, or guaranteed execution times.

## 2. Compare actual generation output

`UmiAiTrainingGenerationPerformanceCompare` runs the existing full-context nucleus generator and persistent KV-cache session against the same decoder, prompt, top-p policy and random seed. It requires exact token-for-token agreement and checks that the cache's recorded work matches the forecast. Failed comparisons do not publish partial results.

The results include process CPU-time readings from the C standard-library `clock()` function. They are single observations, affected by compiler, machine load, cache state and clock granularity. They are **not wall-clock latency measurements**, reliable production performance comparisons or speedup guarantees. A zero reading may mean the timer is too coarse.

## 3. Run the native example

From a configured native LLM learning laboratory build, compile the `umicom-native-llm-generation-work` executable. Run it without arguments. The example uses a deterministic byte-token prompt, reports the reference and cached work, and explains the absolute-position limitation. No network, source execution, pretrained model or scripting runtime is required.

## 4. Exercise

1. Recalculate the rows for a 16-token context and 7 generated tokens.
2. Explain why a KV cache cannot be assumed faster if it must rebuild all keys and values after every token.
3. Compare your CPU observations with a Release build. Why would one small sample not establish a reliable speedup?
4. Describe what would have to change in the model's position-handling strategy before cached keys could be retained across a rolling window. Explain why such a change would require fresh numerical-parity and checkpoint-compatibility tests.

**Key principle:** Preserve model meaning and exact output agreement before optimising execution. Framework services own the model, sampling, memory accounting and measurements; applications consume those contracts without introducing their own inference algorithms.
