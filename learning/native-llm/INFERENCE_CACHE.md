# Incremental key/value inference in Umicom Framework

**Author:** Sammy Hegab  
**Organisation:** Umicom Foundation  
**Licence:** MIT  
**Implementation:** C23, canonical Framework decoder and inference-only cache

## Purpose

The existing Umicom native decoder implements one pre-layer-normalised,
multi-head causal attention block with trainable token and absolute positional
embeddings. This native cache allows a caller to append tokens to a growing
prefix without recalculating the attention **key and value projections** of
unchanged earlier tokens. It does not change checkpoint formats, training
weights, sampling algorithms or experiment evaluation.

The public interface is `include/umicom/ai/training/kv_cache.h`. All inference
maths are in `src/ai/training/kv_cache.c` and are checked against the original
`UmiAiTrainingDecoderLogits` results. The source is Framework-owned and can be
used by any thin Umicom application or native learning laboratory.

## Compatibility and correctness

The model uses trainable *absolute* position embeddings. The cache is therefore
valid while a prefix grows and existing token positions are unchanged. When
appending to an already-full window, the oldest token is evicted, surviving
tokens change position and their cached keys and values must be recomputed.
This implementation rebuilds the entire full window at that boundary.

Do not remove the reconstruction without changing and retraining the model's
positional representation and proving numerical equivalence. In particular,
shifting old cached rows without recomputation is incorrect here.

The cache owns its copied model weights and its token history. The borrowed
decoder must outlive the cache and must not be mutated concurrently. A training
step changes the decoder's recorded optimiser step and makes the old cache
return `UMI_STATUS_INVALID_STATE`. Destroy and recreate the cache from the
updated or reloaded decoder.

`Prefill` and `Append` publish one vocabulary-logit row only after successful
validation and computation; a failed call retains both the previous cache
state and caller output. An ordinary reset clears the current sequence but
retains the approved parameter snapshot.

## Build and use

From the Umicom Applications root in PowerShell:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    -R "kv_cache" --output-on-failure
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-kv-inspect.exe"
```

Optionally pass a single reviewed decoder checkpoint path to the inspector.
The default model has a fixed seed and is **not pretrained**. The command
compares every cached output with the original decoder and prints the maximum
absolute logit difference, retained token count, rows computed and window
rebuild count. The normal test runner also executes this inspector as a CTest
case.

## Evidence and limitations

Four focused tests independently qualify growing prefixes, absolute-position
rolling windows, API rejection without mutation, reloaded trained checkpoints
and stale-weight detection. The inspector is an additional regression test.
All pre-existing native training tests remain registered.

Counters measure only computed token rows. They are *not* elapsed time,
throughput, GPU utilisation or a complete cost model. In particular, repeated
full-window rebuilding limits the benefit of caching this small decoder during
long text generation. The existing `UmiAiTrainingDecoderGenerate` API retains
its original left-padding and full-context behaviour; it has **not** been
silently switched to a different inference policy.

Further work should benchmark and profile the scalar cache, qualify optional
vectorised native backends against the same logits and investigate a positional
representation designed for longer incremental generation.

For the accompanying teaching discussion, read the Bits2Banking course's
*Incremental Native Inference and Key/Value Caching* lesson.
