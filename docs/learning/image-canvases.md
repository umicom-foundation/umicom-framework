# Explicit RGB image canvases

![Umicom](../../resources/brand/umicom-logo.svg)

`UmiAiTrainingImageCanvasRgb8` places a complete decoded RGB8 view at an explicit origin on a larger packed RGB8 canvas. It preserves source pixel values and fills every other pixel with the three supplied colour bytes. It performs no decoding, resizing, cropping, colour conversion, training or implicit mask construction.

## Describe storage before placement

1. Keep the decoded pixel owner alive. Set the view pointer, accessible byte count, width, height and row stride accurately. Inter-row padding is allowed; final-row padding is not required.
2. Choose a positive output width and height. Use `UmiAiTrainingImageDescribeRgb8` to calculate the packed output requirement using a checked `width * 3` stride. The shared bounded training-image limit applies to the complete source and destination.
3. Allocate or provide a separate output buffer. Its byte capacity must cover the planned output. Keep its storage disjoint from the source pixels and descriptor.
4. Choose left and top so the whole source fits. The service rejects an out-of-bounds rectangle instead of cropping it.
5. Supply a readable three-byte RGB fill colour and call the service. Check its returned status before consuming the result. The colour is copied before writes, so those three bytes may be borrowed from old output storage.

```c
const uint8_t white[6] = {255,255,255,255,255,255};
const uint8_t black[3] = {0,0,0};
const UmiAiTrainingRgb8View source = {white,sizeof white,2,1,6};
uint8_t canvas[36];
UmiStatus status = UmiAiTrainingImageCanvasRgb8(
    &source,4,3,1,1,black,canvas,sizeof canvas);
```

The result contains twelve pixels: two original white pixels and ten black border pixels. Every failure leaves output unchanged; successful calls write exactly the packed output size and preserve unused tail capacity. No state or input pointer is retained. Callers must serialize access to these spans.

## Keep preprocessing and learning contracts separate

Canvas placement preserves source dimensions. A letterbox pipeline may resize first, then place that result, but must choose and record its sampling and aspect-ratio policy. Annotation coordinates need the same placement offset. Keep transformed copies of a source in the same dataset partition.

Padding pixels are actual model input. Under signed-unit conversion, black RGB bytes become -1, not zero. A separate loss mask can exclude selected target errors, but does not automatically prevent a model from observing border input. A latent-space model needs masks aligned with its own resolution and semantics.

## Focused qualification

Run from the Framework checkout with a C23 toolchain. Stop on a failed command.

```powershell
cmake -S tests/image_canvas -B build/image-canvas-checks -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/image-canvas-checks --parallel 2
ctest --test-dir build/image-canvas-checks --output-on-failure
```

The checks cover geometry, exact pixel values, padded rows, borrowed crops, colour lifetime, tensor conversion, capacity and metadata refusals. These source tests do not qualify an installed application, decoder or model training run.
