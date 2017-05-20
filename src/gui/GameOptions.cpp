#include "GameOptions.h"
#include "gui.h"
#include "gamePlay.h"
#include "optionSet.h"
using std::to_string;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

void GameOptions::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);
	initSprites();

	otab = gui->themeBB->load("Tab"); // General Options
	otab->setTextSize(32);
	otab->setPosition(250, 30);
	otab->add("General");
	otab->add("Video");
	otab->add("Sound");
	otab->add("Back");
	otab->select(1);
	otab->connect("TabSelected", &GameOptions::otabSelect, this);
	otab->setOpacity(0.7);
	panel->add(otab);

	GenOpt = tgui::Panel::create();
	GenOpt->setSize(960, 500);
	GenOpt->setPosition(0, 100);
	GenOpt->setBackgroundColor(sf::Color(255,255,255,0));
	GenOpt->hide();
	panel->add(GenOpt);

	tgui::Label::Ptr NL = gui->themeTG->load("Label");
	NL->setPosition(80, 13);
	NL->setText("Name");
	GenOpt->add(NL);
	tgui::EditBox::Ptr NTB = gui->themeTG->load("EditBox");
	NTB->setText(gui->options.name);
	NTB->setPosition(150, 10);
	NTB->setSize(400, 30);
	NTB->connect("TextChanged", &GameOptions::changeName, this);
	GenOpt->add(NTB);

	SelectKey = gui->themeTG->load("Panel");
	SelectKey->setSize(300, 100);
	SelectKey->setPosition(250, 150);
	SelectKey->hide();
	GenOpt->add(SelectKey);
	tgui::Label::Ptr CKPL = gui->themeTG->load("Label");
	CKPL->setTextSize(32);
	CKPL->setText("Press any key");
	CKPL->setPosition(40, 30);
	SelectKey->add(CKPL);

	tgui::Label::Ptr LeL = gui->themeTG->load("Label"); // Binding Keys
	LeL->setPosition(0, 63);
	LeL->setSize(90, 30);
	LeL->setText("Left");
	LeL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(LeL);
	BindLeft = gui->themeTG->load("Button");
	BindLeft->setPosition(100, 60);
	BindLeft->connect("pressed", &GameOptions::setKey, this, BindLeft, std::ref(gui->options.left));
	BindLeft->setText(SFKeyToString(gui->options.left));
	GenOpt->add(BindLeft);

	tgui::Label::Ptr RiL = gui->themeTG->load("Label");
	RiL->setPosition(0, 103);
	RiL->setSize(90, 30);
	RiL->setText("Right");
	RiL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(RiL);
	BindRight = gui->themeTG->load("Button");
	BindRight->setPosition(100, 100);
	BindRight->connect("pressed", &GameOptions::setKey, this, BindRight, std::ref(gui->options.right));
	BindRight->setText(SFKeyToString(gui->options.right));
	GenOpt->add(BindRight);

	tgui::Label::Ptr DoL = gui->themeTG->load("Label");
	DoL->setPosition(0, 143);
	DoL->setSize(90, 30);
	DoL->setText("Down");
	DoL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(DoL);
	BindDown = gui->themeTG->load("Button");
	BindDown->setPosition(100, 140);
	BindDown->connect("pressed", &GameOptions::setKey, this, BindDown, std::ref(gui->options.down));
	BindDown->setText(SFKeyToString(gui->options.down));
	GenOpt-> add(BindDown);

	tgui::Label::Ptr CwL = gui->themeTG->load("Label");
	CwL->setPosition(0, 223);
	CwL->setSize(90, 30);
	CwL->setText("Rotate >");
	CwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CwL);
	BindRCW = gui->themeTG->load("Button");
	BindRCW->setPosition(100, 220);
	BindRCW->connect("pressed", &GameOptions::setKey, this, BindRCW, std::ref(gui->options.rcw));
	BindRCW->setText(SFKeyToString(gui->options.rcw));
	GenOpt->add(BindRCW);

	tgui::Label::Ptr CcL = gui->themeTG->load("Label");
	CcL->setPosition(0, 183);
	CcL->setSize(90, 30);
	CcL->setText("Rotate <");
	CcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CcL);
	BindRCCW = gui->themeTG->load("Button");
	BindRCCW->setPosition(100, 180);
	BindRCCW->connect("pressed", &GameOptions::setKey, this, BindRCCW, std::ref(gui->options.rccw));
	BindRCCW->setText(SFKeyToString(gui->options.rccw));
	GenOpt->add(BindRCCW);

	tgui::Label::Ptr R1L = gui->themeTG->load("Label");
	R1L->setPosition(350, 63);
	R1L->setSize(140, 30);
	R1L->setText("Rotate 180");
	R1L->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(R1L);
	BindR180 = gui->themeTG->load("Button");
	BindR180->setPosition(500, 60);
	BindR180->connect("pressed", &GameOptions::setKey, this, BindR180, std::ref(gui->options.r180));
	BindR180->setText(SFKeyToString(gui->options.r180));
	GenOpt->add(BindR180);

	tgui::Label::Ptr HdL = gui->themeTG->load("Label");
	HdL->setPosition(350, 103);
	HdL->setSize(140, 30);
	HdL->setText("Hard Drop");
	HdL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(HdL);
	BindHD = gui->themeTG->load("Button");
	BindHD->setPosition(500, 100);
	BindHD->connect("pressed", &GameOptions::setKey, this, BindHD, std::ref(gui->options.hd));
	BindHD->setText(SFKeyToString(gui->options.hd));
	GenOpt->add(BindHD);

	tgui::Label::Ptr ChL = gui->themeTG->load("Label");
	ChL->setPosition(400, 143);
	ChL->setSize(90, 30);
	ChL->setText("Chat");
	ChL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ChL);
	BindChat = gui->themeTG->load("Button");
	BindChat->setPosition(500, 140);
	BindChat->connect("pressed", &GameOptions::setKey, this, BindChat, std::ref(gui->options.chat));
	BindChat->setText(SFKeyToString(gui->options.chat));
	GenOpt->add(BindChat);

	tgui::Label::Ptr ScL = gui->themeTG->load("Label");
	ScL->setPosition(400, 183);
	ScL->setSize(90, 30);
	ScL->setText("Score");
	ScL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ScL);
	BindScore = gui->themeTG->load("Button");
	BindScore->setPosition(500, 180);
	BindScore->connect("pressed", &GameOptions::setKey, this, BindScore, std::ref(gui->options.score));
	BindScore->setText(SFKeyToString(gui->options.score));
	GenOpt->add(BindScore);

	tgui::Label::Ptr AwL = gui->themeTG->load("Label");
	AwL->setPosition(400, 223);
	AwL->setSize(90, 30);
	AwL->setText("Away");
	AwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(AwL);
	BindAway = gui->themeTG->load("Button");
	BindAway->setPosition(500, 220);
	BindAway->connect("pressed", &GameOptions::setKey, this, BindAway, std::ref(gui->options.away));
	BindAway->setText(SFKeyToString(gui->options.away));
	GenOpt->add(BindAway);

	tgui::Button::Ptr Rp[7]; // Align Pieces
	tgui::Button::Ptr Cc[7];
	for (int i=0; i<7; i++) {
		Rp[i] = gui->themeBB->load("Button");
		Cc[i] = gui->themeBB->load("Button");

		Rp[i]->setPosition(i*115+20, 365);
		Rp[i]->setSize(30,30);
		Rp[i]->setOpacity(0.7);
		Rp[i]->connect("pressed", &GameOptions::rotPiece, this, i);

		Cc[i]->setPosition(i*115+60, 365);
		Cc[i]->setSize(30,30);
		Cc[i]->connect("pressed", &GameOptions::colPiece, this, i);

		GenOpt->add(Rp[i]);
		GenOpt->add(Cc[i]);
	}

	tgui::Label::Ptr Rl1 = gui->themeTG->load("Label"); // Repeat delay & speed
	Rl1->setPosition(750, 0);
	Rl1->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl1->setText("Left & Right Repeat\nDelay         Speed");
	GenOpt->add(Rl1);

	tgui::EditBox::Ptr Re1 = gui->themeTG->load("EditBox");
	Re1->setPosition(750, 50);
	Re1->setSize(70, 30);
	Re1->setInputValidator(tgui::EditBox::Validator::UInt);
	Re1->setText(to_string(gui->options.repeatDelay.asMilliseconds()));
	Re1->connect("TextChanged", &optionSet::setDelay, gui->options, 1);
	GenOpt->add(Re1);

	tgui::EditBox::Ptr Re2 = gui->themeTG->load("EditBox");
	Re2->setPosition(850, 50);
	Re2->setSize(70, 30);
	Re2->setInputValidator(tgui::EditBox::Validator::UInt);
	Re2->setText(to_string(gui->options.repeatSpeed.asMilliseconds()));
	Re2->connect("TextChanged", &optionSet::setDelay, gui->options, 2);
	GenOpt->add(Re2);

	tgui::Label::Ptr Rl2 = gui->themeTG->load("Label");
	Rl2->setPosition(750, 100);
	Rl2->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl2->setText("Down Repeat\n  Delay         Speed");
	GenOpt->add(Rl2);

	tgui::EditBox::Ptr Re3 = gui->themeTG->load("EditBox");
	Re3->setPosition(750, 150);
	Re3->setSize(70, 30);
	Re3->setInputValidator(tgui::EditBox::Validator::UInt);
	Re3->setText(to_string(gui->options.repeatDelayDown.asMilliseconds()));
	Re3->connect("TextChanged", &optionSet::setDelay, gui->options, 3);
	GenOpt->add(Re3);

	tgui::EditBox::Ptr Re4 = gui->themeTG->load("EditBox");
	Re4->setPosition(850, 150);
	Re4->setSize(70, 30);
	Re4->setInputValidator(tgui::EditBox::Validator::UInt);
	Re4->setText(to_string(gui->options.repeatSpeedDown.asMilliseconds()));
	Re4->connect("TextChanged", &optionSet::setDelay, gui->options, 4);
	GenOpt->add(Re4);

	VidOpt = tgui::Panel::create(); // Video Options
	VidOpt->setSize(960, 500);
	VidOpt->setPosition(0, 100);
	VidOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VidOpt->hide();
	panel->add(VidOpt);

	tgui::Label::Ptr ViL = gui->themeTG->load("Label");
	ViL->setPosition(350, 0);
	ViL->setText("Video Mode");
	VidOpt->add(ViL);

	VMSlider = gui->themeTG->load("Slider");
	VMSlider->setPosition(50, 50);
	VMSlider->setSize(700, 30);
	VMSlider->setMaximum(gui->options.modes.size()-1);
	if (gui->options.currentmode == -1)
		VMSlider->setValue(0);
	else
		VMSlider->setValue(gui->options.currentmode);
	VMSlider->connect("ValueChanged", &GameOptions::vidSlide, this);
	VMSlider->disable();
	VidOpt->add(VMSlider);

	VideoMode = gui->themeTG->load("Label");
	VideoMode->setPosition(340, 90);
	sf::String cvmname;
	cvmname = to_string(gui->options.modes[VMSlider->getValue()].width) + "x" + to_string(gui->options.modes[VMSlider->getValue()].height);
	VideoMode->setText(cvmname);
	VidOpt->add(VideoMode);

	Fullscreen = gui->themeTG->load("CheckBox");
	Fullscreen->setPosition(120, 130);
	Fullscreen->setText("Fullscreen");
	Fullscreen->connect("Checked Unchecked", &GameOptions::fsChecked, this);
	VidOpt->add(Fullscreen);
	if (gui->options.fullscreen) {
		Fullscreen->check();
		VMSlider->enable();
	}

	vSync = gui->themeTG->load("CheckBox");
	vSync->setPosition(380, 130);
	vSync->setText("VSync");
	if (gui->options.vSync)
		vSync->check();
	VidOpt->add(vSync);

	performanceOutput = gui->themeTG->load("CheckBox");
	performanceOutput->setPosition(580, 130);
	performanceOutput->setText("Performance output");
	if (gui->options.performanceOutput)
		performanceOutput->check();
	VidOpt->add(performanceOutput);

	tgui::Label::Ptr FdL = gui->themeTG->load("Label");
	FdL->setPosition(150, 200);
	FdL->setText("Frame Rate");
	VidOpt->add(FdL);

	FrameDelay = gui->themeTG->load("EditBox");
	FrameDelay->setPosition(170, 230);
	FrameDelay->setSize(70, 30);
	FrameDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	FrameDelay->setText(to_string(1000000/gui->options.frameDelay.asMicroseconds()));
	VidOpt->add(FrameDelay);

	tgui::Label::Ptr IdL = gui->themeTG->load("Label");
	IdL->setPosition(470, 200);
	IdL->setText("Input Rate");
	VidOpt->add(IdL);

	InputDelay = gui->themeTG->load("EditBox");
	InputDelay->setPosition(460, 230);
	InputDelay->setSize(110, 30);
	InputDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	InputDelay->setText(to_string(1000000/gui->options.inputDelay.asMicroseconds()));
	VidOpt->add(InputDelay);

	tgui::Button::Ptr AvB = gui->themeBB->load("Button");
	AvB->setText("Apply");
	AvB->setPosition(340, 300);
	AvB->setOpacity(0.7);
	AvB->connect("pressed", &GameOptions::applyVideo, this);
	VidOpt->add(AvB);

	tgui::Label::Ptr InL = gui->themeTG->load("Label");
	InL->setPosition(10, 390);
	InL->setText("Frame Rate = frames drawn per second\nInput Rate = input parsed per second\nNote: Enabling vSync will disable both FrameRate and InputRate.");
	VidOpt->add(InL);

	SndOpt = tgui::Panel::create(); // Sound Options
	SndOpt->setSize(800, 500);
	SndOpt->setPosition(0, 100);
	SndOpt->setBackgroundColor(sf::Color(255,255,255,0));
	SndOpt->hide();
	panel->add(SndOpt);

	tgui::CheckBox::Ptr EsC = gui->themeTG->load("CheckBox");
	EsC->setPosition(150, 0);
	EsC->setText("Sound Enabled");
	EsC->connect("Checked Unchecked", &GameOptions::sndChecked, this);
	SndOpt->add(EsC, "sndCheck");
	if (gui->options.sound)
		EsC->check();

	tgui::Slider::Ptr MvS = gui->themeTG->load("Slider");
	MvS->setPosition(50, 100);
	MvS->setSize(700,30);
	MvS->setMaximum(100);
	MvS->connect("ValueChanged", &GameOptions::volSlide, this, 1);
	MvS->setValue(gui->options.MusicVolume);
	SndOpt->add(MvS);

	tgui::Label::Ptr MvL = gui->themeTG->load("Label");
	MvL->setText("Music Volume");
	MvL->setPosition(350, 50);
	SndOpt->add(MvL);

	tgui::Slider::Ptr EvS = gui->themeTG->load("Slider");
	EvS->setPosition(50, 200);
	EvS->setSize(700,30);
	EvS->setMaximum(100);
	EvS->connect("ValueChanged", &GameOptions::volSlide, this, 2);
	EvS->setValue(gui->options.EffectVolume);
	SndOpt->add(EvS);

	tgui::Label::Ptr EvL = gui->themeTG->load("Label");
	EvL->setText("Effect Volume");
	EvL->setPosition(350, 150);
	SndOpt->add(EvL);

	tgui::Slider::Ptr CvS = gui->themeTG->load("Slider");
	CvS->setPosition(50, 300);
	CvS->setSize(700,30);
	CvS->setMaximum(100);
	CvS->connect("ValueChanged", &GameOptions::volSlide, this, 3);
	CvS->setValue(gui->options.ChatVolume);
	SndOpt->add(CvS);

	tgui::Label::Ptr CvL = gui->themeTG->load("Label");
	CvL->setText("Chat Volume");
	CvL->setPosition(350, 250);
	SndOpt->add(CvL);
}

