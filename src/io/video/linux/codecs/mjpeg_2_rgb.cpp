//
// Created by vahagn on 15/04/20.
//

#include "mjpeg_2_rgb.h"
#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>
#include <cstdlib>

namespace gago {
namespace io {
namespace video {

void MJpeg2Rgb(uint8_t *input_data, uint8_t *output, int jpg_size) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  int row_stride, width, height, pixel_size;

  cinfo.err = jpeg_std_error( &jerr );

  jpeg_create_decompress( &cinfo );
  jpeg_mem_src(&cinfo, input_data, jpg_size);

 // jpeg_stdio_src( &cinfo, 1 );

  jpeg_read_header( &cinfo, TRUE );
  jpeg_start_decompress(&cinfo);

  width = cinfo.output_width;
  height = cinfo.output_height;
  pixel_size = cinfo.output_components;
  row_stride = width * pixel_size;



  while (cinfo.output_scanline < cinfo.output_height) {
    unsigned char *buffer_array[1];
    buffer_array[0] = output + \
						   (cinfo.output_scanline) * row_stride;

    jpeg_read_scanlines(&cinfo, buffer_array, 1);

  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);


}

}
}
}