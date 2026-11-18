![Umicom](../../resources/brand/umicom-logo.svg)

# Explicit RGB orientation before tensor packing

Use `UmiAiTrainingImageFlipRgb8` when a native caller has already selected a horizontal or vertical image transform and needs a bounded packed RGB copy. The function shares shape admission with image packing and accepts the same borrowed view used by crop composition. It does not choose an augmentation policy or transform labels.

## Start with the correct representation

Include `umicom/ai/training/image_flip.h`. Input is RGB8: three unsigned bytes per pixel in red, green, blue order. Rows run top to bottom and may contain padding. A crop can borrow a subset of a larger image while retaining the original row stride. Keep the allocation and descriptor stable for the entire synchronous call.

The helper is compiled into `Umicom::ai` and the native `Umicom::AiTraining` target from the same source. Installed package discovery and ABI compatibility need their own qualification. The Bits2Banking image-orientation laboratory builds a narrow source consumer for teaching; it is not an installed-SDK test.

## Select the transform explicitly

| Flags | Result |
| --- | --- |
| 0 | Packed identity copy. |
| UMI_AI_TRAINING_FLIP_HORIZONTAL | Reverse pixels within each row. |
| UMI_AI_TRAINING_FLIP_VERTICAL | Reverse row order. |
| Both flags joined with bitwise OR | Both operations, equivalent to a 180-degree orientation change. |

Unknown bits return INVALID_ARGUMENT. Each pixel keeps its RGB channel order. No 90-degree rotation, resize, color conversion, alpha handling, normalization or random choice occurs.

~~~c
const uint8_t pixels[] = {255,0,0, 0,255,0};
const UmiAiTrainingRgb8View view = {pixels, sizeof pixels, 2U, 1U, 6U};
uint8_t output[6];
UmiStatus status = UmiAiTrainingImageFlipRgb8(
    &view, UMI_AI_TRAINING_FLIP_HORIZONTAL, output, sizeof output);
/* On OK: output is 0,255,0, 255,0,0. */
~~~

## Capacity and ownership

The output contains exactly width*height*3 bytes with packed stride width*3. Final and inter-row input padding is not copied. Byte capacity is not float capacity. The input descriptor, readable pixel span and writable output span must be valid, stable and disjoint. In-place flipping is not supported. No pointers are retained and no memory is allocated.

Shared ImageDescribeRgb8 admission checks positive dimensions, multiplication/addition overflow, sufficient stride and the maximum value count. The caller must also provide the complete declared backing allocation; metadata checks cannot prove a pointer’s lifetime or allocation size. All spans and capacity are admitted before the first read/write. Refusal leaves every output byte unchanged. Success leaves the unused output tail unchanged.

A single-row source needs no final padding, even if its stride is large. A multi-row source must cover every row start plus the last visible row. These cases are different; do not reject or accept them solely from a stride value.

## Compose with crop and packing

1. Admit a source and select a borrowed crop with ImageCropRgb8.
2. Keep the original allocation live while the crop is used.
3. Call ImageFlipRgb8 with the selected flags and a disjoint byte destination.
4. Treat the result as packed RGB with stride crop.width*3, not the original padded stride.
5. Call ImagePackRgb8 with the actual model’s layout and numeric range and a disjoint float destination.
6. Pass the tensor to a separately specified model/training operation.

Reusing the crop’s old stride for the new packed bytes is an error. Serializing the crop’s entire readable span as visible pixels is another error. The teaching laboratory exercises both boundaries with a padded source and a two-by-two crop.

## Training policy belongs to the caller

A mathematically correct mirror can invalidate left/right captions, text, laterality, bounding boxes or segmentation masks. Related video frames generally need a consistent spatial policy. The function cannot inspect those meanings. Record the chosen policy and maintain source-group identity so transformed copies do not cross training/evaluation boundaries unnoticed.

Do not describe a successful transformation as model training or evidence of improved quality. A training loop still needs a model, loss, gradients, optimizer, evaluation protocol and memory budget.

## Focused verification

From the Framework root, run each PowerShell line separately and stop on errors:

~~~powershell
$env:Path = "C:/msys64/ucrt64/bin;$env:Path"
cmake -S tests/image_flip -B build/image-flip -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe
cmake --build build/image-flip --parallel 2
ctest --test-dir build/image-flip --output-on-failure
~~~

Eight cases cover literal asymmetric transformations, padding, borrowed crops, CHW tensor composition, transactional refusal, invalid/overflowing metadata, the maximum shape boundary and repeated-flip restoration across a grid. Literal fixtures are needed alongside the restoration property because a wrong operation can sometimes undo itself too.

If configuration reports an incomplete source set, integrate matching complete headers and implementations and configure again. Do not replace missing files with empty placeholders or accept a stale executable as current evidence. Use the generative-AI lesson “Transform pixels without changing their labels by accident” for the illustrated beginner walkthrough.
