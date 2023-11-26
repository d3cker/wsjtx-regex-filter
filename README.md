# WSJT-X 2.6.1 - Regex ignore filter

## Build and binaries

Navigate to [GitHub Actions](https://github.com/d3cker/wsjtx-regex-filter/actions) for build process details.

[Available releases](https://github.com/d3cker/wsjtx-regex-filter/releases):
- Ubuntu Jammy 22.04
- Debian Bookworm 12 x64
- Debian Bookwork ARM64 (Raspbian)
- Fedora 39
- Microsoft Windows 10/11 (installer)
- Patched source code .tgz archive

You may need to remove `wsjtx`, `wsjtx-doc` and `wsjtx-data` before installing
a release package.

If you want to compile this patch by your own try this instructions:

- Install dependencies (Ubuntu/Debian/Raspbian)
```
sudo apt-get install -y gcc g++ clang gfortran cmake git asciidoc texinfo\
 qtmultimedia5-dev libqt5serialport5-dev qttools5-dev qttools5-dev-tools\
 libboost-all-dev libfftw3-dev libreadline-dev libusb-1.0-0-dev libudev-dev\
 portaudio19-dev build-essential binutils lintian debhelper dh-make devscripts
```

- Install dependencies (Fedora)
```
sudo dnf install -y qt5-qtmultimedia-devel qt5-qtserialport-devel qt5-linguist\
    boost-devel fftw-devel readline-devel systemd-devel libusb1-devel\
    portaudio-devel patch gcc g++ gfortran cmake git asciidoc texinfo\
    rpmdevtools rpmlint
```

- Download `wsjtx-patched-2.6.1.tgz` from [Releases](https://github.com/d3cker/wsjtx-regex-filter/releases)

- Build package
```
tar -xzvf wsjtx-patched-2.6.1.tgz
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../wsjtx-2.6.1
cmake --build . --target package
```

You may add `-- -j<number_of_threads>` to speed up build process.

## Features
- Added option: Setup -> RX/TX Macros -> RX regex ignore filter
![Options](images/options.png)
- CQ answers from matched callsigns are ignored
- reply to CQ from matched callsigns are ignored
- Manual clicks (calls) on matched stations are ignored

![Main window](images/main.png)

Just like that. Tested during WW DIGI'22 contest.

## Note on purpose

I have to confess that I'm not a software developer and this patch is just 
a dirty hack to make it possible to make auto QSO with all except Russians.
To filter out RU stations use this filter: **^(R[A-Z1-9]|U[A-I])+** 

I know that this is not part of "ham spirit" to make such a software but...
let's face it. One does not simply attack its neighbour and expects others
to stay quiet. As a Pole I know my country has long and cloudy history with 
both Russians and Ukrainians,but what happened in the past should stay in the past.
One may remember, one may forget, one may cry for revenge or one my simply forgive.
From my perspective, we live in 2022, Europe, in the times of wide spread Internet.
Suddenly war explodes at the borders of my country. The war that makes no sense,
pure aggression and disgusting act of terrorism. As long as regular Russians do not 
resist to their goverment I would act and make software like this, no matter what.

Bartek SP6XD
