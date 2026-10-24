# Umicom Native LLM — Rotary Gradient Qualification (Lessons 83–84)

**Author:** Sammy Hegab — Umicom Foundation  
**Implementation:** native C23  
**Licence:** MIT  
**Status:** opt-in, bounded mathematical research; not a trained rotary decoder.

## 1. Motivation and repository responsibility

The prior native rotary research introduced a position-aware first-block attention ring. This delivery qualifies a missing *training prerequisite*: reverse-mode gradients through a RoPE transformation. Model weights, optimiser steps, checkpoints and existing generation algorithms remain owned by established Framework services. There is no separate C++/Python neural engine and no application-local mathematics.

All new reusable functions are in `include/umicom/ai/training/` and `src/ai/training/`; the learner demonstration only calls them. The separate `UmicomNativeLlmRotaryTraining.cmake` module adds the implementation and tests to `umicom_ai_training` after the existing rotary research module. The normal prefix laboratory includes both modules. It does not change the established root native LLM build, legacy decoder, or normal training checkpoint codec.

## 2. Mathematical boundary

The existing Framework `UmiAiTrainingRotaryRotate` applies a 2D orthogonal rotation to pairs of projected query/key channels, leaving the unrotated tail unchanged. The new `UmiAiTrainingRotaryRotateAdjoint` computes its Jacobian-transpose action on a caller-owned upstream gradient. For pair angle `theta`:

```text
forward:   x' = x cos(theta) - y sin(theta)
           y' = x sin(theta) + y cos(theta)

backward:  dL/dx = dL/dx' cos(theta) + dL/dy' sin(theta)
           dL/dy = -dL/dx' sin(theta) + dL/dy' cos(theta)
```

This function uses the same absolute-position and frequency convention as the established forward kernel. It stages the complete float32 output before committing it. In-place execution is supported.

For `score = dot(R(pq) query, R(pk) key)` and scalar upstream `g = dL/dscore`, the new `UmiAiTrainingRotaryDotBackward` computes

```text
dL/dquery = g R(pq)^T R(pk) key
dL/dkey   = g R(pk)^T R(pq) query
```

using only the shared forward/adjoint rotation functions. The dot is **unscaled**. The enclosing training-tape attention operator must continue to own softmax, attention scaling, value mixing and their gradients. Calling this scalar gradient a complete attention backward implementation would be incorrect.

### Limits and failure semantics

- `headChannels` is 2..32 and `rotaryChannels` is even and 2..headChannels.
- `absolutePosition` is at most `UMI_AI_ROTARY_MAX_POSITION` (1,048,576).
- Input vectors must be finite; output values must be representable in float32.
- The two output ranges for query/key gradients must not overlap, including partially. Input-to-output aliasing is permitted because gradients are staged.
- All rejected operations leave the caller's output buffers unchanged.
- `UmiAiTrainingRotaryTensorForward` and `...Backward` operate on a bounded logical `[batch, time, heads, headChannels]` tensor with `time <= 32`, at most 32 total channels and at most 16,384 float elements.
- Each batch sequence has its own absolute start position. NULL starts with `startCount=0` means every independent sequence starts at position zero.
- The batch functions call the established scalar kernel for every head and row; they do not duplicate its angle/sine/cosine implementation.
- All state is caller-owned except short-lived staged heap memory; no weights, checkpoints, RNGs, files or globals are mutated.

## 3. How the proof works

The `rotary_gradients` regression compares analytical scalar-dot query and key derivatives with **independent central finite differences** using the established forward score function. It tests seven rotary shapes and eight pairs of absolute positions (56 configurations, with every input component of each configuration perturbed twice). Positions include zero, nonzero, widely separated and near the declared large-position bound. The numerical threshold is explicit in the source and belongs to these finite test inputs; it is not a general error bound for all trained networks.

The `rotary_tensor` regression compares every row/head against an independent call to the scalar owner. It also verifies the adjoint inner-product identity:

```text
  dot(TensorForward(x), g) ≈ dot(x, TensorBackward(g))
```

This test catches incorrect gradient direction even if outputs remain finite. It includes two sequences with different initial absolute positions and verifies in-place calls and independent-sequence resets.

The `rotary_training_rejections` regression checks shape/position/capacity errors, partial output overlap, very large upstream gradients and nonfinite inputs without accepting corrupt or partially published results. The educational `rotary_gradients_demo` executable prints an observed central-difference maximum error and refuses success when its bounded example exceeds tolerance.

## 4. Windows UCRT64 build and verification

All commands are from the existing local repository. The source delivery is a sparse directory tree whose top-level names match the local `C:\umicom` folders.

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S '.\framework\learning\native-llm\prefix-work' `
    -B '.\build\windows-ucrt64-native-llm-prefix-debug' `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Configuration failed.' }
cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
    --target umicom-ai-training-rotary-gradients-test `
             umicom-ai-training-rotary-tensor-test `
             umicom-ai-training-rotary-training_rejections-test `
             umicom-native-llm-rotary-gradients --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Rotary gradient build failed.' }
ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
    -R '^framework\.ai\.training\.rotary_(gradients|tensor|training_rejections|gradients_demo)$' `
    --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Rotary regression failed.' }
```

Next build and test the **entire** native prefix laboratory on the Windows machine. The isolated external test harness used for pre-delivery verification is **not** included in the public ZIP; it contains a test-only replacement of the public status enumeration (not a replacement of any rotary numerical implementation). The full production decoder, signed checkpoint system, Studio integration and Windows UCRT64 build are not claimed to have been executed in this environment.

## 5. What is still required for a separately trained RoPE model

1. Integrate the qualified rotation forward/adjoint as a **Framework training-tape operator** with real graph nodes, provenance checks, memory budgeting and rollback. The existing tape owner must remain authoritative.
2. Compose one dedicated decoder architecture that applies RoPE after Q/K projections, not legacy learned positional embeddings, and train it against canonical dataset and optimiser services.
3. Define a distinct, tested checkpoint family or format with explicitly tagged position strategy. **Never** reinterpret an old learned-absolute checkpoint as rotary-compatible.
4. Verify full-model forward logits and backpropagation numerically; include reproducible training, portable checkpoint resume and inference/training parity.
5. Reassess sliding-window KV correctness across **all** model layers. RoPE stabilises absolute Q/K coordinates, but hidden states in deeper layers may depend on tokens that have been evicted. Do not infer whole-decoder parity from a proven first-layer ring.
6. Compare real CPU time, memory and cache rebuilds against an independently specified decoder reference. No speedup is guaranteed before measurement.

The project remains educational. Do not use it for production banking decisions or execute model-produced source code without independent approval controls.
