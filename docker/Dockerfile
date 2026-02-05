# 阶段 1：准备工具链
FROM debian:bullseye-slim AS builder

RUN apt-get update && apt-get install -y xz-utils && rm -rf /var/lib/apt/lists/*

COPY archives/ /tmp/archives

# 解压 GCC 工具链
RUN tar -xvf /tmp/archives/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz -C /opt/ \
    && mv /opt/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf /opt/gcc-arm
# 准备 imxdownload 工具
RUN mkdir -p /opt/imx-tools \
    && cp /tmp/archives/imxdownload /opt/imx-tools/ \
    && chmod +x /opt/imx-tools/imxdownload

# 阶段 2：最终镜像
FROM ubuntu:16.04
LABEL authors="bfmhno3" website="https://github.com/bfmhnmo3/alientek-imx6ull-mini-box"

# 设置基础环境变量
ENV DEBIAN_FRONTEND=noninteractive LANG=C.UTF-8 LC_ALL=C.UTF-8

# 安装编译环境必要的依赖库
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    vim \
    bc \
    sudo \
    lzop \
    libncurses5-dev \
    libssl-dev \
    flex \
    bison \
    file \
    && rm -rf /var/lib/apt/lists/*

# 拷贝工具链
COPY --from=builder /opt/gcc-arm /opt/gcc-arm
COPY --from=builder /opt/imx-tools /opt/imx-tools

# 设置交叉编译环境变量
ENV PATH="/opt/gcc-arm/bin:/opt/imx-tools:${PATH}" \
    ARCH=arm \
    CROSS_COMPILE=arm-linux-gnueabihf- \
    CC=arm-linux-gnueabihf-gcc \
    CXX=arm-linux-gnueabihf-g++ \
    CPP=arm-linux-gnueabihf-cpp \
    FC=arm-linux-gnueabihf-gfortran \
    LD=arm-linux-gnueabihf-ld \
    AR=arm-linux-gnueabihf-ar \
    RANLIB=arm-linux-gnueabihf-ranlib \
    GDB=arm-linux-gnueabihf-gdb \
    NM=arm-linux-gnueabihf-nm \
    STRIP=arm-linux-gnueabihf-strip \
    OBJCOPY=arm-linux-gnueabihf-objcopy \
    OBJDUMP=arm-linux-gnueabihf-objdump \
    SIZE=arm-linux-gnueabihf-size \
    STRINGS=arm-linux-gnueabihf-strings \
    ADDR2LINE=arm-linux-gnueabihf-addr2line \
    READELF=arm-linux-gnueabihf-readelf \
    GCOV=arm-linux-gnueabihf-gcov \
    ELFEDIT=arm-linux-gnueabihf-elfedit

# 创建工作目录
WORKDIR /project

# 设置默认命令
CMD ["/bin/bash"]