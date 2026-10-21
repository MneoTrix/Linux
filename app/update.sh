#!/usr/bin/fish

echo "[1/4] Compiling LVGL application..."
cd ~/LinuxProject/app
gcc main.c (find lvgl/src -name "*.c") -I. -I lvgl -D LV_CONF_INCLUDE_SIMPLE -static -lpthread -lm -o my_lvgl_app
if test $status -ne 0
    echo "Compilation failed!"
    exit 1
end

echo "[2/4] Copying binary to rootfs..."
cp my_lvgl_app ~/LinuxProject/busybox/_install/usr/bin/

echo "[3/4] Repacking initramfs..."
cd ~/LinuxProject/linux-7.2.11/busybox/_install
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/LinuxProject/linux-7.2.11/rootfs.cpio.gz

echo "[4/4] Launching QEMU..."
qemu-system-x86_64 \
    -kernel ~/LinuxProject/linux-7.2.11/arch/x86/boot/bzImage \
    -initrd ~/LinuxProject/linux-7.2.11/rootfs.cpio.gz \
    -append "console=ttyS0 vga=791" \
    -cpu max \
    -vga std
