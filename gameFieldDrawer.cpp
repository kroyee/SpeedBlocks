#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "network.h"
#include <iostream>
using namespace std;

void UI::addField(obsField& field) {
	fields.push_back(field);
	calFieldPos();
	drawOppField(fields.back());
}

void UI::removeField(sf::Uint16 id) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == id) {
			it = fields.erase(it);
			break;
		}
	calFieldPos();
}

void UI::removeAllFields() {
	while (fields.size())
		fields.pop_front();
}

void UI::updateField(obsField& field) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == field.id) {
			for (int y=0; y<22; y++)
				for (int x=0; x<10; x++)
					it->square[y][x] = field.square[y][x];
			it->nextpiece = field.nextpiece;
			it->nprot = field.nprot;
			it->npcol = field.npcol;
			drawOppField(*it);
		}
}

void UI::calFieldPos() {
	float r = 600/440.0;
	short width = 490, height = 555, startx = 465, starty = 40;
	short total = fields.size(), placed = 0;

	short field_w = width;
	short field_h = r*field_w;
	if (field_h>height) {
		field_h=height;
		field_w=field_h/r;
	}

	bool done = false;
	short x, y, rows;

	while (!done) {
		x=0; y=0; rows=1;
		for (placed = 0; placed < total; placed++) {
			if (x+field_w > width) { x=0; y+=field_h; rows++; }
			if (y+field_h > height)
				break;
			x+=field_w;
		}
		if (placed >= total)
			done=true;
		else {
			field_w--;
			field_h=field_w*r;
		}
	}

	short perrow = (float)total/rows + 0.999999;

	r = field_w/440.0;
	short onrow=0, currow=0;

	short(*origin)[2] = new short[total][2];

	short c;
	for (c=0; c<total; c++) {
		origin[c][0] = startx + (onrow+1)*((width-field_w*perrow) / (perrow+1)) + onrow * field_w;
		origin[c][1] = starty + (currow+1)*((height-field_h*rows) / (rows+1)) + field_h*currow;
		onrow++;
		if (onrow==perrow) {
			onrow=0;
			currow++;
		}
	}

	c=0;
	for (auto &&it : fields) { //start 480, 0
		short originX = (480-origin[c][0]) / (r-1);
		short originY = (origin[c][1] / (r-1))*-1;

		it.sprite.setOrigin(originX, originY);
		it.sprite.setPosition(480+originX, originY);
		it.sprite.setScale(r, r);
		c++;
	}

	delete[] origin;
	origin = nullptr;

	currentR = r;
}

void UI::resetOppFields() {
	for (auto&& field : fields) {
		field.position=0;
		field.datacount=250;
		field.piece.posX=0;
		field.piece.posY=0;
		field.clear();
		drawOppField(field);
	}
}

void UI::drawOppField(obsField& field) {
	if (net.packetid == 101)
		field.preDrawField();
	else
		field.drawField();

	field.drawNextPiece();
}

void UI::drawFields() {
	if (!gui.get("GameFields")->isVisible())
		return;
	for (auto&& field : fields)
		window->draw(field.sprite);
	if (scaleup) {
		scaleup->scale += sclock.restart().asMilliseconds() / 5;
		if (scaleup->scale > 100)
			scaleup->scale = 100;
		float maxRat = 0.85, add;
		add = (maxRat-currentR)/100;
		if (add>0) {
			scaleup->sprite.setScale(currentR+add*scaleup->scale, currentR+add*scaleup->scale);
			window->draw(scaleup->sprite);
			window->draw(scaleup->sprite);
		}
	}
}

const sf::String& UI::getName(sf::Uint16 id) {
	if (id == myId)
		return game.field.name;
	for (auto&& field : fields)
		if (field.id == id)
			return field.name;
	return unknown;
}

void UI::goAway() {
	away=true;
	sendPacket8();
	game.gameover=true;
	game.sendgameover=true;
	game.awayText.setString("Away");
	game.drawGameOver();
}

void UI::unAway() {
	away=false;
	game.autoaway=false;
	sendPacket9();
	game.awayText.setString("");
	game.drawGameOver();
}

void UI::handleEvent(sf::Event& event) {
	gameInput(event);
	windowEvents(event);
	
	if (setkey)
		putKey(event);
	enlargePlayfield(event);
	keyEvents(event);
	roomList.scrolled(event);
	tournamentList.scrolled(event);

	gui.handleEvent(event);
}

