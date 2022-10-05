#!/bin/bash

tar -xzvf wsjtx-patched-2.5.4.tgz
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../wsjtx-2.5.4
cmake --build . --target package -- -j2

cp /build/build/wsjtx-prefix/src/wsjtx-build/wsjtx_2.5.4_amd64.deb /build/wsjtx_2.5.4_jammy_amd64.deb
