#include "gui.h"
#include "sounds.h"
#include "gamePlay.h"
#include "network.h"
#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
using namespace std;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

UI::UI(sf::RenderWindow& window_,
	gamePlay& game_)
    : typewriter(game_.resources.gfx.typewriter),
      printFont2(game_.resources.gfx.printFont),
      gui(window_),
      resources(game_.resources),
      options(game_.resources.options),
      game(game_),
      net(game_.resources.net),
      window(&window_),
      training(false),
      playonline(false),
      setkey(false),
      adjPieces(false),
      updPieces(false),
      chatFocused(false),
      inroom(false),
      away(false),
      key(nullptr),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      clientVersion(0),
      scoreRows(0),
      udpConfirmed(false),
      pingReturned(false),
      pingIdCount(0),
      pingColor(255),
      gamestate(MainMenu),
      spamCount(0),
      scaleup(0),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(0),
      unknown("Unknown") {

    compressor.game=&game;

	themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");
	themeBB = tgui::Theme::create(resourcePath() + "media/BabyBlue.txt");

	initSprites();

	createAllWidgets();


	performanceLabel.setFont(resources.gfx.printFont);
	performanceLabel.setCharacterSize(10);
	performanceLabel.setColor(sf::Color::White);
	performanceLabel.setPosition(846,2);
	performanceLabel.setString("LFT IPS  FPS MS");

	ping.setFont(resources.gfx.printFont);
	ping.setCharacterSize(12);
	ping.setColor(sf::Color::Black);
	ping.setPosition(932,12);
	ping.setString("0");
	frameRate.setFont(resources.gfx.printFont);
	frameRate.setCharacterSize(12);
	frameRate.setColor(sf::Color::Black);
	frameRate.setPosition(902,12);
	inputRate.setFont(resources.gfx.printFont);
	inputRate.setCharacterSize(12);
	inputRate.setColor(sf::Color::Black);
	inputRate.setPosition(872,12);
	longestFrame.setFont(resources.gfx.printFont);
	longestFrame.setCharacterSize(12);
	longestFrame.setColor(sf::Color::Black);
	longestFrame.setPosition(849,12);
}

void UI::sendReport(tgui::ChildWindow::Ptr win) {
	CURL *curl;
	CURLcode res;

	sf::String happened = win->get<tgui::TextBox>("happened")->getText();
	sf::String expected = win->get<tgui::TextBox>("expected")->getText();
	sf::String reproduce = win->get<tgui::TextBox>("reproduce")->getText();
	sf::String contact = win->get<tgui::TextBox>("contact")->getText();

	sf::String postfield = "{\"happening\":\"" + happened + "\",\"supposed\":\"" + expected + "\",\"reproduce\":\"" + reproduce + "\",\"contact\":\"" + contact + "\"}";
	cout << postfield.toAnsiString().c_str() << endl;

	char * cstr = new char [postfield.getSize()+1];
	std::strcpy (cstr, postfield.toAnsiString().c_str());

	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */ 
		struct curl_slist *headers = NULL;

		curl_easy_setopt(curl, CURLOPT_URL, "https://speedblocks.spdns.org/bugs");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cstr);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));

		curl_slist_free_all(headers);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
		cout << "Curl failed to load" << endl;
	curl_global_cleanup();
	win->destroy();
	delete[] cstr;
	cout << endl;
}

