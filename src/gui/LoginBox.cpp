#include "LoginBox.h"
#include "machineid.h"
#include "Options.h"
#include "network.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& SetName = Signal<void, const std::string&>::get("SetName");
static auto& Disconnect = Signal<void, int>::get("Disconnect");
static auto& SetRoomListTime = Signal<void>::get("SetRoomListTime");
static auto& SendPacket = Signal<void, sf::Packet&>::get("SendPacket");

LoginBox::LoginBox(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) :
GuiBase(_pos, _res, parent), connectingScreen(sf::Rect<int>(0,0,960,600), resources) {

	tgui::Label::Ptr header = resources.gfx->load("Label");
	header->setTextSize(42);
	header->setText("Play online");
	header->setPosition(20, 0);
	header->setFont(resources.gfx->font("typewriter"));
	panel->add(header);

	tgui::Label::Ptr LiL1 = resources.gfx->load("Label");
	LiL1->setText("Username");
	LiL1->setPosition(20, 83);
	panel->add(LiL1);

	tgui::Label::Ptr LiL2 = resources.gfx->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(23, 123);
	panel->add(LiL2);

	LiE1 = resources.gfx->load("EditBox");
	LiE1->setPosition(120, 80);
	LiE1->setSize(180, 30);
	LiE1->setText(Options::get<std::string>("username"));
	panel->add(LiE1);

	LiE2 = resources.gfx->load("EditBox");
	LiE2->setPosition(120, 120);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	if (Options::get<bool>("rememberme")) {
		std::string boguspass;
		for (int i=0; i<Options::get<short>("pass"); i++)
			boguspass+="b";
		LiE2->setText(boguspass);
	}
	LiE2->connect("TextChanged", [&](){ edited=true; });
	LiE2->connect("Focused", [&]() { if (!edited) LiE2->setText(""); });
	LiE1->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	LiE2->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiE2);

	tgui::CheckBox::Ptr remember = resources.gfx->load("CheckBox");
	remember->setText("Remember me");
	remember->setPosition(120, 160);
	if (Options::get<bool>("rememberme"))
		remember->check();
	remember->connect("Checked", [&](){ Options::get<bool>("rememberme")=true; });
	remember->connect("Unchecked", [&](){ Options::get<bool>("rememberme")=false; });
	panel->add(remember);

	tgui::Button::Ptr LiB1 = resources.gfx->load("Button");
	LiB1->setPosition(160, 200);
	LiB1->setSize(100, 40);
	LiB1->setText("Login");
	LiB1->connect("pressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiB1);

	tgui::Button::Ptr regButton = resources.gfx->load("Button");
	regButton->setPosition(20, 220);
	regButton->setSize(100, 30);
	regButton->setText("Register");
	regButton->connect("pressed", &LoginBox::regPressed, this);
	panel->add(regButton);

	tgui::Button::Ptr forgotButton = resources.gfx->load("Button");
	forgotButton->setPosition(20, 260);
	forgotButton->setSize(160, 30);
	forgotButton->setText("Forgot password");
	forgotButton->connect("pressed", &LoginBox::forgotPressed, this);
	panel->add(forgotButton);

	tgui::Label::Ptr LiL4 = resources.gfx->load("Label");
	LiL4->setText("Play as guest");
	LiL4->setPosition(145, 330);
	panel->add(LiL4);

	tgui::Label::Ptr LiL3 = resources.gfx->load("Label");
	LiL3->setText("Name");
	LiL3->setPosition(53, 363);
	panel->add(LiL3);

	LiE3 = resources.gfx->load("EditBox");
	LiE3->setPosition(120, 360);
	LiE3->setSize(180, 30);
	LiE3->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiE3);

	tgui::Button::Ptr LiB3 = resources.gfx->load("Button");
	LiB3->setPosition(135, 410);
	LiB3->setSize(150, 40);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiB3);

	connectSignal("IsLoginThreadJoinable", [&](){ return t.joinable(); });
	connectSignal("TellPatcherToQuit", [&](){ patcher.quit=true; });
}

void LoginBox::launchLogin(uint8_t guest) {
	if (guest && !LiE3->getText().getSize())
		return;
	Hide(0);
	connectingScreen.show();
	connectingScreen.label->setText("Connecting to server...");
	if (t.joinable())
		t.join();
	if (guest)
		t = std::thread(&LoginBox::login, this, LiE3->getText(), LiE2->getText(), guest);
	else
		t = std::thread(&LoginBox::login, this, LiE1->getText(), LiE2->getText(), guest);
}

