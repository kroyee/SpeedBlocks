set -e

export SFML_VERSION=2.5.1
export SFML_ROOT=$HOME/SFML_OSX

if [[ ! -f "$SFML_ROOT/travis-cache-$SFML_VERSION" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/${SFML_VERSION}.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-${SFML_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT -DSFML_BUILD_FRAMEWORKS=TRUE .
  make -j2
  sudo make install
  cd ..
  rm $SFML_ROOT/travis-cache-*
  touch $SFML_ROOT/travis-cache-$SFML_VERSION
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

export TGUI_VERSION=0.8.2
export TGUI_ROOT=$HOME/TGUI_OSX

if [[ ! -f "$TGUI_ROOT/travis-cache-$TGUI_VERSION" ]]; then
  echo "$(tput setaf 3)Rebuilding TGUI: no cache available$(tput sgr 0)"

  wget -O TGUI.tar.gz https://github.com/texus/TGUI/archive/v${TGUI_VERSION}.tar.gz
  tar -xzf TGUI.tar.gz
  cd TGUI-${TGUI_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$TGUI_ROOT -DTGUI_BUILD_FRAMEWORKS=TRUE -DSFML_INCLUDE_DIR=$SFML_ROOT/SFML.framework .
  make -j2
  sudo make install
  cd ..
  rm $TGUI_ROOT/travis-cache-*
  touch $TGUI_ROOT/travis-cache-$TGUI_VERSION
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

cmake -DSFML_INCLUDE_DIR=$SFML_ROOT/SFML.framework .
make -j2
make install
cd ..