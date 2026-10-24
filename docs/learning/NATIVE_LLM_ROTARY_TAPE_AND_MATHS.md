# Native LLM research: training-tape RoPE and accessible mathematics

**Author:** Sammy Hegab · Umicom Foundation  
**Development delivery:** 85–86 · 24 October 2026  
**Implementation language:** native C23, Framework-owned, additive  
**Status:** bounded educational tape composition and separate nine-unit mathematics course. Not a new checkpoint family or complete RoPE decoder.

## 1. What changes

Previously the project had independently qualified array-level rotary forward
and backward operations. This delivery makes their results participate in the
**existing** Umicom training tape's differentiation and SGD workflow.

Public interfaces:

- `UmiAiTrainingRotaryTapeApply`: apply per-position RoPE to a projected tape tensor.
- `UmiAiTrainingRotaryTapeAttention`: rotate tape-owned queries and keys and use
  the existing multi-head causal attention operator for values and softmax.

Both use only public `UmiAiTrainingTape*` and `UmiAiTrainingTensor*` APIs.
They do not introduce a second autograd engine, a new optimiser, a checkpoint
codec, a new decoder, application-owned tensor storage, or script-based logic.

## 2. Graph composition and ownership

The existing tape does not expose a generic row-wise custom-operation API.
Rather than rewrite `tape.c` or modify opaque ownership, a deliberately bounded
composition supplies a differentiable bridge:

1. Query `UmiAiTrainingTensorShape` and validate rows/channels/heads/positions.
2. Save a tape graph checkpoint.
3. For each row, gather that row using `UmiAiTrainingEmbeddingLookup`.
4. Construct a **constant** `channels × channels` rotation matrix from the
   existing `UmiAiTrainingRotaryRotate` applied to each canonical basis vector.
   For row-vector multiplication this matrix is the transpose of the usual
   column-vector rotation.
5. Multiply the row by that matrix with the existing tape `Matmul`.
6. Restore that row to its original place using a one-hot column selector
   multiplied by the rotated row, then add all restored rows.
7. Publish the final tensor handle only when the full graph exists.
8. In the combined attention API, repeat for the key tensor, then call the
   **existing** `UmiAiTrainingMultiHeadCausalAttention`. Value tensor is left
   in its original representation.

Every node remains owned by the tape and participates in its established
`UmiAiTrainingBackward` graph traversal. The optimiser remains the established
`UmiAiTrainingSgdStep` or `UmiAiTrainingSgdStepMany`.

**Bounded scope:** no more than eight token rows, eight projected channels and
the global rotary position limits. Each single-tensor composition creates
`6 × rows − 1` tape nodes. These limits are important: this is intentionally
not an efficient general-purpose implementation. A later direct tape operator
may replace this composition **only after** independent numerical tests and
with explicit retention of superseded source under Umicom's preservation policy.

## 3. Failure semantics

The public result handle is not written until a complete operation succeeds.
Invalid configuration and position arrays are rejected before graph mutation.
Allocation or execution failures trigger `UmiAiTrainingTapeRollback` to the
recorded graph size. The original graph nodes are retained, but the *existing*
rollback contract also clears their previous gradients. A caller must run
`Backward` again before an optimiser step following a rollback.

No global RNG, trained parameter, checkpoint, filesystem path, or time-keeping
resource belongs to this bridge.

## 4. Numerical identity and independent evidence

For one rotary transform in column-vector form,

    y = R(p) x

and an upstream gradient of the loss L with respect to y,

    dL/dx = R(p)^T (dL/dy)

The actual tape composition uses **row vectors**, yielding the equivalent
forward matrix `M(p) = R(p)^T` and automatically deriving the transpose action
through tape `Matmul` backward.

Tests provided:

- `test_training_rotary_tape.c`: compare full rotated tensor output with the
  separately qualified array-level RoPE function; independently compare tape
  gradients with the separately qualified rotary adjoint; run an authorised
  existing-tape SGD update; and use central finite differences to check
  multiple query/key derivatives through multi-head causal attention.
- `test_training_rotary_tape_rejections.c`: reject invalid dimensions,
  inconsistent sequence offsets and overflow positions; force a tape memory
  budget failure after graph composition begins; verify previous graph size,
  current byte count and caller output handle remain unchanged.
- `inspect_rotary_tape.c`: tiny complete forward → loss → Backward → SGD
  demonstration with readable quantitative evidence.

**Evidence boundary:** local GCC/Clang qualification can exercise the new
source against an API-compatible training-tape substitute. This alone is
not evidence that every production tape or Windows UCRT64 build passes. Run
the three tests with the actual repository `tape.c` on your local computer
before integration approval. The substitute is test-only and excluded from
the release ZIP.

## 5. Build in Windows PowerShell

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S '.\framework\learning\native-llm\prefix-work' `
  -B '.\build\windows-ucrt64-native-llm-prefix-debug' -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Configuration failed.' }
cmake --build '.\build\windows-ucrt64-native-llm-prefix-debug' `
  --target umicom-ai-training-rotary-tape-forward-test `
           umicom-ai-training-rotary-tape-rejections-test `
           umicom-native-llm-rotary-tape --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
ctest --test-dir '.\build\windows-ucrt64-native-llm-prefix-debug' `
  -R '^framework\.ai\.training\.rotary_tape_' `
  --output-on-failure --no-tests=error
if ($LASTEXITCODE -ne 0) { throw 'Regression tests failed.' }
```

## 6. Maths for AI — separate foundations course

A new independent companion course is delivered in the *Bits2Banking*
repository at `content/courses/maths-for-ai/index.html`.

Its nine units teach counting and percentages, variables and functions,
vectors, matrices, probability and loss, derivatives, rotations, attention,
and how to translate formulas into actual C23. Every unit includes an
accessible illustration, plain-English symbol explanations, fully worked
numbers, one practice task and a revealable worked solution.

LLM Lessons 84–86 link directly to the appropriate maths units so a student
can leave an advanced lesson, study the specific concept, and return without
restarting the entire AI course. The new course also works offline and has
print styles. Previous lessons and the existing site navigation are preserved;
full catalogue integration of earlier lessons is a separate controlled update.

## 7. Roadmap and restrictions

The historical learned-absolute-position decoder and all old checkpoint
formats remain **unchanged**. RoPE inference in a different model family
requires separately trained weights and separately validated checkpoint
encoding, training/inference numerical parity, explicit tokenizer and
position policy, and attention-state correctness across multiple layers.

A direct tape-owned RoPE operator is the next performance engineering step,
not a silent replacement for the teaching composition. Context eviction in
multi-layer transformers may still invalidate cached upper-layer hidden states
when their inputs depended on evicted tokens; single-layer cache savings do
not establish exact full-window multi-layer equivalence.
