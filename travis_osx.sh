set -e

export SFML_VERSION=2.5.1
export SFML_ROOT=$HOME/SFML_OSX

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/${SFML_VERSION}.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-${SFML_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT -DCMAKE_INSTALL_FRAMEWORK_PREFIX=$SFML_ROOT/lib -DSFML_BUILD_FRAMEWORKS=TRUE .
  make -j2
  make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

export TGUI_VERSION=v0.8.2
export TGUI_ROOT=$HOME/TGUI_OSX

if [[ ! -d "$TGUI_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding TGUI: no cache available$(tput sgr 0)"

  wget -O TGUI.tar.gz https://github.com/texus/TGUI/archive/${TGUI_VERSION}.tar.gz
  tar -xzf TGUI.tar.gz
  cd TGUI-${TGUI_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$TGUI_ROOT -DCMAKE_INSTALL_FRAMEWORK_PREFIX=$TGUI_ROOT/lib -DTGUI_BUILD_FRAMEWORKS=TRUE -DSFML_INCLUDE_DIR=$SFML_ROOT/lib/SFML.framework -DTGUI_OPTIMIZE_SINGLE_BUILD=TRUE .
  make -j2
  sudo make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

cmake -DSFML_INCLUDE_DIR=$SFML_ROOT/lib/SFML.framework .
make -j2
make install
cd ..