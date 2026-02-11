# Alientek I.MX6ULL Mini Box - Docker 开发环境

这是一个专为正点原子 I.MX6ULL Mini 开发板打造的 Docker 开发环境。

> 理论上来说也支持正点原子 I.MX6ULL Alpha 开发板，但是你知道的，我没有这个板子，所以无法测试。

本项目通过容器化技术，将复杂的交叉编译工具链（GCC 4.9.4）、烧录工具（imxdownload）以及依赖库封装在一个独立的 Ubuntu 16.04 镜像中。无论宿主机是 Windows (WSL2)、macOS 还是 Linux，都能获得一致、纯净的开发体验，彻底告别环境配置的 “依赖地狱”。

## 📂 目录结构说明

这里仅对 `docker` 目录进行说明，忽略其他目录细节。

```plaintext
alientek-imx6ull-mini-box/
├── docker/
│   ├── Dockerfile              # 构建镜像的脚本
│   └── archives/               # 存放工具链和烧录工具的归档目录
│       ├── gcc-linaro-4.9.4...tar.xz  # 交叉编译器
│       ├── imxdownload                # 烧录工具 (Ubuntu版)
│       └── imxdownload_src.7z         # 烧录工具源码 (用于学习)
└── README.md
```

## 🚀 快速开始 (Quick Start)

### 1. 克隆仓库与准备文件

由于仓库中包含了较大的工具链文件，我们使用了 Git LFS (Large File Storage)。

```bash
# 1. 克隆仓库
git clone https://github.com/bfmhno3/alientek-imx6ull-mini-box.git

# 2. 进入目录
cd alientek-imx6ull-mini-box

# 3. 拉取 LFS 大文件 (确保 gcc tar 包被完整下载)
git lfs install
git lfs pull
```

### 2. 构建 Docker 镜像

使用仓库中的 `Dockerfile` 构建名为 `alientek-imx6ull` 的镜像。

```bash
# 注意：请确保你在包含 Dockerfile 的目录下，或者正确指定上下文路径
# 假设你在项目根目录，且 Dockerfile 在 ./docker 目录下：
docker build -t alientek-imx6ull:latest ./docker
```

### 3. 启动开发容器

建议使用 Clion、Visual Studio Code 等支持 Docker 容器的成熟的 IDE 开发，**对自己好一点**！

如果你因为某些原因一定要进入容器，建议使用 Docker Desktop 或者在终端执行以下命令：

```bash
docker run -it --name imx6ull-dev \
    -v $(pwd):/project \
    --privileged \
    alientek-imx6ull:latest
```

参数说明：

- `-v $(pwd):/project`：将当前目录挂载到容器内的 `/project`
- `--privileged`：获取硬件操作权限 (如果需要在容器内读写 SD 卡设备)

## 🛠️ 环境详情与原理

为了让你知其然更知其所以然，这里详细解释 `Dockerfile` 的构建逻辑。

### 阶段 1：构建者（Builder Stage）

我们使用 `debian:bullseye-slim` 作为轻量级的中转站。

- 任务：负责解压 `gcc-linaro-4.9.4` 交叉编译工具链，并处理 `imxdownload` 工具的权限。
- 目的：将脏乱的解压、移动操作隔离在第一阶段，避免最终镜像中包含无用的压缩包，减小体积。

### 阶段 2：最终环境（Final Stage）

我们选择 `ubuntu:16.04` 作为基础镜像。

#### 为什么是 16.04？

这旨在与正点原子官方教程及 NXP 旧版 BSP（板级支持包）保持 100% 的环境兼容性，从根本上杜绝因 `glibc` 版本、编译器差异或系统库缺失导致的问题，确保开发者拥有较好开发体验的同时能顺畅复现教程中的所有实验。

#### 工具链配置

- GCC：`arm-linux-gnueabihf-gcc`（4.9.4 版本，正点原子官方提供）已加入 `PATH`。
- `imxdownload`：位于 `/opt/imx-tools/`，用于为 `.bin` 文件添加头部信息以便 I.MX6ULL BootROM 识别。

