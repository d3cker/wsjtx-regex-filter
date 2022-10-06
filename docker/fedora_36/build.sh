#!/bin/bash

tar -xzvf wsjtx-patched-2.5.4.tgz
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../wsjtx-2.5.4
cmake --build . --target package -- -j2

cp /build/build/wsjtx-prefix/src/wsjtx-build/wsjtx-2.5.4.x86_64.rpm /build/wsjtx-2.5.4.fc36.x86_64.rpm
