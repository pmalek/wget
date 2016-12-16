## HTTP file downloader build with [boost::asio](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio.html) and [beast](https://github.com/vinniefalco/Beast)

[![Build Status](https://travis-ci.org/pmalek/wget.svg?branch=master)](https://travis-ci.org/pmalek/wget)

### How to build

```
git clone https://github.com/pmalek/wget.git && \
cd wget && git submodule update --init --recursive --depth 1 && \
mkdir ../build && cd ../build && cmake ../wget && cmake --build .
```
