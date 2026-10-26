# Native multi-layer Transformer in Umicom Framework

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT

This learning laboratory executes a real stacked decoder using the Framework's existing trainable Transformer block, embeddings, tape, selected-token cross-entropy and transactional SGD. It does not replace the original educational decoder. Its checkpoint format has a distinct magic identifier and must never be treated as a legacy decoder checkpoint.

## Build and test

On Windows with MSYS2 UCRT64, from `C:\umicom\Umicom-Applications`:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S .\framework\learning\native-ai-stacked -B .\build\windows-ucrt64-stacked-debug -G Ninja -DCMAKE_C_COMPILER=C:\msys64\ucrt64\bin\gcc.exe -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build .\build\windows-ucrt64-stacked-debug --parallel 2
ctest --test-dir .\build\windows-ucrt64-stacked-debug -R '^framework\.ai\.stacked\.' --output-on-failure --no-tests=error
```

Run `umicom-native-stacked-demo.exe` in the build directory to inspect the model, train a few synthetic windows, export an in-memory checkpoint and compare the restored logits.

The model is an intentionally bounded CPU teaching implementation. Multi-layer backpropagation is real, but capacity remains limited by the existing tape's per-tensor, node and allocation ceilings. Its absolute-position semantics require recomputation after a sliding-window rollover; **no multi-layer KV-cache speedup is claimed**.

The snapshot codec uses little-endian integers, IEEE-754 FP32 and Framework SHA-256. Hash agreement is not publisher authentication. Authorised source review, private-data controls and signed promotion are separate responsibilities; do not feed unreviewed or confidential data into the laboratory.

`UmiAiTrainingStackedExperimentRun` accepts already-approved caller-owned token windows, verifies that original-source hashes do not cross train/evaluation partitions, shuffles training examples reproducibly and reports selected-target-weighted losses. Model weights are incrementally updated and a failed multi-epoch experiment is **not** automatically rolled back; take a trusted model snapshot before an experiment if needed.
