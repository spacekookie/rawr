
#include <linux/fb.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

/** Declare some utility functions */
long calculate_point(long, long, struct fb_var_screeninfo*, struct fb_fix_screeninfo*);
uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo);

int main(void)
{
  /* Hold information about screen data */
  struct fb_fix_screeninfo finfo;
  struct fb_var_screeninfo vinfo;

  /* Open a fb device */
  int fb_fd = open("/dev/fb0", O_RDWR);

  /* Get variable screen information */
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

  /* We change some values */
  vinfo.grayscale = 0;
  vinfo.bits_per_pixel = 32;
  ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);

  /* Then verify that everything was updated */
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

  /* Get fixed screen information */
  ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

  /* Calculate the screen size */
  long screensize = vinfo.yres_virtual * finfo.line_length;

  /* Map it */
  uint8_t *fb_ptr = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t) 0);

  /* Draw it all! */
  int x, y;
  for (y = 0; y < vinfo.yres; y++) {
    for (x = 0; x < vinfo.xres; x++) {

      long location = calculate_point(x, y, &vinfo, &finfo);
      *((uint32_t*)(fb_ptr + location)) = pixel_color(0xFF, 0xFF, 0xFF, &vinfo);
    }
  
    usleep(5000);
  }
  
  /*  */
  printf("Rawr!\n");
  return 0;
}

long calculate_point(long x, long y, struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo)
{
  return 
  /* Get the x-offset on a line */
  (x + vinfo->xoffset) * (vinfo->bits_per_pixel / 8) 

  /* Gets the beginning of line "y" */
  + (y + vinfo->yoffset) * finfo->line_length;
}

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
  return (r << vinfo->red.offset) | (g << vinfo->green.offset) | (b << vinfo->blue.offset);
}