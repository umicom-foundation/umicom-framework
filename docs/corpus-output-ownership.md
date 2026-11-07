# Corpus output ownership

UmiAiCorpusWindow prepares input and next-byte target tokens from an immutable corpus. The corpus owns a copy of imported source bytes. The caller owns both output ranges.

## Contract

1. Supply a live corpus and two writable uint32_t arrays.
2. Express start and count in source bytes/pairs, and capacity in uint32_t elements for each output.
3. Leave room for the following byte: count must be at most corpus_length - start - 1.
4. Keep the first count output elements disjoint. Adjacent ranges in the same array are valid; partial overlap is refused in either direction.
5. Check the returned UmiStatus. A rejected request leaves both output ranges unchanged.
6. Destroy the corpus once when its owner finishes. Do not use copied handles after destruction.

The overlap check uses uintptr_t address ordering on supported flat-address hosts. It does not discover allocation sizes or establish pointer lifetime. The caller must still provide valid storage and serialize concurrent writes. The byte-count conversion and address-end additions are checked before the token loop.

## Source map

- [Public contract](../include/umicom/ai/training/corpus.h)
- [Corpus implementation](../src/ai/training/corpus.c)
- [Range regression](../tests/ai_training/test_training_corpus_ranges.c)
- [Native laboratory](../learning/native-llm/inspect_corpus_ranges.c)

The earlier equal-pointer-only implementation remains in a disabled review block. The active implementation preserves valid window behaviour and also rejects intersecting output ranges before either sequence is published.

## Try the laboratory

From the Framework root, configure learning/native-llm into a separate build directory. The focused targets use the same Framework training library as the parent build.

```powershell
cmake -S learning/native-llm -B build/corpus-ranges -G Ninja -DBUILD_TESTING=ON
cmake --build build/corpus-ranges --target umicom-native-llm-corpus-ranges umicom-ai-training-corpus-ranges-test --parallel 2
ctest --test-dir build/corpus-ranges -R "^framework\.ai\.training\.corpus_ranges(_demo)?$" --output-on-failure --no-tests=error
& ".\build\corpus-ranges\umicom-native-llm-corpus-ranges.exe"
```
Expected laboratory output begins with "Overlapping outputs: rejected, storage unchanged" and "Adjacent outputs: accepted". Four token pairs then follow: 101/102, 102/103, 103/104 and 104/105. These are expected values, not a recorded qualification result.

If CMake reports an unknown target, check that the source and build configuration describe the same checkout. If the regression fails, keep its first diagnostic and review the unchanged-output checks before changing the expected values.
