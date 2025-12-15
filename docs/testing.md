@page testing_page Testing

This project uses Zephyr's **Twister** test runner and **ztest**.

@section run_all Run all tests

```bash
west twister -T tests -p native_sim -v
```

@section run_unit Run unit tests

```bash
west twister -T tests/unit -p native_sim -v
```

@section run_integration Run integration tests

```bash
west twister -T tests/integration -p native_sim -v
```

Twister will create output reports under `twister-out/` (or the custom `--outdir` you specify).