void UI::bugReport() {
	tgui::ChildWindow::Ptr ChW = themeBB->load("ChildWindow");
	ChW->setTitleButtons(static_cast<tgui::ChildWindow::TitleButtons>(3));
	ChW->connect("Minimized", &UI::minimize, this);
	ChW->connect("Closed", &UI::close, this);
	ChW->setOpacity(0.95);
	ChW->setPosition(500, 40);
	ChW->setSize(400, 475);
	gui.add(ChW);

	tgui::Label::Ptr WhL = themeBB->load("Label");
	WhL->setPosition(10, 5);
	WhL->setText("What happened?");
	ChW->add(WhL);

	tgui::TextBox::Ptr WhT = themeBB->load("TextBox");
	WhT->setPosition(5, 40);
	WhT->setSize(390, 80);
	ChW->add(WhT, "happened");

	tgui::Label::Ptr WhL2 = themeBB->load("Label");
	WhL2->setPosition(10, 125);
	WhL2->setText("What did you expect to happen?");
	ChW->add(WhL2);

	tgui::TextBox::Ptr WhT2 = themeBB->load("TextBox");
	WhT2->setPosition(5, 160);
	WhT2->setSize(390, 80);
	ChW->add(WhT2, "expected");

	tgui::Label::Ptr WhL3 = themeBB->load("Label");
	WhL3->setPosition(10, 245);
	WhL3->setText("How can we reproduce this?");
	ChW->add(WhL3);

	tgui::TextBox::Ptr WhT3 = themeBB->load("TextBox");
	WhT3->setPosition(5, 280);
	WhT3->setSize(390, 80);
	ChW->add(WhT3, "reproduce");

	tgui::Label::Ptr WhL4 = themeBB->load("Label");
	WhL4->setPosition(10, 365);
	WhL4->setText("How can we contact you with questions?");
	ChW->add(WhL4);

	tgui::TextBox::Ptr WhT4 = themeBB->load("TextBox");
	WhT4->setPosition(5, 400);
	WhT4->setSize(390, 40);
	ChW->add(WhT4, "contact");

	tgui::Button::Ptr send = themeBB->load("Button");
	send->setPosition(170, 445);
	send->setSize(60, 30);
	send->setText("Send");
	send->connect("Pressed", &UI::sendReport, this, ChW);
	ChW->add(send);
}

void UI::createRoom(const sf::String& name, const sf::String& maxplayers) {
	if (!name.getSize())
		return;
	if (!maxplayers.getSize())
		return;
	sendPacket11(name, stoi(maxplayers.toAnsiString()) );
	gui.get<tgui::Tab>("opTab")->select(0);
}

void UI::close(tgui::ChildWindow::Ptr win) {
	win->destroy();
}

void UI::minimize(tgui::ChildWindow::Ptr win) {
	if (win->getSize().y == 0)
		win->setSize(win->getSize().x, 300);
	else
		win->setSize(win->getSize().x, 0);
}

void UI::makeRoomList() {
	sf::Uint8 roomCount;

	net.packet >> roomCount;
	roomList.removeAllItems();

	for (int i=0; i<roomCount; i++)
		addRoom();
}

void UI::addRoom() {
	sf::String name;
	sf::Uint8 maxPlayers, currentPlayers;
	sf::Uint16 id;
	net.packet >> id >> name >> currentPlayers >> maxPlayers;
	sf::String roomlabel = to_string(currentPlayers);
	if (maxPlayers)
		roomlabel += "/" + to_string(maxPlayers);
	roomlabel+= " players";
	roomList.addItem(name, roomlabel, id);
}

void UI::joinRoom(sf::Uint16 id) {
	sendPacket0(id);
	away=false;
	game.autoaway=false;
}

void UI::leaveRoom() {
	sendPacket1();
	inroom=false;
	setGameState(MainMenu);
}

void UI::login(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	gui.get("MainMenu")->hide();
	gui.get("Login")->hide();
	gui.get("Connecting")->show();
	window->draw(resources.gfx.background); //Update the screen so a block on connect will show the connecting screen
	gui.draw();
	window->display();
	net.serverAdd = gui.get<tgui::EditBox>("IPAddr", true)->getText().toAnsiString();
	if (net.connect() == sf::Socket::Done) {
		net.udpSock.unbind();
		net.udpSock.bind(sf::Socket::AnyPort);
		net.localUdpPort = net.udpSock.getLocalPort();
		sendPacket2(name, pass, guest);
		playonline=true;
		if (guest)
			game.field.setName(name);
	}
	else {
		net.disconnect();
		quickMsg("Could not connect to server");
		gui.get("Connecting")->hide();
		gui.get("Login")->show();
		gui.get("MainMenu")->show();
	}
}

void UI::closeLogin() {
	gui.get("MainMenu")->enable();
	gui.get("Login")->hide();
}

void UI::playOnline() {
	gui.get("MainMenu")->disable();
	gui.get("Login")->show();
	gui.get("Username", 1)->focus();
}

void UI::Training() {
	training=true;
	setGameState(CountDown);
    game.startCountdown();
}

