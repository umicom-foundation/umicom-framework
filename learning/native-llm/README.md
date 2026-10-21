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


---

## Batch 03 continuation: native multi-head decoder Transformer Block

The earlier Batch 01 and 02 descriptions above are retained as **historical
milestones**. The current implementation adds the following Framework-owned
capabilities without deleting or replacing existing public APIs:

- `UmiAiTrainingMultiHeadCausalAttention` divides query/key/value channels
  into independently masked heads and accumulates Q/K/V gradients.
- `UmiAiTrainingLayerNorm` implements trainable gamma/beta and verifies
  analytical gradients against central finite differences.
- `UmiAiTrainingTapeCheckpoint` and `UmiAiTrainingTapeRollback` allow
  failure-atomic graph composition and invalidate previous gradients.
- `transformer.h` / `transformer.c` compose one **pre-LN decoder block** with
  two residual connections, Q/K/V/output projections, and a trainable ReLU
  feed-forward sublayer using the existing shared tape operators.
- `train_transformer.c` trains token embeddings, one block and vocabulary
  projection on a deliberately tiny, memorisable two-sequence corpus.

The isolated learning project now runs **21 CTest cases**. The eight new
focused tests cover multi-head forward/masking, multi-head finite-difference
and alias gradients, LayerNorm outputs, LayerNorm derivatives, residual
identity, full composed block finite differences and causality, supervised
parameter optimisation, and safe rollback/invalid parameters. The extra
transformer-demo test makes 21 in total.

### Native build and inspection (Windows PowerShell)

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
      -B ".\build\windows-ucrt64-native-llm-debug" `
      -G Ninja -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" --output-on-failure
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-transformer-demo.exe"
```

### Scope and limitations

The block is a real trainable neural network component, **not yet a complete
LLM or source-code expert**. It has no checkpoint persistence, positional
embedding, general text tokenizer, held-out evaluation, generation sampler,
large-context execution or GPU kernels. It uses float32 scalar CPU operators
and a bounded graph. LayerNorm and multi-head attention support backward
propagation; the feed-forward activation remains ReLU for transparency.

The root Framework `CMakeLists.txt` still needs an explicit additive
`include(cmake/UmicomNativeLlmTraining.cmake)` when a consumer wishes to
register these standalone training targets in the full application build.
Do not add the same include twice. Everything else in the Framework's original
CMake configuration, source catalogue and application profiles is preserved.

---

## Batch 04 continuation — A bounded native decoder and portable checkpoints

The historical Batch 01–03 descriptions above are preserved for provenance.
Batch 04 composes the *same Framework training tape* into a complete, small
one-block decoder-only Transformer experiment. New code is in:

- `include/umicom/ai/training/decoder.h` and `src/ai/training/decoder.c`:
  seeded trainable token/absolute-position tables, a pre-LN two-head block,
  learned vocabulary logits, inference, full-parameter SGD, and evaluation.
- `include/umicom/ai/training/dataset_split.h` and
  `src/ai/training/dataset_split.c`: deterministic whole-sequence permutation.
- `include/umicom/ai/training/checkpoint.h` and
  `src/ai/training/checkpoint.c`: bounded portable binary32 checkpoint
  encoder/decoder with checksum and explicit no-overwrite file publication.
- `src/ai/training/decoder_private.h`: internal, stable parameter order; not an
  application API.
- `tape.h` / `tape.c`: additive `UmiAiTrainingSgdStepMany` transaction so a
  bad weight update cannot leave an earlier model parameter changed.

The existing native lab includes `train_decoder.c` and one new CTest entry;
the Framework CMake module adds five focused tests. The isolated suite now
contains **27 tests**, including all 21 prior tests.

### Build and run the first complete decoder

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
      -B ".\build\windows-ucrt64-native-llm-debug" `
      -G Ninja -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
      --output-on-failure --no-tests=error
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-decoder-demo.exe"
```

To save a **new** model file (the same path will never intentionally be
replaced), create an untracked experiment directory and supply a filename:

