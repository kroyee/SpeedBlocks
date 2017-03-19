#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "network.h"
#include <iostream>
using namespace std;

void GameFieldDrawer::addField(obsField& field) {
	fields.push_back(field);
	calFieldPos();
	drawOppField(fields.back());
}

void GameFieldDrawer::removeField(sf::Uint16 id) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == id) {
			it = fields.erase(it);
			break;
		}
	calFieldPos();
}

void GameFieldDrawer::removeAllFields() {
	while (fields.size())
		fields.pop_front();
}

void GameFieldDrawer::updateField(obsField& field) {
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

void GameFieldDrawer::calFieldPos() {
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

void GameFieldDrawer::resetOppFields() {
	for (auto&& field : fields) {
		field.datacount=250;
		field.nextpiece=game->nextpiece;
		field.posX=0;
		field.posY=0;
		field.clear();
		drawOppField(field);
	}
}

void GameFieldDrawer::drawOppField(obsField& field) {
	field.drawField();

	for (int rot=0; rot < field.nprot; rot++)
		options->basepiece[field.nextpiece].rcw();
	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (options->basepiece[field.nextpiece].grid[y][x] != 0) {
                	field.tile[field.npcol-1].setPosition(sf::Vector2f(-15*options->basepiece[field.nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[field.npcol-1]);
            	}
    for (int rot=0; rot < field.nprot; rot++)
		options->basepiece[field.nextpiece].rccw();
}

void GameFieldDrawer::drawFields() {
	if (!gui.get("GameFields")->isVisible())
		return;
	for (auto &&it : fields)
		window->draw(it.sprite);
	if (scaleup) {
		scaleup->scale += sclock.restart().asMilliseconds() / 5;
		if (scaleup->scale > 100)
			scaleup->scale = 100;
		float maxRat = 0.85, add;
		add = (maxRat-currentR)/100;
		if (add>0) {
			scaleup->sprite.setScale(currentR+add*scaleup->scale, currentR+add*scaleup->scale);
			window->draw(scaleup->sprite);
		}
	}
}

sf::String GameFieldDrawer::getName(sf::Uint16 id) {
	if (id == myId)
		return game->field.name;
	for (auto&& field : fields)
		if (field.id == id)
			return field.name;
	return "Unknown";
}

void GameFieldDrawer::goAway() {
	away=true;
	net->packet.clear();
	sf::Uint8 packetid = 8; //8-Packet
	net->packet << packetid;
	net->sendTCP();
	game->gameover=true;
	game->sendgameover=true;
	game->awayText.setString("Away");
	game->drawGameOver();
}

void GameFieldDrawer::unAway() {
	away=false;
	net->packet.clear();
	sf::Uint8 packetid = 9; //9-Packet
	net->packet << packetid;
	net->sendTCP();
	game->awayText.setString("");
	game->drawGameOver();
}

void GameFieldDrawer::handleEvent(sf::Event event) {
	gui.handleEvent(event);
	if (setkey)
		putKey(event);
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
	if (event.type == sf::Event::KeyPressed)
		if (event.key.code == sf::Keyboard::Escape) {
			if (chatFocused) {
				gui.get("ChatBox", 1)->unfocus();
				gui.get("slChatBox", 1)->unfocus();
			}
			else if (gui.get("Login")->isVisible()) {
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
	if (gui.get("Rooms")->isVisible())
		if (event.type == sf::Event::MouseWheelScrolled)
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (!gui.get("RoomScroll", 1)->isFocused()) {
					short cur = gui.get<tgui::Scrollbar>("RoomScroll", 1)->getValue();
					cur-=event.mouseWheelScroll.delta;
					if (cur<0)
						cur=0;
					gui.get<tgui::Scrollbar>("RoomScroll", 1)->setValue(cur);
				}
}

void GameFieldDrawer::sendGameData() { //UDP-Packet
	sf::Time tmp = game->keyclock.getElapsedTime();
	if (tmp>gamedata) {
		gamedata=tmp+sf::milliseconds(100);
		compressor.compress();
		net->packet.clear();
		sf::Uint8 packetid = 100;
		net->packet << packetid << myId << gamedatacount;
		gamedatacount++;
		for (int i=0; i<compressor.tmpcount; i++)
			net->packet << compressor.tmp[i];
		if (compressor.bitcount>0)
			net->packet << compressor.tmp[compressor.tmpcount];
		net->sendUDP();
	}

	if (game->linesSent > linesSent) { //5-Packet
		sf::Uint8 packetid=5;
		sf::Uint8 amount = game->linesSent-linesSent;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		linesSent = game->linesSent;
	}

	if (game->garbageCleared > garbageCleared) { //6-Packet
		sf::Uint8 packetid=6;
		sf::Uint8 amount = game->garbageCleared-garbageCleared;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		garbageCleared = game->garbageCleared;
	}

	if (game->linesBlocked > linesBlocked) { //7-Packet
		sf::Uint8 packetid=7;
		sf::Uint8 amount = game->linesBlocked-linesBlocked;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		linesBlocked = game->linesBlocked;
	}
}

void GameFieldDrawer::sendGameOver() { //3-Packet
	net->packet.clear();
	sf::Uint8 packetid = 3;
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
	game->sendgameover=false;
}

void GameFieldDrawer::sendGameWinner() { //4-Packet
	net->packet.clear();
	sf::Uint8 packetid = 4;
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
	game->winner=false;
}

void GameFieldDrawer::handlePacket() {
	if (net->packetid !=100)
		cout << "Packet id: " << (int)net->packetid << endl;
	switch ((int)net->packetid) {
		case 255: //Disconnect
			gui.get("opTab")->hide();
			gui.get("Rooms")->hide();
			gui.get("ServerLobby")->hide();
			gui.get("AUS")->hide();
			gui.get("Connecting")->hide();
			gui.get("MainMenu")->enable();
			mainMenu();
			disconnect=true;
			playonline=false;
		break;
		case 100: //Game data
		{
			sf::Uint16 clientid;
			sf::Uint8 datacount;
			net->packet >> clientid >> datacount;
			for (auto&& field : fields)
				if (field.id==clientid) {
					if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
						field.datacount=datacount;
						for (int c=0; net->packet >> compressor.tmp[c]; c++) {}
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
			sf::String name;
			sf::Uint8 roomCount, maxPlayers, currentPlayers;
			sf::Uint16 id;

			net->packet >> myId >> name >> roomCount;

			lobbyMsg("Server", name);

			removeAllRooms();

			for (int i=0; i<roomCount; i++) {
				net->packet >> id >> name >> currentPlayers >> maxPlayers;
				addRoom(name, currentPlayers, maxPlayers, id);
			}
		}
		break;
		case 1://Start countdown
		{
			sf::Uint8 countdown;
			sf::Uint16 seed1, seed2;
			net->packet >> countdown >> seed1 >> seed2;
			game->rander.seedPiece(seed1);
			game->rander.seedHole(seed2);
			game->rander.reset();
			game->startCountdown();
			game->countDown(countdown);
			resetOppFields();
			startcount=true;
		}
		break;
		case 2://Countdown
		{
			sf::Uint8 countdown;
			net->packet >> countdown;
			game->countDown(countdown);
			if (!countdown) {
				startgame=true;
				gamedatacount=0;
				gamedata=sf::seconds(0);
			}
		}
		break;
		case 3://Join room ok/no
		{
			sf::Uint8 joinok;
			net->packet >> joinok;
			if (joinok) {
				sf::Uint8 playersinroom;
				sf::Uint16 playerid;
				net->packet >> playersinroom;
				obsField newfield(textureBase->tile, &textureBase->fieldBackground);
				newfield.clear();
				sf::String name;
				for (int c=0; c<playersinroom; c++) {
					net->packet >> playerid >> name;
					newfield.id = playerid;
					newfield.setName(name, *printFont);
					addField(newfield);
				}
				inroom=true;
				gui.get("InGameTab")->show();
				gui.get<tgui::Tab>("InGameTab")->select(0);
	            gui.get("GameFields")->show();
	            gui.get("opTab")->hide();
	            gui.get("Rooms")->hide();
	            game->makeNewPiece();
				game->field.clear();
				game->countdownText.setString("");
				game->preDraw();
			}
			else {} //Show "Couldn't join room" message
		}
		break;
		case 4: //Another player joined room
		{
			sf::String name;
			obsField newfield(textureBase->tile, &textureBase->fieldBackground);
			newfield.clear();
			net->packet >> newfield.id >> name;
			newfield.setName(name, *printFont);
			addField(newfield);
		}
		break;
		case 5: //Another player left the room
		{
			sf::Uint16 removeid;
			net->packet >> removeid;
			removeField(removeid);
		}
		break;
		case 6: //Round ended
			game->gameover=true;
		break;
		case 7: //Round ended and you where the winner
			game->gameover=true;
			game->winner=true;
		break;
		case 8: // Round score data
		{
			sf::Uint8 count, maxcombo, rank, position;
			sf::Uint16 id, linesSent, linesRecieved, linesBlocked, bpm, spm, score;
			net->packet >> count;
			clearScoreBox();
			for (int i=0; i<count; i++) {
				net->packet >> id >> maxcombo >> linesSent >> linesRecieved >> linesBlocked >> bpm >> spm >> rank >> position >> score;
				printScoreBox(getName(id), score, rank, bpm, maxcombo, linesSent, spm, linesRecieved, linesBlocked);
			}
		}
		break;
		case 9: // Auth result
		{
			sf::Uint8 success;
			net->packet >> success;
			if (success == 1) {
				sf::String name;
				net->packet >> name >> myId;
				game->field.setName(name, *printFont);
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
				net->disconnect();
				playonline=false;
				if (success == 3)
					quickMsg("You have the wrong client version");
				else
					quickMsg("Authentication failed");
				gui.get("Connecting")->hide();
				gui.get("MainMenu")->show();
				gui.get("Login")->show();
			}
		}
		break;
		case 10: //Garbage received
		{
			sf::Uint8 amount;
			net->packet >> amount;
			game->addGarbage(amount);
		}
		break;
		case 11: //Server telling me to reset my oppfields
			resetOppFields();
		break;
		case 12:
		{
			sf::String from, msg;
			sf::Uint8 type;
			net->packet >> type >> from >> msg;
			if (type == 3)
				privMsg(from, msg);
			else if (type == 2)
				lobbyMsg(from, msg);
			else
				roomMsg(from, msg);
		}
		break;
	}
}