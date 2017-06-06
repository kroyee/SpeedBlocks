#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "network.h"
#include "MainMenu.h"
#include "LoginBox.h"
#include "GameOptions.h"
#include "Connecting.h"
#include "GameplayUI.h"
#include "OnlineplayUI.h"
#include "AreYouSure.h"
#include "PerformanceOutput.h"
#include <iostream>
using std::cout;
using std::endl;

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
	fields.clear();
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
		field.datacount=250;
		field.clear();
		drawOppField(field);
	}
}

void UI::drawOppField(obsField& field) {
	field.drawField();

	field.drawNextPiece();
}

void UI::drawFields() {
	if (!gameplayUI->GameFields->isVisible())
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
			sf::FloatRect pos = scaleup->sprite.getGlobalBounds();
			sf::RectangleShape blackback;
			blackback.setPosition(pos.left, pos.top);
			blackback.setSize({pos.width, pos.height});
			blackback.setFillColor(sf::Color::Black);
			window->draw(blackback);
			window->draw(scaleup->sprite);
		}
	}
}

void UI::goAway() {
	away=true;
	sendPacket8();
	game.gameover=true;
	game.sendgameover=true;
	game.field.text.away=true;
	game.draw();
}

void UI::unAway() {
	away=false;
	game.autoaway=false;
	sendPacket9();
	game.field.text.away=false;
	game.draw();
}

void UI::ready() {
	if (game.field.text.ready) {
		sendPacket13();
		game.field.text.ready=false;
	}
	else {
		sendPacket12();
		game.field.text.ready=true;
	}
	game.draw();
}

void UI::handleEvent(sf::Event& event) {
	gameInput(event);
	windowEvents(event);
	
	if (gameOptions->SelectKey->isVisible())
		gameOptions->putKey(event);
	
	if (gameplayUI->isVisible())
		enlargePlayfield(event);

	keyEvents(event);

	if (onlineplayUI->isVisible()) {
		onlineplayUI->roomList.scrolled(event);
		onlineplayUI->tournamentList.scrolled(event);
	}

	tGui.handleEvent(event);
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
	else if (gamestate == Game || gamestate == Practice) {
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
            if (event.key.code == sf::Keyboard::Return) {
            	if (playonline) {
            		game.gameover=false;
            		setGameState(Practice);
            	}
            	else {
	                setGameState(CountDown);
	                game.startCountdown();
	                game.gameover=false;
            	}
            }
            else if (event.key.code == options.ready)
            	ready();
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
    tGui.setView(view);
}

void UI::enlargePlayfield(sf::Event& event) {
	if (gameplayUI->GameFields->isVisible()) {
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
			if (loginBox->isVisible()) {
				loginBox->closeLogin();
			}
			else if (mainMenu->isVisible()) {
				if (areYouSure->isVisible()) {
					areYouSure->hide();
					mainMenu->enable();
				}
				else {
					areYouSure->label->setText("Do you want to quit?");
					areYouSure->show();
					mainMenu->disable();
				}
			}
			else if (gameOptions->isVisible())
				gameOptions->otab->select("Back");
			else if (onlineplayUI->isVisible())
				onlineplayUI->opTab->select("Back");
			else if (gameplayUI->isVisible()) {
				if (areYouSure->isVisible())
					areYouSure->hide();
				else {
					areYouSure->label->setText("Leave this game?");
					areYouSure->show();
				}
			}
		}
	}
}

