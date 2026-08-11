# UI boundary

- Product-neutral models/controllers: no GTK types.
- `Umicom::ui` and `Umicom::ui_components`: toolkit-neutral.
- `Umicom::ui_gtk4`: explicit GTK4 adapter and reusable GTK4 components.
- Product frontend: may use `Umicom::ui_gtk4` and raw GTK privately for product-specific widgets.
