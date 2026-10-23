# Native LLM — Canonical Decoder and KV Positional Qualification

**Umicom Framework | C23 | Development Batches 71–72**  
**Author:** Sammy Hegab, Umicom Foundation  
**Licence:** MIT

## Why this qualification exists

The opt-in growing-prefix study delivered in Batches 69–70 proved that two
execution schedules on the **existing Framework KV kernel** agree: rebuild
prefixes independently, or retain previously computed K/V and append one row.
For N prefixes, the former computes N(N+1)/2 token rows and the latter N rows.

That study did **not** compare those prefix logits against the independent,
canonical **training-tape decoder**. Both reference and candidate used the KV
kernel, so a shared error in the kernel might have escaped its parity checks.

Batches 71–72 introduce a second, independent numerical acceptance boundary
which calls `UmiAiTrainingDecoderLogits()` directly. They also establish that
cache invalidation and complete recomputation occur after the **first eviction**
under the decoder's learned absolute positional embeddings.

## Public Framework API

`include/umicom/ai/training/kv_position_qualification.h` declares:

```c
UmiStatus UmiAiTrainingKvPositionQualify(
    UmiAiTrainingDecoder *model,
    const size_t *tokenIds, size_t tokenCount, size_t nextTokenId,
    UmiAiTrainingKvPositionQualification *outQualification);
```

The caller supplies exactly `decoder.contextLength` token IDs, plus one token
to append after the context becomes full. The caller owns the model, input
buffer and result. The function borrows these values synchronously and does
not train, publish or store anything.

## The two independently checked states

| State | KV sequence | Independent reference | KV computed rows | Full-window rebuilds |
|---|---|---|---:|---:|
| Before rollover | Prefill the first token, append the remainder | `UmiAiTrainingDecoderLogits(model, original, contextLength, ...)` | N | 0 |
| After first rollover | Append the next token and correctly rebase learned positions | `UmiAiTrainingDecoderLogits(model, shifted, contextLength, ...)` | 2N cumulative | 1 |

The final vocabulary-logit row in **each** state is compared for *every*
vocabulary ID. Both sides must be finite and their absolute differences must
be <= `0.00004F`, matching the already established KV context regression
threshold. It is inappropriate to compare only top-1 token predictions:
incorrect scores can produce the same selected token.

At N=8 and vocabulary 260, 520 logit values are checked in total, the
incremental cache has performed 8 rows before rollover and 16 total after
one rollover, and one full-window rebuild must be recorded.

## Correctness, not a rolling speed claim

The model learns an embedding for each **position within its context**.
Before eviction, token A stays at position 0, B at position 1, and so on.
After the oldest token is removed, every surviving token moves left by one
position. Its original K/V representation no longer corresponds to the new
embedding. Existing K/V may not be reused as if its position were unchanged.

The Framework deliberately recomputes the entire context to preserve
numerical semantics. The `UmiAiTrainingKvPositionQualify` function verifies
that implementation rather than hiding it behind an optimistic speed label.
No new model variant, RoPE mode, ALiBi mode, implicit checkpoint conversion,
quantisation or alternative attention implementation is introduced.

## Failure and ownership contract

- Validate all pointers, exact context length and all token IDs before
  making any caller-visible changes.
- Check cache allocation using its existing model-shaped memory planner.
- Check cache work counters both before and after eviction.
- Verify decoder public configuration and completed optimiser step count are
  unchanged across both inference evaluations.
- Destroy the temporary cache on all post-creation paths.
- Publish the output structure **only after** all checks pass. Any mismatch
  returns `UMI_STATUS_INVALID_STATE`; bad inputs return
  `UMI_STATUS_INVALID_ARGUMENT`.

The function compares **configuration identity and optimiser step counts**;
the dedicated real-decoder regression also encodes checkpoint bytes before
and after the qualification and checks their identity. This distinction
prevents a runtime inspection from being misrepresented as a complete
checkpoint-compatibility proof.

## Building the isolated laboratory

After merging Batches 69–70 and 71–72 into their exact repository-relative
paths under `C:\umicom\Umicom-Applications\framework`:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path

cmake -S '.\framework\learning\native-llm\prefix-work' `
    -B '.\build\windows-ucrt64-native-llm-prefix-debug' -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Native KV qualification configure failed.' }

cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
    --target umicom-ai-training-kv-position-qualification-test `
             umicom-ai-training-kv-position-rejections-test `
             umicom-native-llm-position-qualification --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Native KV qualification build failed.' }

ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
    -R '^framework\.ai\.training\.(kv_position_|kv_prefix_)' `
    --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Native KV regressions failed.' }
```

For the complete canonical native LLM, continue to use the existing
`learning/native-llm` CMake project. Neither its source composition nor the
consolidated Framework/Studio build is silently changed by this opt-in batch.

## Unresolved and next milestones

1. Run the real decoder on Windows UCRT64; this is required before claiming
   production-decoder parity on the user's development environment.
2. Extend this numerical gate across recorded, trained checkpoints and
   distinguish tolerances, inference precision and model-quality effects.
3. Define an **opt-in** positional-encoding evolution plan with explicit
   parameter-shape, checkpoint and re-training decisions. Do not reinterpret
   learned absolute-position checkpoints as if they were RoPE-compatible.
4. Only after the alternative semantics are qualified should the original
   full-context text generator be changed or advertised as faster.
5. Independently investigate the pre-existing `accumulation_demo` Windows
   process-launch issue and consolidated build failures. This addition does
   not claim to repair them.
