# Smart BIOS Update Tool

A lightweight CLI utility that simplifies BIOS flashing on Linux. It auto-detects `.rom` firmware files, validates filenames, and wraps the AMI `afulnx_64` flashing utility with safety checks — so you don't have to memorize long terminal commands.

## Features

- **Auto-detection** — scans the current directory for `.rom` / `.ROM` files automatically
- **Safe execution** — uses `fork()` + `execvp()` instead of `system()` to prevent shell injection
- **Filename validation** — rejects files with potentially dangerous characters
- **Multi-file menu** — if multiple firmware files are found, prompts you to choose
- **Failsafe exit** — cancels cleanly if no valid firmware file is present

## Prerequisites

| Item | Description |
|------|-------------|
| `afulnx_64` | AMI Firmware Update utility for 64-bit Linux |
| `.rom` file | BIOS firmware file matching your motherboard model |
| `gcc` | Any C compiler (only needed to build from source) |

## Setup

Place these three items in the same directory:

```
my-bios-update/
├── updateBIOS          # compiled binary
├── afulnx_64           # AMI flashing utility
└── MYBIOS.rom           # your firmware file
```

### Building from source

```bash
gcc -o updateBIOS updateBIOS.c
chmod +x updateBIOS
```

## Usage

```bash
cd /path/to/my-bios-update
./updateBIOS
```

The tool will prompt for your `sudo` password (characters won't be visible — this is normal) and then either auto-select the single `.rom` file or present a numbered menu if multiple are found.

## Warnings

> **Do NOT power off or close the terminal while the BIOS is being flashed.** Interrupting the process can permanently brick your motherboard.

- Always verify that the `.rom` file matches your exact motherboard model.
- This tool is a wrapper around `afulnx_64`. It does not modify firmware files — it only passes them to the AMI utility.

## How It Works

1. Scans the working directory for files ending in `.rom` (case-insensitive).
2. Filters out filenames containing unsafe characters.
3. If one file is found, selects it. If multiple, displays a choice menu.
4. Forks a child process and calls `sudo ./afulnx_64 <file> /b /p /n /x`.
5. Waits for completion and reports the result.

## License

This project is provided as-is for personal use. Use at your own risk.