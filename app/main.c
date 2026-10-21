#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl/lvgl.h"

int main(void) {
    lv_init();

    int fbfd = open("/dev/fb0", O_RDWR);
    if(fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return 1;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;
    char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if((long)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        close(fbfd);
        return 1;
    }

    lv_display_t * disp = lv_display_create(vinfo.xres, vinfo.yres);
    lv_display_set_buffers(disp, fbp, NULL, screensize, LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_obj_t * scr = lv_screen_active();
    
    lv_obj_t * btn = lv_button_create(scr);
    lv_obj_set_size(btn, 200, 80);
    lv_obj_center(btn);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Gui finally works les go");
    lv_obj_center(label);

    printf("LVGL GUI running on framebuffer!\n");
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