void UI::setGameState(GameStates state) {
	if (gamestate == MainMenu) { // Reset depending on what state we come from
		gui.get("MainMenu")->hide();
		gui.get("opTab")->hide();
	    gui.get("Rooms")->hide();
	    gui.get("ServerLobby")->hide();
		gui.get("CreateRoom")->hide();
		if (state != MainMenu) {
			gui.get("InGameTab")->show();
			gui.get<tgui::Tab>("InGameTab")->select(0);
    		gui.get("GameFields")->show();
		}
	}

	if (state == MainMenu) { // Set depending on what state we are going into
		if (playonline) {
			gui.get("opTab")->show();
			gui.get("Rooms")->show();
			gui.get<tgui::Tab>("opTab")->select(0);
			gui.get("InGameTab")->hide();
			gui.get("Chat")->hide();
			gui.get("Score")->hide();
			gui.get("GameFields")->hide();
			removeAllFields();
		}
		else
			mainMenu();
	}
	else if (state == CountDown) {
        game.sRKey();
        game.sLKey();
        game.sDKey();
        game.gameover=false;
	}
	else if (state == Game) {
		linesSent=0;
        garbageCleared=0;
        linesBlocked=0;
        gamedatacount=0;
		gamedata=sf::seconds(0);
        game.startGame();
	}
	else if (state == GameOver) {
        if (game.autoaway)
            goAway();
        if (game.sendgameover)
            sendGameOver();
        if (game.winner)
            sendGameWinner();
	}
	else if (state == Replay) {
		game.startReplay();
	}

	gamestate = state;
}

void UI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		gui.get("Rooms")->show();
		gui.get("Tournaments")->hide();
		gui.get("ServerLobby")->hide();
		gui.get("CreateRoom")->hide();
	}
	else if (tab == "Lobby") {
		gui.get("ServerLobby")->show();
		gui.get("Rooms")->hide();
		gui.get("Tournaments")->hide();
		gui.get("CreateRoom")->hide();
	}
	else if (tab == "Tournaments") {
		gui.get("Tournaments")->show();
		gui.get("Rooms")->hide();
		gui.get("ServerLobby")->hide();
		gui.get("CreateRoom")->hide();
	}
	else if (tab == "Create room") {
		gui.get("ServerLobby")->hide();
		gui.get("Rooms")->hide();
		gui.get("Tournaments")->hide();
		gui.get("CreateRoom")->show();
	}
	else if (tab == "Back") {
		gui.get("ServerLobby")->hide();
		gui.get("Rooms")->hide();
		gui.get("Tournaments")->hide();
		gui.get("opTab")->hide();
		gui.get("CreateRoom")->hide();
		gui.get("MainMenu")->show();
		disconnect();
	}
}

void UI::disconnect() {
	net.disconnect();
	playonline=false;
	udpConfirmed=false;
	ping.setString("0");
	pingColor=255;
}

void UI::ausY() {
	if (playonline)
		leaveRoom();
	else if (gamestate == MainMenu)
		window->close();
	else
		setGameState(MainMenu);
	gui.get("AUS")->hide();
}

void UI::ausN() {
	gui.get("AUS")->hide();
	gui.get("MainMenu")->enable();
}

void UI::quickMsg(const sf::String& msg) {
	gui.get<tgui::Label>("QuickMsg")->setText(msg);
	gui.get<tgui::Label>("QuickMsg")->show();
	quickMsgTime = delayClock.getElapsedTime();
}

void UI::chatFocus(bool i) {
	if (i) {
		chatFocused=true;
		window->setKeyRepeatEnabled(true);
	}
	else {
		chatFocused=false;
		window->setKeyRepeatEnabled(false);
	}
}

void UI::sendMsg(const sf::String& to, const sf::String& msg) {
	if (!msg.getSize()) {
		gui.get("ChatBox", 1)->unfocus();
		return;
	}
	if (spamCount>7000) {
		gui.get<tgui::ChatBox>(to, 1)->addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", sf::Color(200, 200, 50));
		if (to == "Lobby")
			gui.get<tgui::ChatBox>("Lobby2", 1)->addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", sf::Color(200, 200, 50));
		gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
		gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
		spamCount=12000;
		return;
	}
	if (msg[0]=='/' && msg[1]=='w' && msg[2]==' ') {
		short until = msg.find(' ', 3);
		sf::String privto = msg.substring(3, until-3);
		sf::String privmsg = msg.substring(until, sf::String::InvalidPos);
		sendPacket10(privto, privmsg);
		gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
		gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
		spamCount+=2000;
		return;
	}
	sf::String postmsg = game.field.name + ": " + msg;
	gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
	gui.get<tgui::EditBox>("slChatBox", 1)->setText("");

	for (unsigned int i=0; i<msg.getSize(); i++)
		if (msg[i] != ' ') {
			gui.get<tgui::ChatBox>(to, 1)->addLine(postmsg, sf::Color(200, 200, 50));
			if (to == "Lobby")
				gui.get<tgui::ChatBox>("Lobby2", 1)->addLine(postmsg, sf::Color(200, 200, 50));
			sendPacket10(to, msg);
			spamCount+=2000;
			return;
		}
}