void UI::sendGameData() {
	sf::Time tmp = game.gameclock.getElapsedTime();
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
			onlineplayUI->hide();
			areYouSure->hide();
			connectingScreen->hide();
			mainMenu->enable();
			loginBox->hide();
			inroom=false;
			playonline=false;
			performanceOutput->ping->hide();
			setGameState(MainMenu);
			quickMsg("Disconnected from server");
		break;
		case 100: //Game data
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

			gameplayUI->Lobby->addLine("Server: " + welcomeMsg);
			onlineplayUI->Lobby->addLine("Server: " + welcomeMsg);

			onlineplayUI->makeRoomList();
			onlineplayUI->makeClientList();
			performanceOutput->ping->show();
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
					addField(newfield);
					fields.back().text.setName(name);
				}
				inroom=true;
				setGameState(GameOver);
				game.field.clear();
				game.draw();
			}
			else if (joinok == 2)
				quickMsg("Room is full");
			else if (joinok == 3)
				quickMsg("Please wait for server to get your user-data");
			else
				quickMsg("This is not your game");
		}
		break;
		case 4: //Another player joined room
		{
			sf::String name;
			obsField newfield(resources);
			newfield.clear();
			net.packet >> newfield.id >> name;
			addField(newfield);
			fields.back().text.setName(name);
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
			gameplayUI->clearScore();
			for (int i=0; i<count; i++)
				gameplayUI->scoreRow();
		}
		break;
		case 9: // Auth result
		{
			sf::Uint8 success;
			net.packet >> success;
			if (success == 1) {
				sf::String name;
				net.packet >> name >> myId;
				game.field.text.setName(name);
				connectingScreen->hide();
				onlineplayUI->show();
				onlineplayUI->opTab->select(0);
				mainMenu->enable();
			}
			else if (success == 2) {
				connectingScreen->hide();
				onlineplayUI->show();
				onlineplayUI->opTab->select(0);
				mainMenu->enable();
			}
			else {
				disconnect();
				if (success == 3)
					quickMsg("You have the wrong client version");
				else if (success == 4)
					quickMsg("Name already in use");
				else
					quickMsg("Authentication failed");
				connectingScreen->hide();
				mainMenu->show();
				loginBox->show();
				performanceOutput->ping->hide();
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
			game.field.clear();
			game.draw();
		}
		break;
		case 12: // Incoming chat msg
			getMsg();
		break;
		case 13: // Another player went away
		{
			sf::Uint16 id;
			net.packet >> id;
			for (auto&& field : fields)
				if (field.id == id) {
					field.text.away=true;
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
					field.text.away=false;
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
					field.text.setPosition(position);
					drawOppField(field);
				}
			if (id == myId) {
				game.field.text.setPosition(position);
				game.draw();
			}
		}
		break;
		case 16: // Server sending room list
			// This is not being used yet, but you could put a "refresh" button in the lobby for the furture?
			onlineplayUI->makeRoomList();
		break;
		case 17: // New room created
			onlineplayUI->addRoom();
		break;
		case 18: // Room was removed
		{
			sf::Uint16 id;
			net.packet >> id;
			onlineplayUI->roomList.removeItem(id);
		}
		break;
		case 19: // UDP-port was established by server
			udpConfirmed=true;
		break;
		case 20: // Another client connected to the server
			onlineplayUI->addClient();
		break;
		case 21: // Another client left the server
			onlineplayUI->removeClient();
		break;
		case 22: // Get tournament list
			onlineplayUI->makeTournamentList();
		break;
		case 23: // Get tournament info
			onlineplayUI->tournamentPanel.getInfo(myId);
		break;
		case 24: // Get tournament game standings
			game.field.text.setResults(myId);
			game.draw();
		break;
		case 25: // Players is ready
		{
			sf::Uint16 clientid;
			net.packet >> clientid;
			for (auto&& field : fields)
				if (field.id == clientid) {
					field.text.ready=true;
					field.drawField();
				}
		}
		break;
		case 26: // Player is not ready
		{
			sf::Uint16 clientid;
			net.packet >> clientid;
			for (auto&& field : fields)
				if (field.id == clientid) {
					field.text.ready=false;
					field.drawField();
				}
		}
		break;
		case 27: // Tournament update
			onlineplayUI->tournamentPanel.getUpdate(myId);
		break;
		case 102: // Ping packet returned from server
		{
			sf::Uint8 pingId;
			sf::Uint16 clientid;
			net.packet >> clientid >> pingId;
			if (pingId == performanceOutput->pingIdCount) {
				sf::Time pingResult = delayClock.getElapsedTime() - performanceOutput->pingTime;
				performanceOutput->setPing(pingResult.asMilliseconds());
				performanceOutput->pingReturned=true;
			}
		}
		break;
		case 254: // Signal packet
			handleSignal();
		break;
	}
}

void UI::handleSignal() {
	sf::Uint8 signalId;
	sf::Uint16 id1, id2;

	net.packet >> signalId;
	if (!net.packet.endOfPacket())
		net.packet >> id1;
	if (!net.packet.endOfPacket())
		net.packet >> id2;

	switch (signalId) {
		case 0:
			quickMsg("Not enough players to start tournament");
		break;
		case 1: // A tournament game is ready
			onlineplayUI->alertMsg(id1);
		break;
	}
}