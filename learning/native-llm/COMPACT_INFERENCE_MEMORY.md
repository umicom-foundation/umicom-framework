# Compact Native Inference Memory

**Umicom Framework · Umicom Foundation · Sammy Hegab · MIT**

## Purpose

A language model needs memory for its trained parameters and for attention
state while it processes input tokens. Previously the native decoder's
inference cache reserved the maximum capacity of every supported parameter
array, regardless of a model's actual dimensions. The profiler correctly
identified that a model with 19,168 bytes of parameters used more than
255,000 bytes for its cache object.

The current cache allocates exactly the validated weight tensors, the
configured key/value rows and the last-token vocabulary scores. The
canonical decoder still owns all model training and checkpoint data.

## Where the implementation lives

| Contract | Authoritative implementation |
| --- | --- |
| Cache construction and inference | `include/umicom/ai/training/kv_cache.h` and `src/ai/training/kv_cache.c` |
| Parameter shapes | `src/ai/training/decoder_private.h` and `decoder.c` |
| Memory planning and full-context comparison | `inference_profile.h` and `inference_profile.c` |
| Existing numerical forward and training kernels | `matrix_kernel.c`, `tape.c`, `transformer.c` |
| Educational consumer | `learning/native-llm/inspect_cache_memory.c` |

Applications use Framework public headers and do not copy this allocation
logic. Parameters are copied into an opaque cache snapshot; the decoder is
borrowed and must outlive its cache. A successful optimiser update marks an
existing cache stale, and inference must recreate it.

## Exact allocation

The cache owns one C23 flexible-array allocation. Its payload contains,
consecutively, each existing parameter tensor in the canonical order,
`contextLength * channels` key floats, the same number of value floats and
`vocabularySize` last-token logit floats.

If `P` is the total number of parameter floats, `T` the context length,
`C` the channel count and `V` the vocabulary size, the requested cache
reservation is:

```text
sizeof(UmiAiTrainingKvCache) + (P + 2*T*C + V)*sizeof(float)
```

This is an exact request to the C allocator for the native cache object,
including the structure's alignment and fixed token-ID slots. It does not
include the allocator's private metadata, the decoder's own training graph or
temporary stack buffers.

Every multiplication and addition used to derive the size is checked before
allocation. Invalid model shapes and capacity overflows are rejected without
modifying caller outputs. Parameter offsets come exclusively from the
Framework's existing decoder shape contract.

## Why inference remains correct

The existing algorithm stages each operation in a separate bounded local
state. Only after every calculation succeeds does it publish token history,
keys, values, last-token logits and work counters. Previously the stage was
copied into a fixed-size structure. The compact implementation copies the
same valid rows into a shape-sized allocation, preserving the previous
failure-atomic semantics.

The decoder still uses *learned absolute positions*. While a prefix grows,
previous key/value rows can be reused. When the fixed context window rolls,
retained tokens change positions; the cache must rebuild from the shifted
window. Compact storage must not conceal those rebuilds or claim additional
speedups. The original full decoder remains the numerical oracle.

## Compatibility

`UmiAiTrainingKvCacheAllocationSize` continues to exist as a conservative,
shape-independent memory-planning **upper bound**, not as the actual
reservation of an instantiated cache.

`UmiAiTrainingKvCacheAllocationForConfig` reports the expected real
allocation for a validated decoder configuration. The new
`UmiAiTrainingKvCacheAllocatedBytes` reports the memory requested by an
existing cache instance.

`UmiAiTrainingInferenceEstimate` now uses the shape-specific calculation and
continues to report logical model parameter bytes separately. No existing
entry points or public C structures were removed.

## Build and inspect on Windows

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-cache-memory.exe"
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-profile.exe"
```

The native inspector verifies planned bytes against actual allocated bytes
for two configurations. The profiler separately measures process CPU time,
computed rows and full-context numerical parity; it does not claim that
smaller allocations necessarily improve execution time.

## Engineering limitations

The decoder's maximum context length, channel width, vocabulary size and
number of parameter tensors remain bounded by the existing educational
interfaces. This change reduces **memory waste**, not those model limits.
Temporary inference staging still uses bounded stack storage, and training
still uses the existing tape. Future work may compact the staging buffers,
measure cache allocation latency and introduce independently validated
quantised or accelerated representations. These are not claimed here.
