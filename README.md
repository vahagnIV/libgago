# LibGago

This is the library part of a larger project Gago which is split into two projects 

 1. LibGago - the library api 
 2. [Gago-UI](https://github.com/vahagnIV/gago-ui), a UI interface.
 
## Installation

Currently, LibGago works only under linux. The only way to install it is to build it from source. 

Dependencies:

```bash
sudo apt-get install -y libboost-filesystem-dev libopencv-dev libv4l-dev
```

Clone the project

```bash
git clone git@github.com:vahagnIV/libgago.git
cd libgago
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 5
sudo make install
```

