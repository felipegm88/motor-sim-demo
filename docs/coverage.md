# Coverage

This repository enforces **100% line coverage** for its own `src/` (excluding `src/main.c`).
The CI fails if the coverage gate does not pass.

## Generate coverage and HTML report

Run the full test suite with coverage enabled:

```bash
west twister -T tests -p native_sim -v \
  --outdir twister-out-coverage \
  --coverage --coverage-tool gcovr --coverage-formats html
```

Open the HTML report:

- `twister-out-coverage/coverage/index.html`

## Terminal summary and 100% gate

Generate a terminal summary and enforce 100% line coverage for this repository's code:

```bash
gcovr twister-out-coverage --root . \
  --filter '^src/' \
  --exclude '^src/main\.c$' \
  --exclude '^tests/' \
  --print-summary \
  --fail-under-line 100
```

Notes:

- `--filter '^src/'` keeps only this repo's code (Zephyr sources are ignored).
- `--exclude '^src/main\.c$'` removes the demo entrypoint from the metric (it mostly initializes and then loops forever).

## Branch coverage vs line coverage

- **Line coverage** answers: “Did we execute this line at least once?”
- **Branch coverage** answers: “Did we execute all outcomes of each decision?” (true/false branches, switch cases, short-circuit paths, etc.)

Branch coverage is usually harder to push to 100% in embedded-style code without explicit fault injection to trigger rare error paths.
