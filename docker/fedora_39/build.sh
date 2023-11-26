#!/bin/bash

tar -xzvf wsjtx-patched-2.6.1.tgz
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../wsjtx-2.6.1
cmake --build . --target package -- -j4

cp /build/build/wsjtx-prefix/src/wsjtx-build/wsjtx-2.6.1.x86_64.rpm /build/wsjtx-2.6.1.fc39.x86_64.rpm