void LoginBox::login(std::string name, std::string pass, uint8_t guest) {
	patcher.status=1;
	if (resources.net->connect() == sf::Socket::Done) {
		std::string hash;
		if (!guest) {
			patcher.status=2;
			if (!edited && Options::get<bool>("rememberme"))
				hash = resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&remember=" + Options::get<std::string>("hash") + "&machineid=" + machineid::machineHash(), 1);
			else
				hash = resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&pass=" + pass + "&machineid=" + machineid::machineHash(), 1);
			if (Options::get<bool>("rememberme")) {
				if (hash.size() >= 40)
					Options::get<std::string>("hash") = hash.substr(20);
			}
			else
				Options::get<std::string>("hash") = "null";
			patcher.status=3;
			hash = hash.substr(0,20);
			Options::get<std::string>("username")=name;
			Options::get<short>("pass") = pass.size();
			sendLogin(hash, guest);
		}
		else {
			patcher.status=3;
			sendLogin(name, guest);
			SetName(name);
			resources.name = name;
		}
		resources.playonline=true;
		SetRoomListTime();
		Show(11);
	}
	else {
		Disconnect(2);
		patcher.status=-1;
	}
}

void LoginBox::checkStatus() {
	if (patcher.status == 2) {
		connectingScreen.label->setText("Doing secure auth with auth-server...");
		connectingScreen.label->setPosition(130, 290);
		connectingScreen.changelog->hide();
		connectingScreen.cancel->hide();
		connectingScreen.apply->hide();
		connectingScreen.apply->disable();
		connectingScreen.downloadProgress->hide();
		connectingScreen.downloadProgress->setValue(0);
		connectingScreen.progressLabel->hide();
	}
	else if (patcher.status == 3)
		connectingScreen.label->setText("Waiting for auth-response from game-server...");
	else if (patcher.status == -1) {
		t.join();
		QuickMsg("Could not connect to server");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == 4) {
		t.join();
		QuickMsg("No new version found");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == 5) {
		connectingScreen.label->setText("New patch found, " + std::to_string(patcher.files_downloaded) + " of " + std::to_string(patcher.files_total) + " files downloaded");
		connectingScreen.label->setPosition(130, 40);
		connectingScreen.setDownloadProgress(patcher.filesize, patcher.downloaded);
		connectingScreen.changelog->setText(patcher.changelog);
		connectingScreen.changelog->show();
		connectingScreen.cancel->show();
		connectingScreen.downloadProgress->show();
		connectingScreen.progressLabel->show();
	}
	else if (patcher.status == 6) {
		t.join();
		connectingScreen.label->setText("Patch is ready!");
		connectingScreen.apply->show();
		connectingScreen.apply->enable();
	}
	else if (patcher.status == -2) {
		t.join();
		QuickMsg("Error talking to patch server");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == -3) {
		t.join();
		QuickMsg("Error downloading file");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == -4) {
		t.join();
		QuickMsg("The md5-sum for downloaded file did not match, aborting");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == -5) {
		t.join();
		QuickMsg("Error saving file");
		connectingScreen.hide();
		Show(0);
	}
	else if (patcher.status == -6) {
		t.join();
		connectingScreen.hide();
		Show(0);
	}
	patcher.status=0;
}

void LoginBox::show() {
	panel->show();
	LiE1->focus();
}

void LoginBox::sendLogin(const std::string& hashorname, uint8_t guest) {
	sf::Packet packet;
	packet << (uint8_t)2 << resources.clientVersion << guest << hashorname;
	SendPacket(packet);
}

void LoginBox::regPressed() {
	#ifdef _WIN32
		system("start https://speedblocks.se/forum/ucp.php?mode=register");
	#elif __APPLE__
		system("open https://speedblocks.se/forum/ucp.php?mode=register");
	#else
		system("xdg-open https://speedblocks.se/forum/ucp.php?mode=register");
	#endif
}

void LoginBox::forgotPressed() {
	#ifdef _WIN32
		system("start https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#elif __APPLE__
		system("open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#else
		system("xdg-open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#endif
}
