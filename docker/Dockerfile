# 设置基础镜像
FROM ubuntu:16.04
LABEL authors="bfmhno3" website="https://github.com/bfmhnmo3/alientek-imx6ull-mini-box"

# 设置非交互式安装，防止安装过程卡住
ENV DEBIAN_FRONTEND=noninteractive

# 更新软件包并安装必要工具
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    wget \
    curl \
    build-essential \
    cmake \
    git \
    vim \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录（移除多余的反斜杠）
WORKDIR /opt

# 复制并解压交叉编译工具链（简化 COPY 语法）
COPY archives/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz /tmp/
RUN cd /tmp && \
    tar -xf gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz && \
    mv gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf /opt/gcc-arm && \
    rm -f /tmp/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz

# 设置交叉编译工具链环境变量
ENV PATH="/opt/gcc-arm/bin:${PATH}"
ENV CC=arm-linux-gnueabihf-gcc
ENV CXX=arm-linux-gnueabihf-g++
ENV AR=arm-linux-gnueabihf-ar
ENV STRIP=arm-linux-gnueabihf-strip

# 创建工作目录
WORKDIR /workspace

# 设置默认命令
CMD ["/bin/bash"]
