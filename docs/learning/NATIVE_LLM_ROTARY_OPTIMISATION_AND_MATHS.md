# Native LLM Batch 87–88 — A Complete Bounded RoPE Training Exercise

**Author:** Sammy Hegab — Umicom Foundation  
**Implementation language:** C23  
**Status:** Opt-in educational qualification; *not* a separately trained language-model decoder, a new checkpoint format or an inference speedup.

## 1. Why this step matters

Batch 85–86 proved a differentiable, tape-connected rotary attention composition. The next question is whether the same path can perform an actual parameter update, preserve its training-state boundaries and report an independently recomputed loss after the update. A mathematical derivative without an observable update is not yet a complete training exercise.

Batch 87–88 introduces `UmiAiTrainingRotaryOptimisationRun`, a Framework-owned **complete bounded training experiment**. It reuses the Framework's five major mathematical owners:

1. `UmiAiTrainingMatmul` for query/key projections;
2. `UmiAiTrainingRotaryTapeAttention` for the existing, bounded RoPE training graph;
3. `UmiAiTrainingMeanSquaredError` for the scalar teaching objective;
4. `UmiAiTrainingBackward` for reverse-mode automatic differentiation;
5. `UmiAiTrainingSgdStepMany` for validated two-matrix parameter updates.

It does **not** copy, rename, delete or replace `tape.c`, `transformer.c`, `decoder.c`, original position embeddings or checkpoint codecs. Training-tape lifecycle and mutable parameter ownership remain in the Framework.

## 2. The problem, using small numbers

A single experiment contains three token-feature rows, each with four float32 channels, arranged into two attention heads of two channels each. Both query and key projection matrices are 4×4 trainable float32 tensors. Values, desired attention outputs and token features are fixed. The model uses a declared absolute start position and the existing RoPE frequency base.

Mathematically:

- `Q = X Wq`, where X has shape 3×4 and Wq has shape 4×4, yielding Q with shape 3×4;
- `K = X Wk`, similarly yielding K with shape 3×4;
- existing RoPE attention rotates Q and K before masked multi-head attention with the fixed V tensor;
- `MSE = sum((actual - desired)^2) / 12` is the mean squared error over the 3×4 result;
- `gradientWq` and `gradientWk` are tape-derived gradients;
- `Wnew = Wold - learningRate * gradientW` is a validated step applied to both leaves.

The experiment allows 1–8 parameter updates, and the caller supplies a finite learning rate in `(0, 1]`. These are deliberate educational bounds, not model hyperparameter recommendations.

## 3. Training sequence and evidence integrity

1. Validate the plan, numerical features, both matrices, frequency base and absolute token-position limits **before allocating a tape**.
2. Copy the five caller-owned inputs into five tape-owned leaf tensors, making only the two projection matrices trainable.
3. Build Q, K, rotary attention and MSE using the unchanged Framework operations.
4. Record the baseline loss, perform Backward and inspect **both** initial gradient magnitudes.
5. Use `UmiAiTrainingSgdStepMany` to update the two trainable projection matrices as one validated operation.
6. Rewind the tape to its original five leaf nodes, invalidating previous graph intermediates. The weight tensors themselves are preserved.
7. Compute the loss again with the **updated** weights; store it under the corresponding completed step.
8. Repeat for the requested number of steps, inspect the persistent-tape allocation high-water mark and copy the final weights into the staged result.
9. Only after every stage succeeds publish the whole result to the caller. On error, destroy the temporary tape and leave all caller-owned inputs and outputs unchanged.

`lossReduced` is a *reported observation* (`finalLoss < initialLoss`). The API must not assume this is always true: different supplied data or large learning rates can cause loss to rise.

### What the returned memory number means

`peakTapeBytes` comes from `UmiAiTrainingTapeMemoryInspect` and represents the **persistent training-tape ownership high-water mark**. It excludes unrelated process allocations, stack memory, allocator metadata, temporary scratch, window managers and GPU resources. Do not advertise it as physical RSS.

### What the results do *not* establish

A small numerical improvement on three artificial token rows is **not** evidence that a language model understands text, generalises to unseen instructions or is ready for financial decision-making. These tests do not measure inference throughput or replace end-to-end regression. The original learned-absolute-position decoder and checkpoints remain supported independently. The future RoPE decoder requires a new model-family identity and checkpoint compatibility controls.

## 4. Public API and source map

- `include/umicom/ai/training/rotary_optimisation.h`: caller-owned problem/plan/result contracts and numeric limits.
- `src/ai/training/rotary_optimisation.c`: complete source-preserving Framework orchestration.
- `cmake/UmicomNativeLlmRotaryOptimisation.cmake`: additive CMake registration; no historic targets removed.
- `learning/native-llm/prefix-work/train_rotary_attention.c`: small native C23 educational client.
- `tests/ai_training/test_training_rotary_optimisation_convergence.c`: reproducibility, repeated updates, output limits and final fresh-forward evidence.
- `tests/ai_training/test_training_rotary_optimisation_rejections.c`: invalid inputs, untouched caller output.

### What is intentionally left for future batches

A dedicated fused rotary operator could reduce the number of tape nodes built per row. It must first prove correct forward, adjoint and memory behaviour against the existing, slow but inspectable composition. This batch does **not** claim that optimisation. It qualifies the current training sequence before performance-oriented modifications are attempted.

A further stage will need a separately versioned RoPE decoder architecture, token-level next-token cross-entropy, train/evaluate pipeline, independent numerical parity, and a new checkpoint codec. Existing model weights must never be silently reinterpreted.

## 5. Regression and local acceptance

From a Windows UCRT64 PowerShell session, configure the isolated native LLM prefix project and run the `framework.ai.training.rotary_optimisation_` CTest pattern. Compare observed loss, result reproducibility and failure-atomic outputs. Verify the complete native prefix laboratory and consolidated application tree independently before committing.

A compiler warning, numerical mismatch, test marked *Not Run*, or missing executable is a failure requiring investigation. Never convert those outcomes into an unsupported 100%-pass claim.

This release's local Linux qualification, if included in the private guide, may use deterministic **test-only, API-compatible tape substitutes**. That does *not* establish Windows or complete production-tape integration; the user must validate those targets on the actual repository and compiler.

## 6. The companion Mathematics for AI path

Three new accessible Units 10–12 extend the existing nine-unit course without deleting or rewriting its earlier instructional material:

- **Unit 10 — Gradient descent and learning rates:** changes to a single number, gradients, step size and checking new loss.
- **Unit 11 — Matrix weights and training:** an exact two-input numerical example, matrix gradients, one SGD step and recomputed error.
- **Unit 12 — Finite differences and floating-point:** independent derivative estimates, numerical tolerances and the limits of timing and training claims.

In native LLM **Lesson 87**, readers follow query/key projections through the real tape. **Lesson 88** teaches evidence review, repeatability, memory interpretation and accepted error conditions. Each lesson links to the mathematical foundation at the point where a symbol first matters. The additional `recent-lessons-69-88.html` page makes all recent lessons discoverable without replacing the older original course map.

**Teaching rule:** Explain *what a formula is for*, *what every symbol means*, *why the units and shapes matter*, then calculate at least one concrete numerical example. Maintain accessible, professional wording; no assumed university mathematics background. Learners can return to the engineering lesson immediately after completing the relevant maths unit.

---

**Engineering boundary:** Preserve all original code, authorship, comments, licences, APIs and workflows. The Framework remains the sole owner of reusable runtime and mathematical functionality. Applications and educational frontends remain thin.
