# Measuring native decoder inference and memory

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

This laboratory compares the established full-context decoder and its own
key/value inference cache on **identical token streams**. Nothing in this
exercise implements a second decoder, attention kernel, or optimiser. The
Framework's model remains the authority for generated logits and trained
parameters.

## What is actually measured?

| Measurement | Meaning | Does NOT mean |
| --- | --- | --- |
| Logical model parameter bytes | Sum of exact trainable matrix elements multiplied by four for float32 | Overall memory consumed by the model process |
| KV payload bytes | Two float32 matrices, each sized `contextLength * channels` | Total cache allocation |
| Cache allocation bytes | `sizeof(UmiAiTrainingKvCache)` on the compiling platform | Peak resident set, allocator overhead, or every decoder allocation |
| Full-context output-logit bytes | `contextLength * vocabularySize * sizeof(float)` | An observed physical memory write count |
| Decoder reference rows | `referenceQueries * contextLength` | Measured CPU instructions |
| KV cache rows | Rows actually computed by the existing cache since prefill | Guaranteed fewer physical operations or greater throughput |
| Window rebuilds | Count of full cache refreshes after rolling the context | A correctness error |
| Max logit deviation | Largest absolute difference between cached and canonical token scores | Probability error, held-out accuracy, or model quality |
| `clock()` seconds | Process CPU time accumulated around the two inference paths | Wall-clock latency, sustained throughput, or a speedup guarantee |

The current cache reserves parameter arrays sized by their maximum supported
element count; they are not allocated compactly for each trained model. We
expose the **real struct allocation size** rather than pretending only the
live key/value rows consume memory. This is useful evidence for a future
variable-sized storage design.

## Why a full context can erase caching benefits

Suppose the model context is six tokens. Prefill uses two tokens, then nine
more arrive. Four appended tokens grow the cache to six. The remaining five
append operations shift the window. Our decoder uses learned **absolute**
position embeddings, so those five operations must rebuild all six rows.

- Cache rows: `2 + 4 + (5 * 6) = 36`.
- Reference calls: `1 + 9 = 10`, each computing six rows, or `60` rows.

That difference represents **24 fewer calculated rows**, not necessarily
less elapsed CPU time. The cached implementation has bookkeeping and uses
specialised scalar inference code. On a very small model it may be slower.
If the prefill starts with six tokens, both implementations compute 36 rows
for the same six queries: all subsequent appends shift absolute positions.

## How numerical parity is verified

The profiler always calls the existing `UmiAiTrainingDecoderLogits` API
for an independent result. A short prefix is followed by *future* BOS tokens
to fill the configured reference context. Those future positions are hidden
from earlier predictions by causal attention. Comparing the prefix's own
last row therefore preserves its original position. When the window is
full, the profiler compares the final row in the full rolling context.

The profiler refuses to publish a report if an absolute score difference
exceeds `0.00005`. This tolerance reflects the present float32 educational
kernel; stricter numeric requirements must be requalified for other models
and accelerated math backends.

## Native entry points

The public profile contracts are:

- `include/umicom/ai/training/inference_profile.h`
- `src/ai/training/inference_profile.c`

The cache allocation is reported by the additive
`UmiAiTrainingKvCacheAllocationSize` public API. It returns the actual size
of the cache heap object on this platform; it does not allocate one.

The read-only C23 demonstration is
`learning/native-llm/profile_inference.c`. It accepts an optional reviewed
checkpoint and uses a bounded source-code teaching fixture. No network
access, source execution, model training, checkpoint writes, or scripting
runtime is required.

## Run the experiment on Windows

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-profile.exe"
```

To inspect a previously trained model, pass the path to its **reviewed**
checkpoint. The checkpoint loader and validation remain Framework-owned.

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-profile.exe" `
    "C:\umicom\_local\decoder-training-demo.umi"
```

An example with the tiny seeded untrained model reports a cache reservation
much larger than the live model's logical weights. These figures depend on
the C ABI and current struct layout; always use the executable's values
from the actual computer. Run the profiler repeatedly under the same build,
with the same input and otherwise idle CPU, before interpreting timing
comparisons. The CTest suite does **not** pass or fail on elapsed time.

## Safety, limitations and the next scaling decision

The bounded educational profiler accepts at most 256 tokens per experiment.
It does not yet support a production context window, a multi-block decoder,
quantised weights, GPU timing, stack tracing, peak resident-set measurement,
or multi-process benchmark coordination. It does reveal the next engineering
problem: the inference cache has fixed, largely unused parameter buffers.

The next memory engineering step should replace those reservations with
bounded, shape-sized allocations **only after** explicit lifetime contracts,
allocation-failure injection, checkpoint parity and complete regression
qualification. Increasing the model dimensions without those checks would
make memory requirements harder to reason about, not automatically improve
our language model.
