# Serial shell (native_sim)

When running `native_sim`, Zephyr connects the UART console/shell to a pseudo-terminal (PTY).

You will see a line like:

```bash
    uart connected to pseudotty: /dev/pts/3
```

## serial_shell_run Run (Terminal A)

```bash
    west build -b native_sim -d build
    ./build/zephyr/zephyr.exe
```

Keep this terminal open to watch logs and copy the printed PTY path.

## serial_shell_connect Connect (Terminal B)

Note: for PTYs the baudrate is not really “used”, but `115200 8N1` works fine.

picocom:

```bash
    picocom -b 115200 /dev/pts/3
```

minicom:

```bash
    minicom -D /dev/pts/3 -b 115200
```

screen:

```bash
    screen /dev/pts/3 115200
    (exit: Ctrl+A then K)
```

@section serial_shell_commands Useful commands

```bash
    help
    motor_info
    motor_set <rpm>
```

