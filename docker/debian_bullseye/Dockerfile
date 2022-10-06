FROM debian:11

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive && apt-get install -y gcc\
    g++ clang gfortran cmake git asciidoc texinfo qtmultimedia5-dev\
    libqt5serialport5-dev qttools5-dev qttools5-dev-tools libboost-all-dev\
    libfftw3-dev libreadline-dev libusb-1.0-0-dev libudev-dev portaudio19-dev\
    build-essential binutils lintian debhelper dh-make devscripts

