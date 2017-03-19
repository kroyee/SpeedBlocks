CC?=g++
GCC_VERSION := $(shell $(CC) -dumpversion)
GCC_VERSION_MAJOR := $(shell echo $(GCC_VERSION) | cut -d'.' -f1)
GCC_VERSION_MINOR := $(shell echo $(GCC_VERSION) | cut -d'.' -f2)

ifeq ($(shell test $(GCC_VERSION_MAJOR) -lt 5 -a $(GCC_VERSION_MINOR) -lt 9; echo $$?), 0)
  $(error GCC version >= 4.9 is requited. Detected version: $(GCC_VERSION))
endif

default: sfml

sfml: main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o -o sfml -ltgui -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

main.o: main.cpp pieces.h gameField.h gamePlay.h textures.h optionSet.h randomizer.h sounds.h gui.h network.h packetcompress.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c main.cpp -o main.o

gameField.o: gameField.cpp gameField.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c gameField.cpp -o gameField.o

optionSet.o: optionSet.cpp optionSet.h pieces.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c optionSet.cpp -o optionSet.o

pieces.o: pieces.cpp pieces.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c pieces.cpp -o pieces.o

gamePlay.o: gamePlay.cpp pieces.h gameField.h gamePlay.h optionSet.h randomizer.h sounds.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c gamePlay.cpp -o gamePlay.o

randomizer.o: randomizer.cpp randomizer.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c randomizer.cpp -o randomizer.o

textures.o: textures.cpp textures.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c textures.cpp -o textures.o

sounds.o: sounds.cpp sounds.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c sounds.cpp -o sounds.o

gui.o: gui.cpp gui.h sounds.h gamePlay.h network.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c gui.cpp -o gui.o

gameFieldDrawer.o: gameFieldDrawer.cpp gui.h optionSet.h gamePlay.h network.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c gameFieldDrawer.cpp -o gameFieldDrawer.o

network.o: network.cpp network.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c network.cpp -o network.o

packetcompress.o: packetcompress.cpp packetcompress.h gamePlay.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c packetcompress.cpp -o packetcompress.o

EmptyResourcePath.o: EmptyResourcePath.cpp EmptyResourcePath.h
	$(CC) -LTGUI-0.7.3/lib/ -LSFML-2.4.2/lib/ -ITGUI-0.7.3/include/ -Iboost_1_63_0/ -ISFML-2.4.2/include/ -std=c++11 -c EmptyResourcePath.cpp -o EmptyResourcePath.o

run: sfml
	./sfml

.PHONY: clean

clean:
	-rm -f ./*.o
	-rm ./sfml
