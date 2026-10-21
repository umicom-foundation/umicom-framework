# Umicom Framework — Native LLM Learning Laboratory (Batch 01)

**Author:** Sammy Hegab · Umicom Foundation  
**Licence:** MIT  
**Primary implementation:** C23; no new runtime scripts, pretrained models or vendor services.

## What this batch actually implements

The Framework owns `include/umicom/ai/training/tape.h`, `src/ai/training/tape.c` and the shared CMake composition module in `cmake/UmicomNativeLlmTraining.cmake`. The laboratory contains only a thin native demonstration and six regression tests. There is no second application-local neural implementation.

The tape supports bounded row-major float32 matrices, validated allocation, checked matrix multiplication, equal/row-broadcast addition, ReLU, mean-squared-error loss, numerically stable categorical cross-entropy, reverse-mode differentiation and SGD. A tape owns every tensor; `Rewind` disposes of derived nodes between training steps without discarding original parameters.

The demonstration learns deterministic next-token transitions using a single linear one-hot **bigram** layer. It deliberately does **not** implement attention, embeddings, a transformer, a general tokenizer, checkpoint persistence, a model registry or multi-expert coordination. Those belong to future lessons/batches.

## Compile and test after merging into your actual checkout

Open PowerShell in `C:\umicom\Umicom-Applications`:

```powershell
cmake -S '.\framework\learning\native-llm' -B '.\build\native-llm-lesson' -G Ninja -DBUILD_TESTING=ON
cmake --build '.\build\native-llm-lesson' --parallel 2
ctest --test-dir '.\build\native-llm-lesson' --output-on-failure
& '.\build\native-llm-lesson\umicom-native-llm-train.exe'
```

The stand-alone educational CMake project requires the **real** Umicom Framework public header `include/umicom/base/status.h` from your checkout. Do not copy the learning subfolder out of the Framework and expect it to compile on its own.

To make the new targets part of the full Framework and parent Umicom Applications CMake build, review and add this **single additive include** at the end of `framework/CMakeLists.txt`, preserving every existing line:

```cmake
# Register the native C23 training laboratory; existing Umicom AI is unchanged.
include(cmake/UmicomNativeLlmTraining.cmake)
```

This optional full-tree wiring is **not** silently applied by this batch, because the existing root build file is very large and should be merged against your current local revision. The isolated laboratory builds and tests without modifying it.

## Test catalogue

- `framework.ai.training.contract`: dimensions, provenance, buffers, ownership, rewind.
- `framework.ai.training.gradients`: independently calculated matrix/bias/ReLU/MSE and CE derivatives.
- `framework.ai.training.regression`: fit the line `y=2x+1` by SGD.
- `framework.ai.training.bigram`: train next-token transitions by SGD.
- `framework.ai.training.finite_differences`: central finite-difference derivative checking.
- `framework.ai.training.limits`: rejected values, overflow, tape capacity.

The test executables report failures with explicit line numbers even in release mode, rather than depending on disabled assertions.

## Known limitations

- CPU and float32 only; double intermediates where documented.
- Two-dimensional matrices with fixed per-tape and per-tensor limits.
- No gradient update is supported while a tape is concurrently used by another thread; caller serialises access.
- Checkpoint files, AdamW, higher-rank tensors, causal attention, positional embeddings, batching policies, deterministic RNG seeds and trained model evaluation remain future work.
- The training corpus is deliberately memorisable. Falling loss is evidence that the code trains a simple model, **not** evidence of generalisation.

---

## Batch 02 continuation: reproducible embeddings and attention

The preceding Batch 01 text is retained as the historical starting-point
reference. This addendum describes the features now implemented in Batch 02:

- `initialisation.h` / `initialisation.c` provide a caller-owned seeded PRNG
  for reproducible parameter creation without global random state.
- `sequence_shape.h` / `sequence_shape.c` validate logical rank-three
  `[batch,time,channels]` layouts, flattened to the established 2D tape.
- The existing `tape.h` / `tape.c` now add trainable embedding gather and
  reverse-mode gradient scatter-add, including repeated token IDs.
- The existing tape also adds single-head scaled dot-product *causal* attention
  and independently tested Q/K/V derivatives, with explicit batch boundaries.
- `train_attention.c` trains a small context-sensitive model from seeded
  weights. This remains an educational attention-based experiment, **not** a
  complete transformer or a production language model.

The stand-alone CMake project now registers **12 tests**: six preserved
Batch 01 tests, five new focused tests and the new attention-demo regression.
Use the same PowerShell build procedure above, then run:

```powershell
& '.\build\native-llm-lesson\umicom-native-llm-attention-demo.exe'
```

The canonical Framework's *root* `CMakeLists.txt` still needs to include
`cmake/UmicomNativeLlmTraining.cmake` for these tests to appear in the
`windows-ucrt64-all-debug` consolidated preset. If the Batch 01 one-line
include was already added, do **not** add it again. This Batch 02 package
updates the existing module; it never replaces the main `Umicom::ai` runtime.

The next milestone is the composition of a complete transformer block with
residual connections, layer normalisation, feed-forward layers and multi-head
attention. Proper training/evaluation splits and checkpoint persistence must
precede claims that a model generalises to unseen data.