void UI::gameInput(sf::Event& event) {
	if (gamestate != Replay && gamestate != MainMenu)
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == options.chat)
                Chat();
            else if (event.key.code == options.score)
                Score();
            else if (event.key.code == options.away && playonline) {
                if (away)
                    unAway();
                else
                    goAway();
            }
		}
	if (gamestate == CountDown) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == options.right)
                game.rKey=true;
            else if (event.key.code == options.left)
                game.lKey=true;
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == options.right)
                game.rKey=false;
            else if (event.key.code == options.left)
                game.lKey=false;
        }
	}
	else if (gamestate == Game) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == options.right)
                game.mRKey();
            else if (event.key.code == options.left)
                game.mLKey();
            else if (event.key.code == options.rcw)
                game.rcw();
            else if (event.key.code == options.rccw)
                game.rccw();
            else if (event.key.code == options.r180)
                game.r180();
            else if (event.key.code == options.down)
                game.mDKey();
            else if (event.key.code == options.hd)
                game.hd();
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == options.right)
                game.sRKey();
            else if (event.key.code == options.left)
                game.sLKey();
            else if (event.key.code == options.down)
                game.sDKey();
        }
	}
	else if (gamestate == GameOver) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == sf::Keyboard::Return && !playonline) {
                setGameState(CountDown);
                game.startCountdown();
                game.gameover=false;
            }
        }
	}
}

void UI::windowEvents(sf::Event& event) {
	if (event.type == sf::Event::Closed)
        window->close();
    else if (event.type == sf::Event::Resized && options.currentmode == -1) {
        resizeWindow(event);
    }
}

void UI::resizeWindow(sf::Event& event) {
	sf::View view = window->getView();
    float ratio;
    if ((float)event.size.width/event.size.height > 960.0/600) {
        ratio = (event.size.height * (960.0/600)) / event.size.width;
        view.setViewport(sf::FloatRect((1-ratio)/2.0, 0, ratio, 1));
    }
    else {
        ratio = (event.size.width / (960.0/600)) / event.size.height;
        view.setViewport(sf::FloatRect(0, (1-ratio)/2.0, 1, ratio));
    }
    window->setView(view);
    gui.setView(view);
}

void UI::enlargePlayfield(sf::Event& event) {
	if (gui.get("GameFields")->isVisible()) {
		if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
			sf::FloatRect box;
			if (scaleup) {
				box = scaleup->sprite.getGlobalBounds();
				if (!box.contains(pos)) {
					scaleup->scale=0;
					scaleup->sprite.setScale(currentR, currentR);
					scaleup=0;
				}
			}
			for (auto &&it : fields) {
				box = it.sprite.getGlobalBounds();
				if (box.contains(pos)) {
					if (&it != scaleup) {
						if (scaleup) {
							scaleup->scale=0;
							scaleup->sprite.setScale(currentR, currentR);
						}
						scaleup=&it;
						scaleup->scale=0;
						sclock.restart();
						break;
					}
				}
			}
		}
		else if (event.type == sf::Event::MouseLeft && scaleup) {
			scaleup->scale=0;
			scaleup->sprite.setScale(currentR, currentR);
			scaleup=0;
		}
	}
}

void UI::keyEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			if (gui.get("Login")->isVisible()) {
				closeLogin();
			}
			else if (gui.get("MainMenu")->isVisible()) {
				if (gui.get("AUS")->isVisible()) {
					gui.get("AUS")->hide();
					gui.get("MainMenu")->enable();
				}
				else {
					gui.get<tgui::Label>("AUSL", 1)->setText("Do you want to quit?");
					gui.get("AUS")->show();
					gui.get("MainMenu")->disable();
				}
			}
			else if (gui.get("OptTab")->isVisible())
				gui.get<tgui::Tab>("OptTab")->select("Back");
			else if (gui.get("opTab")->isVisible())
				gui.get<tgui::Tab>("opTab")->select("Back");
			else if (gui.get("InGameTab")->isVisible()) {
				if (gui.get("AUS")->isVisible())
					gui.get("AUS")->hide();
				else {
					gui.get<tgui::Label>("AUSL", 1)->setText("Leave this game?");
					gui.get("AUS")->show();
				}
			}
		}
	}
}

