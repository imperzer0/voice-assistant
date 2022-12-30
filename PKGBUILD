#!/bin/bash
# Maintained by imper <imperator999mcpe@gmail.com>

pkgname=assistant
pkgver=1.0
pkgrel=1
pkgdesc='Voice-controlled assistant in C++ using rev.ai API'
author="imperzer0"
arch=('any')
url=https://github.com/$author/$pkgname
license=('GPL3')
depends=('openssl' 'gcc' 'cpprestsdk' 'sfml')
makedepends=('cmake' 'git' 'gcc' 'make' 'openssl' 'cpprestsdk' 'sfml')

_srcprefix="local:/"
_libfiles=(
  "CMakeLists.txt" "main.cpp"
  "assistant.cpp" "assistant.h" "constants.hpp"
  "config.cpp" "config.h"
  "cmdline.cpp" "cmdline.h"
)

# shellcheck disable=SC2068
for _libfile in ${_libfiles[@]}; do
  source=(${source[@]} "$_srcprefix/$_libfile")
  sha512sums=(${sha512sums[@]} "SKIP")
done

_package_version=" ("$pkgver"-"$pkgrel")"

prepare() {
  makepkg -sif --noconfirm -p PKGBUILD.fineftp
}

build() {
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ \
    -DPACKAGE_VERSION="$_package_version" -DAPPNAME="$pkgname" .
  make -j 8
}

package() {
  install -Dm755 $pkgname "$pkgdir/usr/bin/$pkgname"
}
