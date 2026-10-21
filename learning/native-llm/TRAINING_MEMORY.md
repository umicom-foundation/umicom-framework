# Native Decoder Training — Capacity Planning and Memory Budgets

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT  
**Implementation:** C23 · **Framework ownership:** `include/umicom/ai/training/`

## Purpose and engineering scope

The native learning decoder is deliberately bounded. It has a fixed maximum
number of tape nodes and a maximum number of scalar elements per tensor. These
constraints are helpful for inspecting a first transformer but must not be
confused with computer RAM capacity. A configuration can fail because a
matrix exceeds the per-tensor element limit even when the computer has many
gigabytes available.

Before increasing those limits, we must measure where memory is allocated,
prove that failed allocations cannot corrupt training state, and distinguish
weight storage from transient training data. This module provides:

- **Read-only capacity planning** from the decoder's authoritative parameter
  shapes, rather than an application-maintained duplicate model definition.
- **Committed training graph accounting** for the tape object, node metadata,
  value and gradient arrays, embedding token-index arrays and saved attention
  probabilities.
- **A configurable committed-graph budget**, which rejects publication of a
  computed tensor that would exceed the selected limit.
- **A high-water mark** that records the largest successfully published graph,
  including temporary activations before rewind.
- **Decoder-level accessors** that do not expose or transfer tensor ownership.

These are reusable native capabilities, not a change to the model architecture
or a claim of production-scale training.

## What we count — and what we do not

A leaf tensor with `N` float32 elements retains an owned allocation for its
metadata, `N × 4` bytes of values and `N × 4` bytes of gradients. A derived
attention tensor additionally retains a probability buffer. An embedding
lookup or cross-entropy operator also retains a copy of its input indices.

The memory inspector includes the tape object and all **live** committed
nodes. It does **not** include temporary scratch arrays, temporary unpublished
nodes computed immediately before admission, allocator bookkeeping, model
objects outside the tape, stack frames, memory-mapped files or process RAM.
Therefore the budget is a bound on **persistent committed tape allocations**,
not a hard operating-system memory limit. Use a process-level memory observer
when investigating total RAM and swap use.

The tape starts without a committed-graph limit. A nonzero budget can be set
to the current committed allocation or higher; zero disables the restriction.
A tensor is computed in private storage and only published if its persistent
allocation can be admitted. On budget refusal, the temporary tensor is freed,
no new graph node appears and the caller's output tensor handle is unchanged.

### Memory lifetime

```text
Initialise tape
    ↓  base allocation (tape metadata)
Create trainable parameters
    ↓  values + gradients + node metadata
Forward pass
    ↓  embeddings, attention, activations and logits accumulate
Loss and backward pass
    ↓  gradient values update in their already-owned arrays
Optimiser step
    ↓  updates trainable parameter values
Rewind derived nodes
    ↓  temporary committed allocations released
Persistent parameter tensors remain until model destruction.
```

If an operator fails after creating some graph nodes, the existing Framework
rollback/rewind contracts release the temporary suffix without discarding the
trained parameter prefix. The high-water mark still describes a prior successful
peak, whereas the current allocation returns to its previous size.

## Planning a feasible configuration

`UmiAiTrainingCapacityPlanCompute()` uses the existing model's parameter
schema and tape dimension constraints. For example, with vocabulary 260,
context 8, channel width 8, hidden width 16 and two attention heads:

| Quantity | Value |
| --- | ---: |
| Parameter tensors | 15 |
| FP32 parameter values | 4,792 |
| Parameter value storage | 19,168 bytes |
| Parameter gradient storage | 19,168 bytes |
| Batch token count (one sequence) | 8 |
| Output logits | 8 × 260 = 2,080 floats |
| Logits payload | 8,320 bytes |
| Attention probabilities | 2 × 8 × 8 = 128 floats |
| Attention probability payload | 512 bytes |
| Maximum sequences in this exact configuration | 1 |

The per-tensor limit is **4,096 scalar elements**. Two context-eight sequences
would require `16 × 260 = 4,160` logit values and therefore cannot be admitted.
Choosing a larger batch without changing the actual tensor engine would be
misleading; the planner explicitly rejects it.

The sum of named parameter/logit/attention payloads is only a **lower bound**:
it does not include all temporary matrices, node objects and auxiliary buffers.
Do not set a runtime memory budget equal to that lower bound expecting training
to succeed. Use the live tape inspector to measure the actual working graph.

## Run the native inspection tool

From PowerShell, after merging the source files into your local checkout:

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-llm" `
      -B ".\build\windows-ucrt64-native-llm-debug" `
      -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON

cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2

ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
      --output-on-failure --no-tests=error

& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-training-memory.exe"
```

The executable reports the capacity plan, committed parameter memory, loss
of a small training step, committed memory after rewind and the largest
observed committed graph. It does not save a checkpoint or modify source files.

## Test boundaries

The regression tests validate the same public contracts used by Umicom Studio
and the native LLM application:

- A limit just above the live parameter allocation refuses a derived tensor
  while preserving its caller's output pointer and the existing graph.
- Releasing a computed graph suffix decreases committed memory, but the peak
  remains available for diagnostics.
- Embedding indices, cross-entropy targets and causal-attention probability
  arrays are included in auxiliary allocation counts.
- A rejected decoder training operation does not change trained weights,
  completed steps, reported loss or an existing checkpoint.
- The capacity planner rejects infeasible tensor/batch combinations before
  allocating a model and leaves the caller's plan untouched.

## Next steps for genuine scale

Larger specialised experts will require more than adjusting a constant. Before
changing the public tensor limits, qualify separately: shape-dependent tensor
workspaces, live activation counts, training-graph recomputation or gradient
checkpointing, multi-sequence attention memory, checkpoint format limits,
optimiser state, and realistic CPU/GPU memory ceilings. Use independent
numerical-gradient and decoder-parity tests when introducing new operators.

The present system remains a **small CPU teaching model**. Memory budgeting is
an engineering control and measurement foundation, not a claim that arbitrary
vocabulary sizes, many decoder layers or long training contexts are supported.
