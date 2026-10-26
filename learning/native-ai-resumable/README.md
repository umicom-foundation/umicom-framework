# Native AI: Resumable Supervised Training

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

This opt-in native C23 laboratory demonstrates an exact restart of *small,
stateless-SGD, decoder-projection adapter* training. It composes existing
Framework owners: training tape, frozen decoder, low-rank projection adapters,
reviewed source admission, byte tokeniser, deterministic dataset, supervised
loss mask and held-out evaluation. No alternative model, attention kernel,
optimiser or script is introduced.

## Why two checkpoint layers?

A decoder checkpoint holds the base model weights. The projection-adapter
snapshot holds independently trained low-rank weights for selected Q/K/V or
feed-forward projections, tied to the exact original decoder checkpoint SHA-256
and completed SGD steps. The continuation snapshot wraps that portable adapter
snapshot with the immutable dataset's ordered SHA-256, training plan, cursor
and selected aggregate metrics. Both snapshots have their own strict length,
version, little-endian fields and SHA-256 checksum.

The entire state can be restored only with the **same approved decoder and the
same ordered, reviewed dataset**. A truncated, altered or source-mismatched
snapshot is rejected before a restored adapter becomes visible. An unsigned
SHA-256 digest detects accidental damage; it does not prove publisher identity.
Authenticate and authorise external model and data files separately.

## One small deterministic training step

1. Start from a frozen decoder with `DecoderProjectionAdapterCreate`.
2. Admit a complete source-reviewed dataset through `SupervisedDatasetCreate`.
3. Call `SupervisedContinuationBegin` once with a validated epoch plan.
4. Call `SupervisedContinuationTrainNext` to consume exactly the next approved
   training example in the existing seeded permutation; only adapter weights
   are updated and only assistant-target positions enter the cross-entropy.
5. Once the epoch has consumed every example, call
   `SupervisedContinuationFinishEpoch` to apply the held-out evaluation schedule.
6. Export one complete in-memory checkpoint with
   `SupervisedContinuationSnapshotEncode`. Use established approved Framework
   storage services to persist it outside this educational demonstration.
7. Reconstruct the *same* approved dataset and frozen decoder. Restore the
   snapshot with `SupervisedContinuationSnapshotRestore`. Its owned adapter and
   next example offset exactly continue the earlier deterministic SGD sequence.

The original full-run `SupervisedExperimentRun` remains unchanged and is still
useful for complete uninterrupted experiments. Mid-run failures do not rewind
SGD updates already committed; never pretend that a failed run was rolled back.
The resumable cursor is durable **only if its matching adapter weight snapshot
was safely persisted**. A snapshot alone cannot provide an atomic filesystem
transaction: use the Framework's established file publication policy, rather
than overwriting a last-known-good checkpoint in place.

## Build and test

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-ai-resumable" `
    -B ".\build\windows-ucrt64-ai-resume-debug" -G Ninja `
    -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" `
    -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-ai-resume-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-ai-resume-debug" `
    -R '^framework\.ai\.(continuation|supervised|adaptation)\.' `
    --output-on-failure --no-tests=error
```

Run the optional demo executable only after a successful build. The focused
regressions compare an uninterrupted run with a run stopped after one example,
checkpointed, restored and completed; their final adapter snapshots must be
byte-identical on the same native build. They also exercise corruption,
foreign model identity, changed data ordering, unchanged failed output and
held-out quality gates.

## Limitations to retain

- The current experimental model is single-block, CPU-bounded and has learned
  absolute token positions. It is **not** a production LLM.
- Only SGD without moment state is exactly resumed here. A future AdamW trainer
  must save its moment tensors and schedule state; **do not** label this snapshot
  an AdamW continuation checkpoint.
- The report records quantitative token metrics; it is not a semantic evaluation,
  automated release approval or publisher signature.
- Provenance approval, personal-data controls, licence verification, near-
  duplicate detection, process lifetime, storage ACLs and disk publication
  remain separate mandatory boundaries.
