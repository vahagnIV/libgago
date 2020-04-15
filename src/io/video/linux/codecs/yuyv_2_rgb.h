//
// Created by vahagn on 15/04/20.
//

#ifndef LIBGAGO_YUYV_2_RGB_H
#define LIBGAGO_YUYV_2_RGB_H

#include <cstdint>
namespace gago {
namespace io {
namespace video {

void Yuyv2Rgb(const uint8_t *src,
              uint8_t *dest,
              int width, int height,
              int stride);
}

}
}
#endif //LIBGAGO_YUYV_2_RGB_H
