# Native LLM prefix benchmark: like-for-like, evidence-first profiling

Umicom Foundation · Sammy Hegab · MIT · 24 October 2026

## Purpose and approved scope

The Framework now contains two distinct autoregressive inference policies:

1. **Historical full BOS-left-padded generation** (`UmiAiTrainingDecoderGenerateNucleus` and `UmiAiTrainingGenerationSession`). The entire context always has `contextLength` tokens, including BOS padding. Learned absolute positions change when the window slides, requiring KV rebuilding.
2. **Opt-in unpadded prefix generation** (`UmiAiTrainingDecoderGeneratePrefixNucleus`). The first real prompt token is learned position zero. Within a growing prefix, one added token can reuse previous K/V rows; after eviction, the original cache rebuilds the context.

**Never compare generated sequences across these policies and claim token parity.** They represent different numerical inputs and can legitimately disagree even when both are implemented correctly.

Batch 75–76 adds a *like-for-like* comparison inside the **second** policy. The reference does not use the KV cache. Instead it invokes the existing `UmiAiTrainingDecoderLogits()` with a right-padded context and samples the row belonging to the last retained real token. Causal masking ensures later padding cannot influence that earlier row. The incremental path invokes the public prefix generator using the same seed and sampling policy. Neither path has its own decoder mathematics, weights or tokeniser.

## C23 API and ownership

Include `umicom/ai/training/prefix_benchmark.h` and call `UmiAiTrainingPrefixBenchmarkRun(model, promptIds, promptCount, policy, plan, report)`.

- The decoder and prompt remain caller-owned. No pointers survive return.
- `generatedTokens`: 1–64; `warmupPairs`: 0–5; `measuredPairs`: 1–9.
- All pairs use the same model, prompt, seed, temperature, top-K and top-p.
- All generated token IDs must match pairwise and across repeated trials.
- Every measured cache row count and rebuild count must match the existing Framework workload planner.
- The *entire binary portable checkpoint*, encoded before and after the experiment, must be byte-for-byte unchanged; optimiser steps and decoder configuration are checked as well.
- Output is staged. Any mismatch, validation failure or internal error leaves the caller's report unchanged.
- The returned token fingerprint is FNV-1a, a non-cryptographic reproducibility marker, **not** a release signature or authenticity guarantee.
- No checkpoint is saved or loaded by the experiment. No generated text is executed.

## Timing design

The benchmark uses a monotonic wall timer where available: `QueryPerformanceCounter` on Windows and `CLOCK_MONOTONIC` on POSIX. The fallback is C23 `timespec_get(TIME_UTC)`, which is not guaranteed monotonic and is reported honestly. CPU time uses C `clock()` and is process-wide, not per-thread. A zero, reversed, invalid or non-finite clock interval is *unavailable*, not a zero-duration miracle.

The benchmark performs a bounded warm-up, then alternates execution order between canonical and cached paths to reduce predictable thermal and CPU-cache bias. Both timings include each complete generation call. The cached path includes session creation, cache snapshot and destruction; the reference path includes full logits and sampling. Report minimum, median and maximum wall time over repeated trials, and median process CPU time where available. Timings may vary across machines, power settings, compiler options and concurrent processes.

**A measured decrease in computed token rows does not guarantee a decrease in CPU time or wall latency.** A scalar KV cache can have more overhead than a full-context optimised matrix implementation. The existing decoder is an educational CPU implementation with tiny dimensions; extrapolating these timings to production LLMs would be invalid.

## Worked example

For an eight-token context, a three-token prompt and twelve generated samples:

- Canonical right-padded reference: `8 × 12 = 96` computed token rows.
- KV session prefill: 3 rows.
- First 5 appends grow the context: 5 more rows.
- Remaining 6 appends rebuild the full eight-token window: `6 × 8 = 48` rows.
- Total cached rows: `3 + 5 + 48 = 56`.
- Rows avoided: `96 − 56 = 40`, a 41.67% reduction in *this workload*, not a guaranteed performance improvement.
- The twelfth generated token is pending and is not appended until another generation pull.

If the prompt begins at full context capacity, there is no growing-prefix saving: every append rebuilds the full context.

## Validation sequence

Use the opt-in `learning/native-llm/prefix-work` CMake project, which includes the same production C23 decoder, generation and cache sources as the Framework. No scripting-language implementation is introduced. Focused tests:

- `framework.ai.training.prefix_benchmark_parity` — real decoder, repeated paired generation, unchanged model checkpoint.
- `framework.ai.training.prefix_benchmark_rejections` — invalid input and byte-for-byte caller-report preservation.
- `framework.ai.training.prefix_benchmark_demo` — benchmark executable output.

Run both Debug and Release configurations on Windows UCRT64. Report `ctest` pass/fail totals; do not infer them from prior mock-based test outcomes. Cross-compiler results and sanitizer builds should be retained as separate evidence. Do not commit generated timing files or build directories.

## Controls before future optimisation

Any future relative-position encoding, RoPE or position-rebased KV-cache design changes model mathematical semantics. It must be an explicitly named model architecture/checkpoint family, not an in-place modification of learned-absolute-position checkpoints. First introduce independent numerical regressions and compatibility migration policy; only then discuss reuse after eviction. This benchmark deliberately does NOT change current position encoding, checkpoint format or weight parameters.