void UI::sendGameData() {
	sf::Time tmp = game.keyclock.getElapsedTime();
	if (tmp>gamedata) {
		gamedata=tmp+sf::milliseconds(100);
		sendPacket100();
	}

	if (game.linesSent > linesSent) {
		sf::Uint8 amount = game.linesSent-linesSent;
		sendPacket5(amount);
		linesSent = game.linesSent;
	}

	if (game.garbageCleared > garbageCleared) {
		sf::Uint8 amount = game.garbageCleared-garbageCleared;
		sendPacket6(amount);
		garbageCleared = game.garbageCleared;
	}

	if (game.linesBlocked > linesBlocked) {
		sf::Uint8 amount = game.linesBlocked-linesBlocked;
		sendPacket7(amount);
		linesBlocked = game.linesBlocked;
	}
}

void UI::sendGameOver() {
	sendPacket3();
	game.sendgameover=false;

	sendPacket100();
}

void UI::sendGameWinner() {
	sendPacket4();
	game.winner=false;
}

void UI::handlePacket() {
	if (net.packetid <100)
		cout << "Packet id: " << (int)net.packetid << endl;
	switch ((int)net.packetid) {
		case 255: //Disconnected from server
			disconnect();
			gui.get("opTab")->hide();
			gui.get("Rooms")->hide();
			gui.get("ServerLobby")->hide();
			gui.get("AUS")->hide();
			gui.get("Connecting")->hide();
			gui.get("MainMenu")->enable();
			gui.get("Login")->hide();
			inroom=false;
			playonline=false;
			ping.setString("0");
			setGameState(MainMenu);
			quickMsg("Disconnected from server");
		break;
		case 100: //Game data
		case 101:
		{
			sf::Uint16 clientid;
			sf::Uint8 datacount;
			net.packet >> clientid >> datacount;
			for (auto&& field : fields)
				if (field.id==clientid) {
					if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
						field.datacount=datacount;
						for (int c=0; net.packet >> compressor.tmp[c]; c++) {}
						compressor.extract();
						if (compressor.validate()) {
							compressor.field = &field;
							compressor.copy();
							drawOppField(field);
						}
					}
					break;
				}
		}
		break;
		case 0: //Welcome packet
		{
			sf::String welcomeMsg;

			net.packet >> myId >> welcomeMsg;

			lobbyMsg("Server", welcomeMsg);

			makeRoomList();
		}
		break;
		case 1://Start countdown
		{
			sf::Uint8 countdown;
			sf::Uint16 seed1, seed2;
			net.packet >> countdown >> seed1 >> seed2;
			game.rander.seedPiece(seed1);
			game.rander.seedHole(seed2);
			game.rander.reset();
			game.startCountdown();
			game.countDown(countdown);
			game.position=0;
			resetOppFields();
			setGameState(CountDown);
			gamedatacount=251;
			sendPacket100();
		}
		break;
		case 2://Countdown
		{
			if (gamestate != CountDown)
				setGameState(CountDown);
			if (!game.rander.total)
				game.startCountdown();
			sf::Uint8 countdown;
			net.packet >> countdown;
			game.countDown(countdown);
			if (!countdown)
				setGameState(Game);
			else {
				gamedatacount=255-countdown;
				sendPacket100();
			}
		}
		break;
		case 3://Join room ok/no
		{
			sf::Uint8 joinok;
			net.packet >> joinok;
			if (joinok == 1) {
				sf::Uint8 playersinroom;
				sf::Uint16 playerid, seed1, seed2;
				net.packet >> seed1 >> seed2 >> playersinroom;
				game.rander.seedPiece(seed1);
				game.rander.seedHole(seed2);
				game.rander.reset();
				obsField newfield(resources);
				newfield.clear();
				sf::String name;
				for (int c=0; c<playersinroom; c++) {
					net.packet >> playerid >> name;
					newfield.id = playerid;
					newfield.setName(name);
					addField(newfield);
				}
				inroom=true;
				setGameState(GameOver);
				gui.get("InGameTab")->show();
				gui.get<tgui::Tab>("InGameTab")->select(0);
	            gui.get("GameFields")->show();
	            gui.get("opTab")->hide();
	            gui.get("Rooms")->hide();
				game.field.clear();
				game.countdownText.setString("");
				game.preDraw();
			}
			else if (joinok == 2)
				quickMsg("Room is full");
			else
				quickMsg("Please wait for server to get your user-data");
		}
		break;
		case 4: //Another player joined room
		{
			sf::String name;
			obsField newfield(resources);
			newfield.clear();
			net.packet >> newfield.id >> name;
			newfield.setName(name);
			addField(newfield);
		}
		break;
		case 5: //Another player left the room
		{
			sf::Uint16 removeid;
			net.packet >> removeid;
			removeField(removeid);
		}
		break;
		case 6: //Round ended
			game.gameover=true;
		break;
		case 7: //Round ended and you where the winner
			game.gameover=true;
			game.winner=true;
			game.autoaway=false;
		break;
		case 8: // Round score data
		{
			sf::Uint8 count;
			net.packet >> count;
			clearScore();
			for (int i=0; i<count; i++)
				scoreRow();
		}
		break;
		case 9: // Auth result
		{
			sf::Uint8 success;
			net.packet >> success;
			if (success == 1) {
				sf::String name;
				net.packet >> name >> myId;
				game.field.setName(name);
				gui.get("Connecting")->hide();
				gui.get("opTab")->show();
				gui.get("Rooms")->show();
				gui.get<tgui::Tab>("opTab")->select(0);
				gui.get("MainMenu")->enable();
			}
			else if (success == 2) {
				gui.get("Connecting")->hide();
				gui.get("opTab")->show();
				gui.get("Rooms")->show();
				gui.get<tgui::Tab>("opTab")->select(0);
				gui.get("MainMenu")->enable();
			}
			else {
				disconnect();
				if (success == 3)
					quickMsg("You have the wrong client version");
				else if (success == 4)
					quickMsg("Name already in use");
				else
					quickMsg("Authentication failed");
				gui.get("Connecting")->hide();
				gui.get("MainMenu")->show();
				gui.get("Login")->show();
				ping.setString("0");
			}
		}
		break;
		case 10: //Garbage received
		{
			sf::Uint8 amount;
			net.packet >> amount;
			game.addGarbage(amount);
		}
		break;
		case 11: //Server telling me to reset my oppfields. This is the same as Packet 1, but when client is away.
		{
			resetOppFields();
			sf::Uint16 seed;
			net.packet >> seed;
			game.rander.seedPiece(seed);
			net.packet >> seed;
			game.rander.seedHole(seed);
			game.rander.reset();
			game.position=0;
			game.countdownText.setString("");
			game.drawGameOver();
		}
		break;
		case 12: // Incoming chat msg
		{
			sf::String from, msg;
			sf::Uint8 type;
			net.packet >> type >> from >> msg;
			if (type == 3)
				privMsg(from, msg);
			else if (type == 2)
				lobbyMsg(from, msg);
			else
				roomMsg(from, msg);
		}
		break;
		case 13: // Another player went away
		{
			sf::Uint16 id;
			net.packet >> id;
			for (auto&& field : fields)
				if (field.id == id) {
					field.away=true;
					drawOppField(field);
				}
		}
		break; // Another player came back
		case 14:
		{
			sf::Uint16 id;
			net.packet >> id;
			for (auto&& field : fields)
				if (field.id == id) {
					field.away=false;
					drawOppField(field);
				}
		}
		break;
		case 15: // Server reported the position of a player
		{
			sf::Uint16 id;
			sf::Uint8 position;
			net.packet >> id >> position;
			for (auto&& field : fields)
				if (field.id == id) {
					field.position = position;
					drawOppField(field);
				}
			if (id == myId) {
				game.position = position;
				game.drawGameOver();
			}
		}
		break;
		case 16: // Server sending room list
			// This is not being used yet, but you could put a "refresh" button in the lobby for the furture?
			makeRoomList();
		break;
		case 17: // New room created
			addRoom();
		break;
		case 18: // Room was removed
		{
			sf::Uint16 id;
			net.packet >> id;
			roomList.removeItem(id);
		}
		break;
		case 19: // UDP-port was established by server
			udpConfirmed=true;
		break;
		case 102: // Ping packet returned from server
		{
			sf::Uint8 pingId;
			sf::Uint16 clientid;
			net.packet >> clientid >> pingId;
			if (pingId == pingIdCount) {
				sf::Time pingResult = delayClock.getElapsedTime() - pingTime;
				setPing(pingResult);
				pingReturned=true;
			}
		}
		break;
	}
}