> [!TIP]
>
> 已经预先定义了环境变量 `CROSS_COMPILE`，其值为 `arm-linux-gnueabihf-`。
>
> 这意味着在 CMake 中使用 `$ENV{CROSS_COMPILE}gcc` 的方式调用 `arm-linux-gnueabihf-gcc`。
>
> 同时也预先定义了其他相关的环境变量，例如 `OBJDUMP`

> [!CAUTION]
>
> 虽然镜像中已经设置了 `CC`、`CXX`、`CPP` 等环境变量指向交叉编译器，但是更推荐编写 `toolchain.cmake` 脚本指定交叉编译器，并在命令行中指定 `-DCMAKE_TOOLCHAIN_FILE`。

#### 预装依赖

- `build-essential`, `make`, `cmake`：基础构建工具
- `lzop`, `libncurses5-dev`：编译 U-Boot 和 Linux Kernel 必须的库。
- `bc`：内核编译需要的计算工具。

## 📝 常用命令指南

进入容器后，你可以直接使用以下命令：

### 1. 检查交叉编译器

```bash
arm-linux-gnueabihf-gcc -v
```

其输出为：

```bash
Using built-in specs.
COLLECT_GCC=arm-linux-gnueabihf-gcc
COLLECT_LTO_WRAPPER=/opt/gcc-arm/bin/../libexec/gcc/arm-linux-gnueabihf/4.9.4/lto-wrapper
Target: arm-linux-gnueabihf
Configured with: ...
Thread model: posix
gcc version 4.9.4 (Linaro GCC 4.9-2017.01)
```

> 为了避免篇幅过大，这里省略了 `Configured With` 后的内容。

### 2. 编译裸机程序

假设你有一个 `led.c`：

```bash
arm-linux-gnueabihf-gcc -g -c led.c -o led.o
arm-linux-gnueabihf-ld -Ttext 0X87800000 led.o -o led.elf
arm-linux-gnueabihf-objcopy -O binary -S -g led.elf led.bin
```

推荐使用 `CMake` 进行构建， 基础的 `CMakeList.txt` 如下所示：

```cmake
cmake_minimum_required(VERSION 3.5)
project(<your_project_name> LANGUAGES C ASM)

add_executable(${PROJECT_NAME} <your_src_files>)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-nostartfiles -T${CMAKE_SOURCE_DIR}/imx6ull.lds -Wl,--build-id=none"
    SUFFIX ".elf"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND $ENV{CROSS_COMPILE}objcopy -O binary -S -g $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND imxdownload ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${PROJECT_NAME}.imx
    COMMAND $ENV{CROSS_COMPILE}objdump -D $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dis
    COMMENT "Converting led.bin, led.imx and generating disassembly"
)
```

### 3. 生成最终烧录文件（`.imx`）

由于在 Docker 容器中直接挂载 USB 设备较为繁琐，推荐采用以下流程：

```mermaid
flowchart LR
	step1["容器内生成 .imx"]
	step2["Windows 端烧录"]
	step1 --> step2
```

`imxdownload` 工具的作用是为编译好的 `.bin` 文件添加头部信息（IVT、DCD 等），使其能被 `I.MX6ULL` 的 BootROM 识别并引导。

推荐在 `CMakeLists.txt` 中使用如下命令生成 `.imx`：

```cmake
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND imxdownload ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${PROJECT_NAME}.imx
)
```

### 4. 烧录到 SD 卡（Windows）

> [!TIP]
>
> 这里所说的 SD 卡准确来说应该叫做 MicroSD 卡，真正的 SD 卡是相机里面用的那种大卡，但是似乎我见到的都是这么叫的🤣，所以我就这么写了。

回到 Windows 主机，将 SD 卡插入读卡器，使用以下任意工具将生成的 `.imx` 文件写入 SD 卡即可：

- USB Image Tool（推荐）
- Win32DiskImager（不推荐，因为它会跟虚拟磁盘冲突，比如 Google Drive）

烧录完成后，将 SD 卡插入开发板，设置好启动跳线（Boot Mode），即可上电运行。