```powershell
New-Item -ItemType Directory -Force "C:\umicom\_local\models" | Out-Null
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-decoder-demo.exe" `
  "C:\umicom\_local\models\first-decoder-01.umidec"
```

### What was measured

An independent Linux GCC 14.2 run on a 12-sequence toy corpus, with 9 distinct
training combinations and 3 withheld combinations, observed training
cross-entropy **1.386273 -> 0.590156** over 420 updates, while held-out
cross-entropy **worsened from 1.415977 to 4.573408**. This is precisely why
held-out evaluation matters. Neither loss trend proves language comprehension.
Checkpoints restored byte-for-byte and inference predictions were identical.

### Boundaries and current limitations

- One decoder block, 15 trainable parameter tensors, no BPE tokenizer, no
  decoder stack, no sampling, no optimiser moments or cloud runtime.
- The checkpoint is `UMIDEC04` version 1 (NOT the historical `UMIMODEL`
  word-transition file). Its FNV checksum detects accidents, not hostile edits;
  authentication and trust policy belong outside the codec.
- A checkpoint stores weights, architecture, seed and completed steps, but not
  corpus identity, data-loader cursor, or learning-rate schedule. A later
  AdamW/training-pipeline milestone must store all relevant optimiser state.
- The standalone `learning/native-llm` CMake project builds independently.
  To register its test targets in the large consolidated Framework build, add
  **once** to the current Framework root `CMakeLists.txt`, after review:

  ```cmake
  # Share the same Framework-owned native training library with host apps.
  include(cmake/UmicomNativeLlmTraining.cmake)
  ```

Use `Bits2Banking/content/lessons/10-native-ai/13-...`, `14-...` and `15-...`
for full explanations, diagrams in text, laboratory exercises and correct
interpretations of the evidence.


## Batch 05 — Native source corpus and byte-level tokenisation

`umicom-native-llm-corpus` imports one explicitly selected **local** file
(up to 1 MiB) and displays a few reversible next-byte token pairs. A byte
tokenizer provides IDs 4..259, reserving 0..3 for PAD/BOS/EOS/UNK. Files
are read exactly; UTF-8 encoding is preserved byte-for-byte, but no Unicode
normalisation, subword vocabulary, model training, or document permission
management is claimed. Use only files approved for training.

This is a data-preparation prerequisite; it does **not** yet join the
full transformer training loop to a real corpus. The existing split and
checkpoint services remain authoritative. The 3 additional focused CTest
regressions exercise byte round trips, bounded windows and file import.

Example from the Umicom-Applications repository root:

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-corpus.exe" `
  ".\framework\learning\native-llm\README.md"
```

---

## Batch 06 — Reviewed source-code dataset and real byte-level training

The previous historical batch descriptions are preserved above. Batch 06 adds
`dataset_manifest.h` / `.c` to the existing Framework training library, using
its canonical Corpus Loader, byte tokenizer, RNG and decoder. Each file is
assigned to **either training or held-out evaluation**. No training window
crosses a document boundary. Exact repeated paths and identical-size/FNV64
fingerprints are rejected. Those are basic integrity measures, not a filesystem
sandbox or statistical leakage guarantee.

The public decoder vocabulary limit is raised from 64 to 260 so the existing
reversible byte tokenizer can be connected to the existing decoder. The
4096-elements-per-tensor tape limit still applies to each configuration.

Run the tiny full training/evaluation example after building:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-source-train.exe" `
  ".\framework\learning\native-llm\fixtures\manifest.txt" 64
```

The executable reports per-file provenance and held-out loss measured before
and after training. Source-code fixtures are fictional, MIT-licensed teaching
examples; the decoder does not yet understand C23 semantics. An explicit third
argument optionally names a **new** checkpoint file. The program writes none
by default and does not perform network requests. The operator must review all
manifest entries and file contents before invoking this example on other data.

Batch 06 registers three new tests and retains all previous ones, giving **33
standalone CTest cases**. Inspect `CHANGE_SUMMARY.md` and `VALIDATION.md` for
exact evidence and the limitations of the isolated Linux qualification.