void UI::chattabSelect(const std::string& tab) {
	for (size_t i = 0; i < privChats.size(); i++)
		privChats[i].chatBox->hide();
	gui.get("Lobby", 1)->hide();
	gui.get("Room", 1)->hide();
	gui.get(tab, 1)->show();
}

void UI::roomMsg(const sf::String& name, const sf::String& msg) {
	gui.get<tgui::ChatBox>("Room", 1)->addLine(name + ": " + msg);
}

void UI::lobbyMsg(const sf::String& name, const sf::String& msg) {
	gui.get<tgui::ChatBox>("Lobby", 1)->addLine(name + ": " + msg);
	gui.get<tgui::ChatBox>("Lobby2", 1)->addLine(name + ": " + msg);
}

void UI::privMsg(const sf::String& name, const sf::String& msg) {
	short create=-1;
	for (size_t i = 0; i < privChats.size(); i++)
		if (privChats[i].name == name) {
			create=i;
		}
	if (create == -1) {
		privChat newchat;
		newchat.name=name;
		privChats.push_back(move(newchat));
		create=privChats.size()-1;


		privChats[create].chatBox = themeBB->load("ChatBox");
		privChats[create].chatBox->setPosition(5, 35);
		privChats[create].chatBox->setSize(480, 475);
		privChats[create].chatBox->hide();
		gui.get<tgui::Panel>("Chat")->add(privChats[create].chatBox, name);
		int tmp = gui.get<tgui::Tab>("ChatTab", 1)->getSelectedIndex();
		gui.get<tgui::Tab>("ChatTab", 1)->add(name);
		gui.get<tgui::Tab>("ChatTab", 1)->select(tmp);
	}
	privChats[create].chatBox->addLine(name + ": " + msg);
}

