# Alientek I.MX6ULL Mini Box

[![Status](https://img.shields.io/badge/status-in_progress-green)](https://github.com/your-username/imx6ull-mini-box) [![Platform](https://img.shields.io/badge/platform-I.MX6ULL-orange)](https://www.nxp.com/products/processors-and-microcontrollers/arm-processors/i-mx-applications-processors/i-mx-6-processors/i-mx-6ull-single-core-processor-with-arm-cortex-a7-core:i.MX6ULL)

这是一个为**正点原子 I.MX6ULL Mini 开发板**创建的嵌入式 Linux 学习工作区。

本项目记录了从**裸机开发**到**完整的 Linux 系统移植**，再到**上层应用（Qt, C/C++）开发**的全过程。所有开发工作都在一个统一的、可复现的 **Docker** 环境中进行。

## ✨ 项目特点

- 🛠 **一体化环境**：使用 Docker 统一管理交叉编译工具链和所有依赖，实现一键式环境搭建。
- 📚 **阶段化学习**：目录结构清晰，按照官方教程循序渐进学习。
- ✍️ **代码与笔记**：不仅有可运行的示例代码，还有详细的文档和笔记记录学习过程中的要点和思考。
- 💻 **现代化工具链**：结合 **Git**，**GitHub** 和 **CLion** 进行高效的代码管理与开发。

## 目录结构

```bash
.
├── 01_bare_metal/        # 裸机开发
├── 02_bootloader/        # U-Boot 移植
├── 03_linux_kernel/      # Linux 内核移植与编译
├── 04_rootfs/            # 根文件系统构建 (Buildroot/BusyBox)
├── 05_linux_driver/      # Linux 驱动开发
├── 07_qt_app/            # Qt 应用程序
├── docker/               # Docker 开发环境
├── docs/                 # 学习笔记与文档
├── tools/                # 实用工具脚本
└── README.md
```

## 🚀 快速开始

### 1. 克隆仓库

```bash
git clone https://github.com/bfmhno3/alientek-imx6ull-mini-box.git
git lfs install
cd alientek-imx6ull-mini-box
```

### 2. 构建 Docker 环境

```bash
docker build -t alientek-imx6ull:latest ./docker 
```

### 3. 启动开发容器

```bash
docker run -it --name imx6ull-dev alientek-imx6ull:latest
```

### 4. 开始编码

现在你已经在一个包含了所有工具的 Ubuntu 环境中了，可以开始你的嵌入式之旅了！

**Happy Hacking!**
