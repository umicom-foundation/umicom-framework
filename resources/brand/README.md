# Common Umicom Brand Assets

Copy the existing canonical files from the current `umicom-studio-ide` repository into this directory during Repository Mega Batch R2:

```text
assets/umicom-icon.svg  -> resources/brand/umicom-icon.svg
assets/umicom-logo.svg  -> resources/brand/umicom-logo.svg
win/umicom-logo.png     -> resources/brand/umicom-logo.png
win/umicom.ico          -> resources/brand/umicom.ico
```

These four files become the Framework-owned common brand source of truth. Do not copy the old `umicom.rc`; Framework now supplies a reusable product-neutral template under `resources/windows/umicom-application.rc.in`.
