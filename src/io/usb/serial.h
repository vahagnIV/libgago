//
// Created by vahagn on 8/1/20.
//

#ifndef LIBGAGO_SRC_IO_USB_SERIAL_H_
#define LIBGAGO_SRC_IO_USB_SERIAL_H_
#include <string>
#include <vector>

namespace gago {
namespace io {
namespace usb {

class Serial {
 public:
  Serial(const std::string &path);
  int Open();
  void Close();
  void Write(const std::vector<uint8_t > & data);
  void Write(int  data);
  void Write(uint8_t  data);
  void Write(char  data);

  int Read(int & r);
  int ReadChar(char &c);

  virtual ~Serial();
 private:
  int fd_;
  std::string path_;

};

}
}
}
#endif //LIBGAGO_SRC_IO_USB_SERIAL_H_
