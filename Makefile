default: sfml

sfml: main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o
	g++ -std=c++11 main.o gameField.o optionSet.o pieces.o gamePlay.o randomizer.o textures.o sounds.o gui.o network.o packetcompress.o gameFieldDrawer.o EmptyResourcePath.o -o sfml -ltgui -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

main.o: main.cpp pieces.h gameField.h gamePlay.h textures.h optionSet.h randomizer.h sounds.h gui.h network.h packetcompress.h
	g++ -std=c++11 -c main.cpp -o main.o -Wall -Wextra

gameField.o: gameField.cpp gameField.h
	g++ -std=c++11 -c gameField.cpp -o gameField.o -Wall -Wextra

optionSet.o: optionSet.cpp optionSet.h pieces.h
	g++ -std=c++11 -c optionSet.cpp -o optionSet.o -Wall -Wextra

pieces.o: pieces.cpp pieces.h
	g++ -std=c++11 -c pieces.cpp -o pieces.o -Wall -Wextra

gamePlay.o: gamePlay.cpp pieces.h gameField.h gamePlay.h optionSet.h randomizer.h sounds.h
	g++ -std=c++11 -c gamePlay.cpp -o gamePlay.o -Wall -Wextra

randomizer.o: randomizer.cpp randomizer.h
	g++ -std=c++11 -c randomizer.cpp -o randomizer.o -Wall -Wextra

textures.o: textures.cpp textures.h
	g++ -std=c++11 -c textures.cpp -o textures.o -Wall -Wextra

sounds.o: sounds.cpp sounds.h
	g++ -std=c++11 -c sounds.cpp -o sounds.o -Wall -Wextra

gui.o: gui.cpp gui.h sounds.h gamePlay.h network.h
	g++ -std=c++11 -c gui.cpp -o gui.o -Wall -Wextra

gameFieldDrawer.o: gameFieldDrawer.cpp gui.h optionSet.h gamePlay.h network.h
	g++ -std=c++11 -c gameFieldDrawer.cpp -o gameFieldDrawer.o -Wall -Wextra

network.o: network.cpp network.h
	g++ -std=c++11 -c network.cpp -o network.o -Wall -Wextra

packetcompress.o: packetcompress.cpp packetcompress.h gamePlay.h
	g++ -std=c++11 -c packetcompress.cpp -o packetcompress.o -Wall -Wextra

EmptyResourcePath.o: EmptyResourcePath.cpp EmptyResourcePath.h
	g++ -std=c++11 -c EmptyResourcePath.cpp -o EmptyResourcePath.o -Wall -Wextra

run: sfml
	./sfml