# Maintainer: Andrea Diamantini <adjam@protonmail.com>

pkgname=cutepad-git
_pkgname=cutepad
pkgver=0.0.3
pkgrel=1
pkgdesc='Lightweight Qt plain text editor'
arch=(x86_64)
url='https://github.com/adjamhub/cutepad'
license=(GPL3)
depends=(qt5-base syntax-highlighting hicolor-icon-theme)
makedepends=(cmake git)
source=("git+https://github.com/adjamhub/cutepad.git")
md5sums=('SKIP')


pkgver() {
  cd "$_pkgname"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
  mkdir -p build
}

build() {
  cd build
  cmake ../cutepad \
     -DCMAKE_INSTALL_PREFIX=/usr
  make
}

package() {
  cd build
  make DESTDIR="$pkgdir" install
}
