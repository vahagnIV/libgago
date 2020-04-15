//
// Created by vahagn on 15/04/20.
//

#include "yuyv_2_rgb.h"

namespace gago {
namespace io {
namespace video {

#define CLIP(color) (uint8_t)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))
void Yuyv2Rgb(const uint8_t *src, uint8_t *dest, int width, int height, int stride) {
  int j;

  while (--height >= 0) {
    for (j = 0; j + 1 < width; j += 2) {
      int u = src[1];
      int v = src[3];

      int u1 = (((u - 128) << 7) + (u - 128)) >> 6;
      int rg = (((u - 128) << 1) + (u - 128) +
          ((v - 128) << 2) + ((v - 128) << 1)) >> 3;

      int v1 = (((v - 128) << 1) + (v - 128)) >> 1;

      *dest++ = CLIP(src[0] + v1);
      *dest++ = CLIP(src[0] - rg);
      *dest++ = CLIP(src[0] + u1);

      *dest++ = CLIP(src[2] + v1);
      *dest++ = CLIP(src[2] - rg);
      *dest++ = CLIP(src[2] + u1);
      src += 4;
    }
    src += stride - (width * 2);
  }
}

}
}
}