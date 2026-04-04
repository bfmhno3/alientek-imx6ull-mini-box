echo "Hello World"
cd rootfs
make defconfig
#make menuconfig
make
make install CONFIG_PREFIX=./install