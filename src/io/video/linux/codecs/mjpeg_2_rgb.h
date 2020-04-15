//
// Created by vahagn on 15/04/20.
//

#ifndef LIBGAGO_MJPEG_2_RGB_H
#define LIBGAGO_MJPEG_2_RGB_H
#include <cstdint>
namespace gago {
namespace io {
namespace video {

void MJpeg2Rgb(uint8_t *input_data, uint8_t *output, int jpg_size);

}
}
}
#endif //LIBGAGO_MJPEG_2_RGB_H
