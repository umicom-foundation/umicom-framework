# Wider Native Training Tensors and Shape-Owned CPU Workspaces

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**  
**Primary implementation:** C23 · **Status:** Tested educational extension

## Purpose and scope

The native educational decoder uses one Framework-owned automatic-differentiation tape, trainable token/position embeddings, a pre-layer-normalised Transformer Block and a logits projection. Previous experiments exposed a bottleneck: a 4,096-element-per-tensor limit. Even when a computer had plenty of RAM, a 16-token mini-batch using 260 output token classes needed `16 × 260 = 4,160` logits and could not be represented.

The tape now permits up to **16,384 float32 elements per individual tensor**, without changing its public functions or replacing the underlying training engine. `UMI_AI_TRAINING_MAX_TENSORS` remains 256; the total number of nodes and total committed memory are controlled separately. The matrix-kernel ceiling is linked directly to the tape constant to avoid having two independent, drifting sizes.

This is *capacity expansion*, not removal of memory bounds and not a multi-million-parameter production trainer. The default `UmiAiTrainingTapeSetMemoryBudget` policy still applies to every newly published node. The new tensor maximum is a compile-time safety limit. There are still explicit limits in the decoder configuration, dataset manifest parser, checkpoint format, and experiment orchestration.

## Numerical and ownership behaviour

`UmiAiTrainingMatrixMultiply` previously staged every result inside a fixed-size stack array. That declaration is retained in a documented disabled block for engineering review. The active implementation preserves a 4,096-element stack fast path for smaller results. For larger outputs it allocates exactly `rows × columns × sizeof(float)` bytes of **temporary heap workspace** after validating matrix shapes, caller capacity and finite inputs. It computes the entire matrix into the workspace before copying results into caller-owned memory.

The reference and tiled paths retain their original accumulation order: each dot product accumulates in `double` and materialises a checked `float32`. Both input/output aliases remain supported. On a malformed dimension, insufficient output capacity, invalid numerical result or allocation failure, the output remains unchanged. The new workspace is freed on success and on every post-allocation error path.

The temporary workspace allocation is not part of the training tape's persistent `currentBytes` or `peakBytes` accounting. It is visible to the process allocator during the multiplication. Do not describe the persistent tape budget as a process-wide RAM cap.

## Measured shape examples

| Workload | Calculation | Result |
| --- | --- | ---: |
| Default decoder output, one sequence | `8 × 260` | 2,080 logits |
| Two source-code sequences | `2 × 8 × 260` | 4,160 logits |
| Four source-code sequences | `4 × 8 × 260` | 8,320 logits |
| Four-sequence output buffer | `8,320 × 4` bytes | 33,280 bytes |
| Theoretical logit ceiling at context 8 | `floor(16,384 / (8 × 260))` | 7 sequences |
| Public dataset sampling ceiling | `UMI_AI_DATASET_MAX_SEQUENCES` | 4 sequences |
| Larger embedding table | `260 × 32` | 8,320 parameters |

A capacity planner can report seven shape-feasible sequences even though the current dataset API accepts no more than four. This is not contradictory: tensor memory, dataset limits, and graph-node counts are distinct constraints. Applications should validate against *all* applicable constraints before training.

## Larger decoder parameters and checkpoint compatibility

`UmiAiTrainingRandomFillUniform` now accepts up to `UMI_AI_TRAINING_MAX_ELEMENTS` values. Its deterministic SplitMix64 sequence is unchanged; extending its maximum count does not change results for previously valid seeds and counts.

The decoder can now construct a model with a `260 × 32` token-embedding table, rather than refusing it solely because the table exceeds 4,096 elements. The public `UmiAiTrainingDecoderConfig` layout, the 15-parameter ordering and the checkpoint **`UMIDEC04` binary format** are unchanged. Previously saved, valid checkpoints continue to decode under the same schema. Wider configurations remain bounded by the existing 131,072-byte maximum checkpoint file, other parameter shapes and available training memory. The tests verify round-trip checkpoint bytes and matching logits after restoring a wider trained model.

## Native build, tests and inspection

Use Windows PowerShell and the existing MSYS2 UCRT64 toolchain:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-tensor-scale.exe"
```

The inspector uses the existing decoder training API, not its own neural implementation. It shows theoretical shape feasibility, an actual multi-sequence optimiser step and committed/peak tape-owned memory. The independent regression suite covers enlarged matrix multiplication, gradient propagation, checkpoint reload and repeated source-code experiments with multiple sequences.

## Limits, evidence and next steps

**Do not equate greater tensor capacity with better language ability.** Wider vectors or more sequences can make experiments feasible, but useful specialised models require larger datasets, model-depth engineering, optimiser selection, held-out evaluation and explicit computing budgets. This implementation still has one Transformer Block, bounded absolute positional embeddings, a limited vocabulary and a small teaching corpus. It does not train an expert C23, Framework or Calypso assistant.

Before considering another substantial increase, inspect temporary activation allocations, failure injection, decoder parameter shapes, checkpoint allocation policies, backward-pass memory and end-to-end CPU throughput. Do not make uncited extrapolations from the educational profiler to production LLM throughput.
