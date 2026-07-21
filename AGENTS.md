# Agent Instructions: Alientek i.MX6ULL Mini Box

Embedded Linux learning repository targeting the Alientek i.MX6ULL Mini development board.

## Project Structure

```text
alientek-imx6ull-mini-box/
├── 01_bare_metal/      Bare-metal programs
├── 02_bootloader/      U-Boot source and board porting
├── 03_linux_kernel/    Linux kernel source and device trees
├── 04_rootfs/          BusyBox root filesystem
├── 05_linux_driver/    Linux drivers
├── docker/             Cross-compilation container environment
├── docs/               Learning notes
├── .idea/              CLion / IDE configurations
├── README.md           Project overview
├── .gitignore
├── .gitattributes
└── LICENSE
```

### 1. Bare Metal (`01_bare_mental`)

Direct hardware execution without an OS.

### 2. U-Boot (`02_bootloader/uboot_imx`)

Customized U-Boot with Alientek board adaptations (LCD, HDMI, VGA, eMMC, NAND).

- **Key Board Files**: `configs/mx6ull_alientek_{emmc,nand}_defconfig`, `board/freescale/mx6ullevk/`, `include/configs/mx6ullevk.h`.

### 3. Linux Kernel (`03_linux_kernel/linux_alientek`)

- **Configs**: `arch/arm/configs/imx_alientek_{emmc,nand}_defconfig`, `imx_v7_defconfig`.
- **Device Trees**: `arch/arm/boot/dts/imx6ull-14x14-{emmc,nand}-*.dts` (supports 4.3"/7"/10.1" LCDs, HDMI, VGA, onboard sensors).
- **Build Script**: `build.sh` compiles `zImage`, DTBs, and kernel modules into `tmp/`.

### 4. Root Filesystem (`04_rootfs/busybox_1.29.0`)

Generates core system utilities (`sh`, `ls`, `mount`, `ifconfig`).

### 5. Linux Drivers (`05_linux_driver`)

Reserved for driver development modules.

### 6. Docker Environment (`docker`)

Provides a consistent GCC Linaro 4.9.4 cross-compilation toolchain on Ubuntu 16.04. Pre-configures `ARCH=arm` and `CROSS_COMPILE=arm-linux-gnueabihf-`.

---

## Build and Test Commands

### 1. Repository Setup

Ensure Git LFS binaries are fetched:

```bash
git lfs install
git lfs pull
```

### 2. Build Docker Image

```bash
docker build -t alientek-imx6ull:latest ./docker
```

### 3. Start Development Container

- **Linux / macOS / WSL**:

```bash
docker run -it --name imx6ull-dev \
  --mount "type=bind,source=$PWD,target=/project" \
  --workdir /project \
  alientek-imx6ull:latest
```

- **PowerShell**:

```powershell
docker run -it --name imx6ull-dev `
  --mount "type=bind,source=$($PWD.Path),target=/project" `
  --workdir /project `
  alientek-imx6ull:latest
```

- **Re-enter container**: `docker start -ai imx6ull-dev`

### 4. Module Build Commands

#### Bare Metal (CMake)

```bash
cd /project/01_bare_metal/06_buzzer
mkdir -p build && cd build
cmake ..
make -j"$(nproc)"
```

#### U-Boot

- **eMMC Version**:

```bash
cd /project/02_bootloader/uboot_imx
make distclean
make mx6ull_alientek_emmc_defconfig
make -j"$(nproc)"
```

- **NAND Version**: Replace target config with `mx6ull_alientek_nand_defconfig`.
- **Automation Script**: `./build.sh` (uses `mx6ull_14x14_ddr512_emmc_defconfig`).

#### Linux Kernel

- **eMMC Version**:

```bash
cd /project/03_linux_kernel/linux_alientek
make distclean
make imx_alientek_emmc_defconfig
make -j"$(nproc)" zImage modules
make -j"$(nproc)" imx6ull-14x14-emmc-4.3-480x272-c.dtb
```

- **NAND Version**: Replace with `imx_alientek_nand_defconfig` and corresponding DTB target.
- **Automation Script**: `./build.sh` (uses `imx_v7_defconfig`).

#### BusyBox

```bash
cd /project/04_rootfs/busybox_1.29.0
make distclean
make defconfig
make -j"$(nproc)"
make CONFIG_PREFIX="$PWD/output/rootfs" install
```

### 5. One-Shot Docker Execution

Run builds non-interactively from the host terminal:

```bash
# Bare Metal
docker run --rm -it -v "$PWD:/project" -w /project alientek-imx6ull:latest bash -c "cd 01_bare_metal/06_buzzer && mkdir -p build && cd build && cmake .. && make -j\$(nproc)"

# U-Boot
docker run --rm -it -v "$PWD:/project" -w /project alientek-imx6ull:latest bash -c "cd 02_bootloader/uboot_imx && make distclean && make mx6ull_alientek_emmc_defconfig && make -j\$(nproc)"

# Linux Kernel
docker run --rm -it -v "$PWD:/project" -w /project alientek-imx6ull:latest bash -c "cd 03_linux_kernel/linux_alientek && make distclean && make imx_alientek_emmc_defconfig && make -j\$(nproc) zImage modules dtbs"

# BusyBox
docker run --rm -it -v "$PWD:/project" -w /project alientek-imx6ull:latest bash -c "cd 04_rootfs/busybox_1.29.0 && make distclean && make defconfig && make -j\$(nproc) && make CONFIG_PREFIX=\$PWD/output/rootfs install"
```

### 6. Build Order

`Docker Image` -> `U-Boot` -> `Linux zImage & DTB` -> `Kernel Modules` -> `BusyBox Rootfs` -> `Target Deployment`

---

## Code Style & Formatting Guidelines

Strictly adhere to the *Linux Kernel Coding Style*- (`Documentation/process/coding-style.rst`):

- **Indentation**: Use 8-character tabs for indentation. Do not use spaces.
- **Line Length**: Target limit of 80 columns per line.
- **Brace Placement**:
  - Functions: Place the opening brace on a new line.
  - Control Statements (`if`, `for`, `while`, `switch`): Place the opening brace on the same line.
- **Naming Conventions**: Use lower-case `snake_case` for variables, functions, and structures. Avoid camelCase or Hungarian notation.
- **File Header Standard**:
  - Declare the license explicitly using SPDX tags at the top of every source/header file:

```c
// SPDX-License-Identifier: MIT
```
  - **DO NOT**- include `@author` or `@date` tags in file headers. Version control tracking must be handled strictly by Git.

- **Commenting Standard**:
  - All comments **MUST*- be written in English.
  - Comment modified or newly added code logic explicitly where non-obvious.
  - Use standard C block comments `/- ... */` for multi-line descriptions.

---

## Agent Behavior & Automation Rules

### Output Protocol

- Simplest working solution. No over-engineering.
- Code First: Return modifications, file creations, or script blocks first.
- Minimal Explanation: Provide explanations after the code block only if the execution logic is non-obvious.
- No Prose: Do not include conversational filler, greetings, or inline commentary.
- Comments: Use code comments sparingly, only where logic is highly ambiguous. Do not generate docstrings or type annotations for unchanged/existing code.

### Review Protocol

- State the exact bug.
- Show the explicit fix.
- Stop. Do not offer broader architectural suggestions, alternatives, or compliments.

### Debugging Protocol

- Never hypothesize or speculate about a bug without reading the relevant source files or logs first.
- Clearly state: What was found, where it was located, and the precise fix in a single pass. If the cause is indeterminate, state it explicitly without guessing.

### Formatting Guardrails

- Use plain hyphens and straight quotes (`'` or `"`) only.
- Do not emit smart quotes, em dashes, or decorative Unicode shapes.

### Git Workflow Protocol

- Atomic Commits: Commit early and often. Create a separate commit for each logical change or calculation step. Do not bundle multiple unrelated structural or content updates into a single monolithic commit.
- Conventional Commits: Every commit message must strictly follow the Conventional Commits specification (e.g., `feat(driver): add gpio initialization`, `fix(uboot): resolve emmc clock divider`).
- Summarization: The commit message summary line must be highly descriptive and precise, capturing the exact engineering or structural change made.

### Search & Analysis Infrastructure

- Efficient Tooling: Utilize high-performance search utilities such as `ripgrep` (`rg`) and `ast-grep` for scanning codebase patterns and repository text when gathering context.
- Dependency Missing Protocol: If these external search utilities are missing from the system path, immediately pause execution and explicitly prompt the user to install them before attempting any file modifications.

### Content Preservation & Safety

- Additive Bias: Prioritize adding new content, configurations, and scripts. Avoid deleting or overriding existing functional code or prose unless strictly necessary.
- Destructive Change Safeguard: Never silently delete or heavily modify pre-existing logic or text. If code or content appears incorrect, pause and prompt the user for confirmation, allowing them to decide whether to delete or modify it.
- Post-Modification Accountability: After executing any user-approved deletion or modification, explicitly summarize exactly what was changed or removed and state the outcome.