void UI::scoreRow() {
	sf::Uint8 combo, rank, position;
	sf::Uint16 id, sent, received, blocked, bpm, spm, score;
	float adj;

	net.packet >> id >> combo >> sent >> received >> blocked >> bpm >> spm >> rank >> position >> score >> adj;

	sf::String rounding = to_string((int)adj); //A bit messy-looking way of rounding float to 1 decimal
	rounding += "." + to_string((int)((adj - (int)adj)*10));

	tgui::Label::Ptr label = themeTG->load("Label");
	label->setText(getName(id));
	label->setPosition(5, scoreRows*30+5);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(score));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(65, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(rank));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(110, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(bpm));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(155, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(combo));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(200, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(sent));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(240, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(rounding);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(275, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(spm));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(315, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	label = themeTG->load("Label");
	label->setText(to_string(blocked) + "/" + to_string(received));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(380, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	gui.get<tgui::Panel>("Score")->add(label);

	scoreRows++;
}

void UI::clearScore() {
	gui.get<tgui::Panel>("Score")->removeAllWidgets();

	tgui::Label::Ptr label = themeTG->load("Label");
	label->setPosition(5, 5);
	label->setText("Name                  Score  Rank  BPM  Cmb  Sent  Adj  SPM  Blocked");
	label->setTextSize(16);
	gui.get<tgui::Panel>("Score")->add(label);

	scoreRows=1;
}

void UI::igtabSelect(const std::string& tab) {
	if (tab == "GameFields") {
		gui.get("GameFields")->show();
		gui.get("Score")->hide();
		gui.get("Chat")->hide();
	}
	else if (tab == "Score") {
		gui.get("GameFields")->hide();
		gui.get("Score")->show();
		gui.get("Chat")->hide();
	}
	else if (tab == "Chat") {
		gui.get("GameFields")->hide();
		gui.get("Score")->hide();
		gui.get("Chat")->show();
	}
}

void UI::Chat() {
	if (gui.get("Chat")->isVisible()) {
		gui.get("Chat")->hide();
		gui.get("GameFields")->show();
		gui.get<tgui::Tab>("InGameTab")->select(0);
	}
	else {
		gui.get("Chat")->show();
		gui.get("GameFields")->hide();
		gui.get("Score")->hide();
		gui.get<tgui::Tab>("InGameTab")->select(2);
		gui.get("ChatBox", 1)->focus();
	}
}

void UI::Score() {
	if (gui.get("Score")->isVisible()) {
		gui.get("Score")->hide();
		gui.get("GameFields")->show();
		gui.get<tgui::Tab>("InGameTab")->select(0);
	}
	else if (!chatFocused) {
		gui.get("Chat")->hide();
		gui.get("GameFields")->hide();
		gui.get("Score")->show();
		gui.get<tgui::Tab>("InGameTab")->select(1);
	}
}

void UI::otabSelect(std::string tab) {
	if (tab == "General") {
		gui.get("GenOpt")->show();
		gui.get("VidOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=true;
	}
	else if (tab == "Video") {
		gui.get("VidOpt")->show();
		gui.get("GenOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=false;
		if (options.fullscreen)
			gui.get<tgui::CheckBox>("Fullscreen", 1)->check();
		else
			gui.get<tgui::CheckBox>("Fullscreen", 1)->uncheck();
		if (options.vSync)
			gui.get<tgui::CheckBox>("vSync", 1)->check();
		else
			gui.get<tgui::CheckBox>("vSync", 1)->uncheck();
		gui.get<tgui::EditBox>("FrameDelay", 1)->setText(to_string(1000000/options.frameDelay.asMicroseconds()));
		gui.get<tgui::EditBox>("InputDelay", 1)->setText(to_string(1000000/options.inputDelay.asMicroseconds()));
	}
	else if (tab == "Sound") {
		gui.get("VidOpt")->hide();
		gui.get("GenOpt")->hide();
		gui.get("SndOpt")->show();
		adjPieces=false;
	}
	else if (tab == "Back") {
		gui.get("MainMenu")->show();
		gui.get("OptTab")->hide();
		gui.get("GenOpt")->hide();
		gui.get("VidOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=false;
	}
}

void UI::volSlide(short i, short vol) {
	if (i == 1) {
		options.MusicVolume = vol;
		resources.sounds.setMusicVolume(vol);
	}
	else if (i == 2) {
		options.EffectVolume = vol;
		resources.sounds.setEffectVolume(vol);
	}
	else if (i == 3) {
		options.ChatVolume = vol;
		resources.sounds.setChatVolume(vol);
	}
}

void UI::sndChecked(bool i) {
	if (i)
		options.sound = true;
	else
		options.sound = false;
}

void UI::fsChecked(bool i) {
	if (i)
		gui.get("VMSlider", 1)->enable();
	else
		gui.get("VMSlider", 1)->disable();
}

void UI::applyVideo() {
	if (gui.get<tgui::CheckBox>("Fullscreen", 1)->isChecked()) {
		if (options.currentmode == -1 || options.currentmode != gui.get<tgui::Slider>("VMSlider", 1)->getValue()) {
			options.currentmode = gui.get<tgui::Slider>("VMSlider", 1)->getValue();
			window->close();
			window->create(options.modes[options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
			window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
			options.fullscreen=true;
		}
	}
	else if (options.currentmode != -1) {
		window->close();
		window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		options.currentmode = -1;
		window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		options.fullscreen=false;
	}

	if (gui.get<tgui::CheckBox>("vSync", 1)->isChecked()) {
		options.vSync = true;
		window->setVerticalSyncEnabled(true);
	}
	else {
		options.vSync = false;
		window->setVerticalSyncEnabled(false);
	}

	if (gui.get<tgui::CheckBox>("performanceOutput", 1)->isChecked())
		options.performanceOutput = true;
	else
		options.performanceOutput = false;

	std::string fd = gui.get<tgui::EditBox>("FrameDelay", 1)->getText();
	short value=0;
	if (fd.size())
		value = stoi(fd);
	if (value)
		options.frameDelay = sf::microseconds(1000000/value);
	value=0;
	fd = gui.get<tgui::EditBox>("InputDelay", 1)->getText();
	if (fd.size())
		value = stoi(fd);
	if (value)
		options.inputDelay = sf::microseconds(1000000/value);
}

void UI::vidSlide(short i) {
	sf::String name;
	name = to_string(options.modes[i].width) + "x" + to_string(options.modes[i].height);
	gui.get<tgui::Label>("VideoMode", 1)->setText(name);
}

void UI::setBool(bool& var) {
	var=true;
}

void UI::quitGame() {
	window->close();
}

void UI::Options() {
	gui.get("MainMenu")->hide();
	gui.get("OptTab")->show();
	gui.get("GenOpt")->show();
	gui.get<tgui::Tab>("OptTab")->select(0);
	adjPieces=true;
}

void UI::mainMenu() {
	gui.get("InGameTab")->hide();
	gui.get("Chat")->hide();
	gui.get("Score")->hide();
	gui.get("GameFields")->hide();
	gui.get("MainMenu")->show();
}

void UI::delayCheck() {
	sf::Time currentTime = delayClock.getElapsedTime();
	if (gui.get("QuickMsg")->isVisible())
		if (currentTime - quickMsgTime > sf::seconds(5))
			gui.get("QuickMsg")->hide();

	if (playonline) {
		if (!udpConfirmed)
			if (currentTime - udpPortTime > sf::milliseconds(500)) {
				udpPortTime = currentTime;
				sendPacket99();
			}
		if (currentTime - pingTime > sf::seconds(1)) {
			if (!pingReturned) {
				ping.setString("999");
				pingColor=0;
			}
			pingTime = currentTime;
			pingIdCount++;
			sendPacket102(pingIdCount);
		}
	}

	sf::Time tmp = currentTime - spamTime;
	spamCount-=tmp.asMilliseconds();
	spamTime = currentTime;
	if (spamCount<0)
		spamCount=0;
}

void UI::setKey(tgui::Button::Ptr butt, sf::Keyboard::Key& skey) {
	gui.get("SelectKey", true)->show();
	gui.get("GenOpt", true)->disable();
	setkey=true;
	key=&skey;
	button=butt;
}

void UI::putKey(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == -1 ) {
        	gui.get("SelectKey", true)->hide();
        	gui.get("GenOpt", true)->enable();
        	setkey=false;
        }
        else {
        	gui.get("SelectKey", true)->hide();
        	gui.get("GenOpt", true)->enable();
        	setkey=false;

		if (event.key.code == options.left) {
			options.left = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindLeft", true)->setText("");
		}

		if (event.key.code == options.right) {
			options.right = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRight", true)->setText("");
		}

		if (event.key.code == options.down) {
			options.down = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindDown", true)->setText("");
		}

		if (event.key.code == options.rcw) {
			options.rcw = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRCW", true)->setText("");
		}

		if (event.key.code == options.rccw) {
			options.rccw = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRCCW", true)->setText("");
		}

		if (event.key.code == options.r180) {
			options.r180 = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindR180", true)->setText("");
		}

		if (event.key.code == options.hd) {
			options.hd = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindHD", true)->setText("");
		}

		if (event.key.code == options.chat) {
			options.chat = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindChat", true)->setText("");
		}

		if (event.key.code == options.score) {
			options.score = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindScore", true)->setText("");
		}

		if (event.key.code == options.away) {
			options.away = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindAway", true)->setText("");
		}



        	*key = event.key.code;
        	button->setText(SFKeyToString(event.key.code));
        }
    }
}

void UI::changeName(const sf::String& name) {
	options.name = name;
	game.field.setName(name);
}

void UI::setFrameRate(int fr) {
	frameRate.setString(to_string(fr));
    if (fr < 40)
        frameRateColor = 0;
    else if (fr < 100)
        frameRateColor = (fr-40)*4.25;
    else
    	frameRateColor = 255;
}

void UI::setInputRate(int fc) {
	inputRate.setString(to_string(fc));
    if (fc > 999) {
        inputRate.setCharacterSize(9);
        inputRate.setPosition(872,14);
    }
    else {
        inputRate.setCharacterSize(12);
        inputRate.setPosition(872,12);
    }
    if (fc < 255)
        inputRateColor = fc;
    else
    	inputRateColor = 255;
}

void UI::setLongestFrame(sf::Time& lf) {
	longestFrame.setString(to_string(lf.asMilliseconds()));
    if (lf.asMilliseconds() > 21)
        longestFrameColor = 0;
    else if (lf.asMilliseconds() > 4)
        longestFrameColor = 255 - (lf.asMilliseconds() - 4) * 15;
    else
    	longestFrameColor = 255;
}

void UI::setPing(sf::Time& pingResult) {
	ping.setString(to_string(pingResult.asMilliseconds()));
	if (pingResult.asMilliseconds() > 255)
		pingColor = 0;
	else
		pingColor = 255-pingResult.asMilliseconds();
}

void UI::drawPerformanceOutput() {
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(28,15));
	rect.setOutlineColor(sf::Color(255,255,255));
	rect.setFillColor(sf::Color(255-pingColor, pingColor, 0));
	rect.setPosition(930, 13);
	window->draw(rect);
	window->draw(ping);

	rect.setFillColor(sf::Color(255-frameRateColor, frameRateColor, 0));
	rect.setPosition(900, 13);
	window->draw(rect);
	window->draw(frameRate);

	rect.setFillColor(sf::Color(255-inputRateColor, inputRateColor, 0));
	rect.setPosition(870, 13);
	window->draw(rect);
	window->draw(inputRate);

	rect.setSize(sf::Vector2f(21,15));
	rect.setFillColor(sf::Color(255-longestFrameColor, longestFrameColor, 0));
	rect.setPosition(847, 13);
	window->draw(rect);
	window->draw(longestFrame);

	window->draw(performanceLabel);
}

void UI::rotPiece(short i) {
	options.piecerotation[i]++;
	if (options.piecerotation[i]>3)
		options.piecerotation[i]=0;
	piece[i].setRotation(options.piecerotation[i]*90);
	game.updateBasePieces();
}

void UI::colPiece(short i) {
	if (options.basepiece[i].tile+1>7)
        options.setPieceColor(i, 1);
    else
        options.setPieceColor(i, options.basepiece[i].tile+1);
	piece[i].setColor(pColor(options.basepiece[i].tile));
	game.updateBasePieces();
}

sf::Color pColor(short i) {
	sf::Color col;
	switch (i) {
		case 1: col.r=255; col.g=0; col.b=0; break;
		case 2: col.r=0; col.g=255; col.b=0; break;
		case 3: col.r=115; col.g=145; col.b=255; break;
		case 4: col.r=255; col.g=0; col.b=255; break;
		case 5: col.r=0; col.g=255; col.b=255; break;
		case 6: col.r=255; col.g=255; col.b=0; break;
		case 7: col.r=255; col.g=255; col.b=255; break;

		default: col.r=0; col.g=0; col.b=0; break;
	}
	return col;
}

void UI::initSprites() {
	sf::Texture tileT;
	sf::Sprite tile;
	tileT.loadFromFile(resourcePath() + "media/tile.png");
	tile.setTexture(tileT);

	sf::RenderTexture rendtex;
	rendtex.create(120,120);

	std::vector<short> value = options.pieceArray();

	for (int p=0; p<7; p++) {
		rendtex.clear(sf::Color(255,255,255,0));
		if (p==4 || p==6) {
			for (int x=0; x<4; x++)
				for (int y=0; y<4; y++)
					if (value[16*p + 4*y + x]) {
						tile.setPosition(x*30, y*30);
						rendtex.draw(tile);
					}
			rendtex.display();
			texture[p] = rendtex.getTexture();
			piece[p].setTexture(texture[p]);
			piece[p].setScale(0.8, 0.8);
			piece[p].setPosition(115*p+50, 360+50);
			piece[p].setColor(pColor(options.basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options.piecerotation[p]*90);
		}
		else {
			for (int x=0; x<3; x++)
				for (int y=0; y<3; y++)
					if (value[16*p + 4*y + x]) {
						tile.setPosition(x*30+15, y*30+15);
						rendtex.draw(tile);
					}
			rendtex.display();
			texture[p] = rendtex.getTexture();
			piece[p].setTexture(texture[p]);
			piece[p].setScale(0.8, 0.8);
			piece[p].setPosition(115*p+50, 360+50);
			piece[p].setColor(pColor(options.basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options.piecerotation[p]*90);
		}
	}
}

sf::String SFKeyToString(unsigned int keycode) {
    switch (keycode) {
    case sf::Keyboard::Key::Escape: return "Escape"; break;
    case sf::Keyboard::Key::LControl: return "LControl"; break;
    case sf::Keyboard::Key::LShift: return "LShift"; break;
    case sf::Keyboard::Key::LAlt: return "LAlt"; break;
    case sf::Keyboard::Key::LSystem: return "LSystem"; break;
    case sf::Keyboard::Key::RControl: return "RControl"; break;
    case sf::Keyboard::Key::RShift: return "RShift"; break;
    case sf::Keyboard::Key::RAlt: return "RAlt"; break;
    case sf::Keyboard::Key::RSystem: return "RSystem"; break;
    case sf::Keyboard::Key::Menu: return "Menu"; break;
    case sf::Keyboard::Key::LBracket: return "LBracket"; break;
    case sf::Keyboard::Key::RBracket: return "RBracket"; break;
    case sf::Keyboard::Key::SemiColon: return ";"; break;
    case sf::Keyboard::Key::Comma: return ","; break;
    case sf::Keyboard::Key::Period: return "."; break;
    case sf::Keyboard::Key::Quote: return "\'"; break;
    case sf::Keyboard::Key::Slash: return "/"; break;
    case sf::Keyboard::Key::BackSlash: return "\\"; break;
    case sf::Keyboard::Key::Tilde: return "~"; break;
    case sf::Keyboard::Key::Equal: return "="; break;
    case sf::Keyboard::Key::Dash: return "-"; break;
    case sf::Keyboard::Key::Space: return "Space"; break;
    case sf::Keyboard::Key::Return: return "Return"; break;
    case sf::Keyboard::Key::Tab: return "Tab"; break;
    case sf::Keyboard::Key::PageUp: return "Page Up"; break;
    case sf::Keyboard::Key::PageDown: return "Page Down"; break;
    case sf::Keyboard::Key::End: return "End"; break;
    case sf::Keyboard::Key::Home: return "Home"; break;
    case sf::Keyboard::Key::Insert: return "Insert"; break;
    case sf::Keyboard::Key::Delete: return "Delete"; break;
    case sf::Keyboard::Key::Add: return "+"; break;
    case sf::Keyboard::Key::Subtract: return "-"; break;
    case sf::Keyboard::Key::Multiply: return "*"; break;
    case sf::Keyboard::Key::Divide: return "/"; break;
    case sf::Keyboard::Key::Left: return "Left"; break;
    case sf::Keyboard::Key::Right: return "Right"; break;
    case sf::Keyboard::Key::Up: return "Up"; break;
    case sf::Keyboard::Key::Down: return "Down"; break;
    case sf::Keyboard::Key::BackSpace: return "BackSpace"; break;
    case sf::Keyboard::Key::Numpad0: return "NP 0"; break;
    case sf::Keyboard::Key::Numpad1: return "NP 1"; break;
    case sf::Keyboard::Key::Numpad2: return "NP 2"; break;
    case sf::Keyboard::Key::Numpad3: return "NP 3"; break;
    case sf::Keyboard::Key::Numpad4: return "NP 4"; break;
    case sf::Keyboard::Key::Numpad5: return "NP 5"; break;
    case sf::Keyboard::Key::Numpad6: return "NP 6"; break;
    case sf::Keyboard::Key::Numpad7: return "NP 7"; break;
    case sf::Keyboard::Key::Numpad8: return "NP 8"; break;
    case sf::Keyboard::Key::Numpad9: return "NP 9"; break;
    case sf::Keyboard::Key::Num1: return "1"; break;
    case sf::Keyboard::Key::Num2: return "2"; break;
    case sf::Keyboard::Key::Num3: return "3"; break;
    case sf::Keyboard::Key::Num4: return "4"; break;
    case sf::Keyboard::Key::Num5: return "5"; break;
    case sf::Keyboard::Key::Num6: return "6"; break;
    case sf::Keyboard::Key::Num7: return "7"; break;
    case sf::Keyboard::Key::Num8: return "8"; break;
    case sf::Keyboard::Key::Num9: return "9"; break;
    case sf::Keyboard::Key::Num0: return "0"; break;
    case sf::Keyboard::Key::F1: return "F1"; break;
    case sf::Keyboard::Key::F2: return "F2"; break;
    case sf::Keyboard::Key::F3: return "F3"; break;
    case sf::Keyboard::Key::F4: return "F4"; break;
    case sf::Keyboard::Key::F5: return "F5"; break;
    case sf::Keyboard::Key::F6: return "F6"; break;
    case sf::Keyboard::Key::F7: return "F7"; break;
    case sf::Keyboard::Key::F8: return "F8"; break;
    case sf::Keyboard::Key::F9: return "F9"; break;
    case sf::Keyboard::Key::F10: return "F10"; break;
    case sf::Keyboard::Key::F11: return "F11"; break;
    case sf::Keyboard::Key::F12: return "F12"; break;
    case sf::Keyboard::Key::F13: return "F13"; break;
    case sf::Keyboard::Key::F14: return "F14"; break;
    case sf::Keyboard::Key::F15: return "F15"; break;
    case sf::Keyboard::Key::Pause: return "Pause"; break;

    default:
    if (keycode < 26)
        return (char)(keycode+65);
    else
    	return "";
    }
}