void GameOptions::otabSelect(std::string tab) {
	if (tab == "General") {
		GenOpt->show();
		VidOpt->hide();
		SndOpt->hide();
	}
	else if (tab == "Video") {
		VidOpt->show();
		GenOpt->hide();
		SndOpt->hide();
		if (gui->options.fullscreen)
			Fullscreen->check();
		else
			Fullscreen->uncheck();
		if (gui->options.vSync)
			vSync->check();
		else
			vSync->uncheck();
		FrameDelay->setText(to_string(1000000/gui->options.frameDelay.asMicroseconds()));
		InputDelay->setText(to_string(1000000/gui->options.inputDelay.asMicroseconds()));
	}
	else if (tab == "Sound") {
		VidOpt->hide();
		GenOpt->hide();
		SndOpt->show();
	}
	else if (tab == "Back") {
		gui->mainMenu.show();
		otab->select(1);
		hide();
	}
}

void GameOptions::changeName(const sf::String& name) {
	gui->options.name = name;
	gui->game.field.setName(name);
}

void GameOptions::vidSlide(short i) {
	sf::String name;
	name = to_string(gui->options.modes[i].width) + "x" + to_string(gui->options.modes[i].height);
	VideoMode->setText(name);
}

void GameOptions::fsChecked(bool i) {
	if (i)
		VMSlider->enable();
	else
		VMSlider->disable();
}

