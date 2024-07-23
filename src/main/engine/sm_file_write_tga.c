// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

bool sm_fileWriteTga(FILE *output, uint8_t *pixels, uint16_t width, uint16_t height) {
  // TGA header
  uint8_t header[18] = {0};
  header[2]          = 2;                       // uncompressed RGB format
  header[12]         = (uint8_t)(width % 256);  // width low byte
  header[13]         = (uint8_t)(width / 256);  // width high byte
  header[14]         = (uint8_t)(height % 256); // height low byte
  header[15]         = (uint8_t)(height / 256); // height high byte
  header[16]         = 24;                      // 24 bit bitmap, TrueVision uncompressed RGB

  // Write the TGA file header
  if (fwrite(header, sizeof(header), 1, output) != 1) {
    fprintf(stderr, "Error writing header to file\n");
    return false;
  }

  // Write the image data
  if (fwrite(pixels, 1, 3 * width * height, output) != 3 * width * height) {
    fprintf(stderr, "Error writing image data to file\n");
    return false;
  }

  return true;
}
