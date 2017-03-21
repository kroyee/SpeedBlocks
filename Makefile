CXX?=g++
GCC_VERSION := $(shell $(CXX) $(COMPILER_FLAGS) -dumpversion)
GCC_VERSION_MAJOR := $(shell echo $(GCC_VERSION) | cut -d'.' -f1)
GCC_VERSION_MINOR := $(shell echo $(GCC_VERSION) | cut -d'.' -f2)

COMPILER_FLAGS=-Wall -Wextra

#VENDOR_PATH =
TGUI_LIB_PATH = -L$(VENDOR_PATH)TGUI-0.7.3/lib/
TGUI_INC_PATH = -I$(VENDOR_PATH)TGUI-0.7.3/include/

#SFML_LIB_PATH = -L$(VENDOR_PATH)SFML-2.4.2/lib/
#SFML_LIB_PATH = -L/usr/lib64
#SFML_INC_PATH = -L$(VENDOR_PATH)SFML-2.4.2/include/

#BOOST_INC_PATH = -I$(VENDOR_PATH)boost_1_63_0/

ifeq ($(shell test $(GCC_VERSION_MAJOR) -lt 5 -a $(GCC_VERSION_MINOR) -lt 9; echo $$?), 0)
  $(error GCC version >= 4.9 is requited. Detected version: $(GCC_VERSION))
endif


default: sfml

sfml: main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o -o sfml -ltgui -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

main.o: main.cpp pieces.h gameField.h gamePlay.h textures.h optionSet.h randomizer.h sounds.h gui.h network.h packetcompress.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c main.cpp -o main.o

gameField.o: gameField.cpp gameField.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c gameField.cpp -o gameField.o

optionSet.o: optionSet.cpp optionSet.h pieces.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c optionSet.cpp -o optionSet.o

pieces.o: pieces.cpp pieces.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c pieces.cpp -o pieces.o

gamePlay.o: gamePlay.cpp pieces.h gameField.h gamePlay.h optionSet.h randomizer.h sounds.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c gamePlay.cpp -o gamePlay.o

randomizer.o: randomizer.cpp randomizer.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c randomizer.cpp -o randomizer.o

textures.o: textures.cpp textures.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c textures.cpp -o textures.o

sounds.o: sounds.cpp sounds.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c sounds.cpp -o sounds.o

gui.o: gui.cpp gui.h sounds.h gamePlay.h network.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c gui.cpp -o gui.o

gameFieldDrawer.o: gameFieldDrawer.cpp gui.h optionSet.h gamePlay.h network.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c gameFieldDrawer.cpp -o gameFieldDrawer.o

network.o: network.cpp network.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c network.cpp -o network.o

packetcompress.o: packetcompress.cpp packetcompress.h gamePlay.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c packetcompress.cpp -o packetcompress.o

EmptyResourcePath.o: EmptyResourcePath.cpp EmptyResourcePath.h
	$(CXX) $(COMPILER_FLAGS) $(TGUI_LIB_PATH) $(SFML_LIB_PATH) $(TGUI_INC_PATH) $(BOOST_INC_PATH) $(SFML_INC_PATH) -std=c++11 -c EmptyResourcePath.cpp -o EmptyResourcePath.o

run: sfml
	./sfml

.PHONY: clean

clean:
	-rm -f ./*.o
	-rm ./sfml