void GameOptions::sndChecked(bool i) {
	if (i)
		gui->options.sound = true;
	else
		gui->options.sound = false;
}

void GameOptions::applyVideo() {
	if (Fullscreen->isChecked()) {
		if (gui->options.currentmode == -1 || gui->options.currentmode != VMSlider->getValue()) {
			gui->options.currentmode = VMSlider->getValue();
			gui->window->close();
			gui->window->create(gui->options.modes[gui->options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
			gui->window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
			gui->options.fullscreen=true;
		}
	}
	else if (gui->options.currentmode != -1) {
		gui->window->close();
		gui->window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		gui->options.currentmode = -1;
		gui->window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		gui->options.fullscreen=false;
	}

	if (vSync->isChecked()) {
		gui->options.vSync = true;
		gui->window->setVerticalSyncEnabled(true);
	}
	else {
		gui->options.vSync = false;
		gui->window->setVerticalSyncEnabled(false);
	}

	if (performanceOutput->isChecked()) {
		gui->options.performanceOutput = true;
		gui->performanceOutput.show();
	}
	else {
		gui->options.performanceOutput = false;
		gui->performanceOutput.hide();
	}

	std::string fd = FrameDelay->getText();
	short value=0;
	if (fd.size())
		value = stoi(fd);
	if (value)
		gui->options.frameDelay = sf::microseconds(1000000/value);
	value=0;
	fd = InputDelay->getText();
	if (fd.size())
		value = stoi(fd);
	if (value)
		gui->options.inputDelay = sf::microseconds(1000000/value);
}

void GameOptions::volSlide(short i, short vol) {
	if (i == 1) {
		gui->options.MusicVolume = vol;
		gui->resources.sounds.setMusicVolume(vol);
	}
	else if (i == 2) {
		gui->options.EffectVolume = vol;
		gui->resources.sounds.setEffectVolume(vol);
	}
	else if (i == 3) {
		gui->options.ChatVolume = vol;
		gui->resources.sounds.setChatVolume(vol);
	}
}

void GameOptions::setKey(tgui::Button::Ptr _button, sf::Keyboard::Key& _key) {
	SelectKey->show();
	GenOpt->disable();
	key=&_key;
	button=_button;
}

void GameOptions::putKey(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == -1 ) {
        	SelectKey->hide();
        	GenOpt->enable();
        }
        else {
        	SelectKey->hide();
        	GenOpt->enable();

			if (event.key.code == gui->options.left) {
				gui->options.left = sf::Keyboard::Unknown;
				BindLeft->setText("");
			}

			if (event.key.code == gui->options.right) {
				gui->options.right = sf::Keyboard::Unknown;
				BindRight->setText("");
			}

			if (event.key.code == gui->options.down) {
				gui->options.down = sf::Keyboard::Unknown;
				BindDown->setText("");
			}

			if (event.key.code == gui->options.rcw) {
				gui->options.rcw = sf::Keyboard::Unknown;
				BindRCW->setText("");
			}

			if (event.key.code == gui->options.rccw) {
				gui->options.rccw = sf::Keyboard::Unknown;
				BindRCCW->setText("");
			}

			if (event.key.code == gui->options.r180) {
				gui->options.r180 = sf::Keyboard::Unknown;
				BindR180->setText("");
			}

			if (event.key.code == gui->options.hd) {
				gui->options.hd = sf::Keyboard::Unknown;
				BindHD->setText("");
			}

			if (event.key.code == gui->options.chat) {
				gui->options.chat = sf::Keyboard::Unknown;
				BindChat->setText("");
			}

			if (event.key.code == gui->options.score) {
				gui->options.score = sf::Keyboard::Unknown;
				BindScore->setText("");
			}

			if (event.key.code == gui->options.away) {
				gui->options.away = sf::Keyboard::Unknown;
				BindAway->setText("");
			}



        	*key = event.key.code;
        	button->setText(SFKeyToString(event.key.code));
        }
    }
}

void GameOptions::rotPiece(short i) {
	gui->options.piecerotation[i]++;
	if (gui->options.piecerotation[i]>3)
		gui->options.piecerotation[i]=0;
	piece[i].setRotation(gui->options.piecerotation[i]*90);
	gui->game.updateBasePieces();
}

void GameOptions::colPiece(short i) {
	if (gui->options.basepiece[i].tile+1>7)
        gui->options.setPieceColor(i, 1);
    else
        gui->options.setPieceColor(i, gui->options.basepiece[i].tile+1);
	piece[i].setColor(pColor(gui->options.basepiece[i].tile));
	gui->game.updateBasePieces();
}

void GameOptions::initSprites() {
	sf::Texture tileT;
	sf::Sprite tile;
	tileT.loadFromFile(resourcePath() + "media/tile.png");
	tile.setTexture(tileT);

	sf::RenderTexture rendtex;
	rendtex.create(120,120);

	std::vector<short> value = gui->options.pieceArray();

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
			piece[p].setColor(pColor(gui->options.basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(gui->options.piecerotation[p]*90);
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
			piece[p].setColor(pColor(gui->options.basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(gui->options.piecerotation[p]*90);
		}
	}
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