#!/bin/bash

tar -xzvf wsjtx-patched-2.6.1.tgz
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../wsjtx-2.6.1
cmake --build . --target package -- -j4

cp /build/build/wsjtx-prefix/src/wsjtx-build/wsjtx_2.6.1_amd64.deb /build/wsjtx_2.6.1_bookworm_amd64.deb
