#include "gui.h"
#include "sounds.h"
#include "gamePlay.h"
#include "network.h"
#include <iostream>
#include "MingwConvert.h"
using namespace std;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

UI::UI(sf::RenderWindow& window_, sf::Font& font1, sf::Font& font2,
    optionSet& options_, soundBank& sounds_, gamePlay& game_, network& net_,
    textures& textureBase_)
    : typewriter(font1),
      printFont2(font2),
      printFont(&font2),
      gui(window_),
      options(&options_),
      sounds(&sounds_),
      game(&game_),
      net(&net_),
      textureBase(&textureBase_),
      window(&window_),
      training(false),
      playonline(false),
      setkey(false),
      adjPieces(false),
      updPieces(false),
      chatFocused(false),
      inroom(false),
      disconnect(false),
      away(false),
      key(nullptr),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      clientVersion(0),
      scoreRows(0),
      scaleup(0),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(0),
      gamestate(MainMenu) {

    compressor.game=game;

	themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");
	themeBB = tgui::Theme::create(resourcePath() + "media/BabyBlue.txt");

	initSprites();

	tgui::Panel::Ptr MainMenu = tgui::Panel::create();  // MAINMENU
	MainMenu->setSize(800, 600);
	MainMenu->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(MainMenu, "MainMenu");

	tgui::Button::Ptr PO = themeTG->load("Button");
	PO->setSize(480, 100);
	PO->setPosition(240, 100);
	PO->setText("Play Online");
	PO->setTextSize(72);
	PO->setFont(typewriter);
	PO->connect("pressed", &UI::playOnline, this);
	MainMenu->add(PO);

	tgui::Button::Ptr Tr = themeTG->load("Button");
	Tr->setSize(360, 100);
	Tr->setPosition(300, 220);
	Tr->setText("Training");
	Tr->setTextSize(72);
	Tr->setFont(typewriter);
	Tr->connect("pressed", &UI::Training, this);
	MainMenu->add(Tr);

	tgui::Button::Ptr Opt = themeTG->load("Button");
	Opt->setSize(310, 100);
	Opt->setPosition(325, 340);
	Opt->setText("Options");
	Opt->setTextSize(72);
	Opt->setFont(typewriter);
	Opt->connect("pressed", &UI::Options, this);
	MainMenu->add(Opt);

	tgui::Button::Ptr Quit = themeTG->load("Button");
	Quit->setSize(180, 100);
	Quit->setPosition(390, 460);
	Quit->setText("Quit");
	Quit->setTextSize(72);
	Quit->setFont(typewriter);
	Quit->connect("pressed", &UI::quitGame, this);
	MainMenu->add(Quit);

	tgui::EditBox::Ptr IPAddr = themeTG->load("EditBox");
	IPAddr->setPosition(10, 230);
	IPAddr->setSize(250, 40);
	IPAddr->setText(net->serverAdd.toString());
	MainMenu->add(IPAddr, "IPAddr");

	tgui::Panel::Ptr LiP = tgui::Panel::create(); // Login panel
	LiP->setSize(400, 300);
	LiP->setPosition(280, 150);
	LiP->setBackgroundColor(sf::Color(50,50,50,200));
	LiP->hide();
	gui.add(LiP, "Login");

	tgui::Label::Ptr LiL1 = themeTG->load("Label");
	LiL1->setText("Username/Email");
	LiL1->setPosition(20, 20);
	LiP->add(LiL1);

	tgui::Label::Ptr LiL2 = themeTG->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(220, 20);
	LiP->add(LiL2);

	tgui::EditBox::Ptr LiE1 = themeTG->load("EditBox");
	LiE1->setPosition(10, 60);
	LiE1->setSize(180, 30);
	LiP->add(LiE1, "Username");

	tgui::EditBox::Ptr LiE2 = themeTG->load("EditBox");
	LiE2->setPosition(210, 60);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	LiE2->connect("ReturnKeyPressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	LiP->add(LiE2);

	tgui::Button::Ptr LiB1 = themeBB->load("Button");
	LiB1->setPosition(50, 110);
	LiB1->setSize(100, 40);
	LiB1->setOpacity(0.7);
	LiB1->setText("Login");
	LiB1->connect("pressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	LiP->add(LiB1);

	tgui::Button::Ptr LiB2 = themeBB->load("Button");
	LiB2->setPosition(250, 110);
	LiB2->setSize(100, 40);
	LiB2->setOpacity(0.7);
	LiB2->setText("Cancel");
	LiB2->connect("Pressed", &UI::closeLogin, this);
	LiP->add(LiB2);

	tgui::Label::Ptr LiL3 = themeTG->load("Label");
	LiL3->setText("Guest name");
	LiL3->setPosition(145, 175);
	LiP->add(LiL3);

	tgui::EditBox::Ptr LiE3 = themeTG->load("EditBox");
	LiE3->setPosition(100, 210);
	LiE3->setSize(200, 30);
	LiE3->connect("ReturnKeyPressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	LiP->add(LiE3);

	tgui::Button::Ptr LiB3 = themeBB->load("Button");
	LiB3->setPosition(125, 250);
	LiB3->setSize(150, 40);
	LiB3->setOpacity(0.7);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	LiP->add(LiB3);

	tgui::Tab::Ptr otab = themeBB->load("Tab"); // General Options
	otab->setTextSize(32);
	otab->setPosition(250, 30);
	otab->add("General");
	otab->add("Video");
	otab->add("Sound");
	otab->add("Back");
	otab->select(0);
	otab->hide();
	otab->connect("TabSelected", &UI::otabSelect, this);
	otab->setOpacity(0.7);
	gui.add(otab, "OptTab");

	tgui::Panel::Ptr GenOpt = tgui::Panel::create();
	GenOpt->setSize(960, 500);
	GenOpt->setPosition(0, 100);
	GenOpt->setBackgroundColor(sf::Color(255,255,255,0));
	GenOpt->hide();
	gui.add(GenOpt, "GenOpt");

	tgui::Label::Ptr NL = themeTG->load("Label");
	NL->setPosition(80, 13);
	NL->setText("Name");
	GenOpt->add(NL);
	tgui::EditBox::Ptr NTB = themeTG->load("EditBox");
	NTB->setText(options->name);
	NTB->setPosition(150, 10);
	NTB->setSize(400, 30);
	NTB->connect("TextChanged", &UI::changeName, this);
	GenOpt->add(NTB);

	tgui::Panel::Ptr CKP = themeTG->load("Panel");
	CKP->setSize(300, 100);
	CKP->setPosition(250, 150);
	CKP->hide();
	GenOpt->add(CKP, "SelectKey");
	tgui::Label::Ptr CKPL = themeTG->load("Label");
	CKPL->setTextSize(32);
	CKPL->setText("Press any key");
	CKPL->setPosition(40, 30);
	CKP->add(CKPL);

	tgui::Label::Ptr LeL = themeTG->load("Label"); // Binding Keys
	LeL->setPosition(0, 63);
	LeL->setSize(90, 30);
	LeL->setText("Left");
	LeL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(LeL);
	tgui::Button::Ptr LeB = themeTG->load("Button");
	LeB->setPosition(100, 60);
	LeB->connect("pressed", &UI::setKey, this, LeB, std::ref(options->left));
	LeB->setText(SFKeyToString(options->left));
	GenOpt->add(LeB,"BindLeft");

	tgui::Label::Ptr RiL = themeTG->load("Label");
	RiL->setPosition(0, 103);
	RiL->setSize(90, 30);
	RiL->setText("Right");
	RiL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(RiL);
	tgui::Button::Ptr RiB = themeTG->load("Button");
	RiB->setPosition(100, 100);
	RiB->connect("pressed", &UI::setKey, this, RiB, std::ref(options->right));
	RiB->setText(SFKeyToString(options->right));
	GenOpt->add(RiB,"BindRight");

	tgui::Label::Ptr DoL = themeTG->load("Label");
	DoL->setPosition(0, 143);
	DoL->setSize(90, 30);
	DoL->setText("Down");
	DoL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(DoL);
	tgui::Button::Ptr DoB = themeTG->load("Button");
	DoB->setPosition(100, 140);
	DoB->connect("pressed", &UI::setKey, this, DoB, std::ref(options->down));
	DoB->setText(SFKeyToString(options->down));
	GenOpt-> add(DoB,"BindDown");

	tgui::Label::Ptr CwL = themeTG->load("Label");
	CwL->setPosition(0, 183);
	CwL->setSize(90, 30);
	CwL->setText("Rotate >");
	CwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CwL);
	tgui::Button::Ptr CwB = themeTG->load("Button");
	CwB->setPosition(100, 180);
	CwB->connect("pressed", &UI::setKey, this, CwB, std::ref(options->rcw));
	CwB->setText(SFKeyToString(options->rcw));
	GenOpt->add(CwB,"BindRCW");

	tgui::Label::Ptr CcL = themeTG->load("Label");
	CcL->setPosition(0, 223);
	CcL->setSize(90, 30);
	CcL->setText("Rotate <");
	CcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CcL);
	tgui::Button::Ptr CcB = themeTG->load("Button");
	CcB->setPosition(100, 220);
	CcB->connect("pressed", &UI::setKey, this, CcB, std::ref(options->rccw));
	CcB->setText(SFKeyToString(options->rccw));
	GenOpt->add(CcB,"BindRCCW");

	tgui::Label::Ptr R1L = themeTG->load("Label");
	R1L->setPosition(350, 63);
	R1L->setSize(140, 30);
	R1L->setText("Rotate 180");
	R1L->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(R1L);
	tgui::Button::Ptr R1B = themeTG->load("Button");
	R1B->setPosition(500, 60);
	R1B->connect("pressed", &UI::setKey, this, R1B, std::ref(options->r180));
	R1B->setText(SFKeyToString(options->r180));
	GenOpt->add(R1B,"BindR180");

	tgui::Label::Ptr HdL = themeTG->load("Label");
	HdL->setPosition(350, 103);
	HdL->setSize(140, 30);
	HdL->setText("Hard Drop");
	HdL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(HdL);
	tgui::Button::Ptr HdB = themeTG->load("Button");
	HdB->setPosition(500, 100);
	HdB->connect("pressed", &UI::setKey, this, HdB, std::ref(options->hd));
	HdB->setText(SFKeyToString(options->hd));
	GenOpt->add(HdB,"BindHD");

	tgui::Label::Ptr ChL = themeTG->load("Label");
	ChL->setPosition(400, 143);
	ChL->setSize(90, 30);
	ChL->setText("Chat");
	ChL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ChL);
	tgui::Button::Ptr ChB = themeTG->load("Button");
	ChB->setPosition(500, 140);
	ChB->connect("pressed", &UI::setKey, this, ChB, std::ref(options->chat));
	ChB->setText(SFKeyToString(options->chat));
	GenOpt->add(ChB,"BindChat");

	tgui::Label::Ptr ScL = themeTG->load("Label");
	ScL->setPosition(400, 183);
	ScL->setSize(90, 30);
	ScL->setText("Score");
	ScL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ScL);
	tgui::Button::Ptr ScB = themeTG->load("Button");
	ScB->setPosition(500, 180);
	ScB->connect("pressed", &UI::setKey, this, ScB, std::ref(options->score));
	ScB->setText(SFKeyToString(options->score));
	GenOpt->add(ScB,"BindScore");

	tgui::Label::Ptr AwL = themeTG->load("Label");
	AwL->setPosition(400, 223);
	AwL->setSize(90, 30);
	AwL->setText("Away");
	AwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(AwL);
	tgui::Button::Ptr AwB = themeTG->load("Button");
	AwB->setPosition(500, 220);
	AwB->connect("pressed", &UI::setKey, this, AwB, std::ref(options->away));
	AwB->setText(SFKeyToString(options->away));
	GenOpt->add(AwB,"BindAway");

	tgui::Button::Ptr Rp[7]; // Align Pieces
	tgui::Button::Ptr Cc[7];
	for (int i=0; i<7; i++) {
		Rp[i] = themeBB->load("Button");
		Cc[i] = themeBB->load("Button");

		Rp[i]->setPosition(i*115+20, 365);
		Rp[i]->setSize(30,30);
		Rp[i]->setOpacity(0.7);
		Rp[i]->connect("pressed", &UI::rotPiece, this, i);

		Cc[i]->setPosition(i*115+60, 365);
		Cc[i]->setSize(30,30);
		Cc[i]->connect("pressed", &UI::colPiece, this, i);

		GenOpt->add(Rp[i]);
		GenOpt->add(Cc[i]);
	}

	tgui::Label::Ptr Rl1 = themeTG->load("Label"); // Repeat delay & speed
	Rl1->setPosition(750, 0);
	Rl1->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl1->setText("Left & Right Repeat\nDelay         Speed");
	GenOpt->add(Rl1);

	tgui::EditBox::Ptr Re1 = themeTG->load("EditBox");
	Re1->setPosition(750, 50);
	Re1->setSize(70, 30);
	Re1->setInputValidator(tgui::EditBox::Validator::UInt);
	Re1->setText(to_string(options->repeatDelay.asMilliseconds()));
	Re1->connect("TextChanged", &optionSet::setDelay, options, 1);
	GenOpt->add(Re1);

	tgui::EditBox::Ptr Re2 = themeTG->load("EditBox");
	Re2->setPosition(850, 50);
	Re2->setSize(70, 30);
	Re2->setInputValidator(tgui::EditBox::Validator::UInt);
	Re2->setText(to_string(options->repeatSpeed.asMilliseconds()));
	Re2->connect("TextChanged", &optionSet::setDelay, options, 2);
	GenOpt->add(Re2);

	tgui::Label::Ptr Rl2 = themeTG->load("Label");
	Rl2->setPosition(750, 100);
	Rl2->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl2->setText("Down Repeat\n  Delay         Speed");
	GenOpt->add(Rl2);

	tgui::EditBox::Ptr Re3 = themeTG->load("EditBox");
	Re3->setPosition(750, 150);
	Re3->setSize(70, 30);
	Re3->setInputValidator(tgui::EditBox::Validator::UInt);
	Re3->setText(to_string(options->repeatDelayDown.asMilliseconds()));
	Re3->connect("TextChanged", &optionSet::setDelay, options, 3);
	GenOpt->add(Re3);

	tgui::EditBox::Ptr Re4 = themeTG->load("EditBox");
	Re4->setPosition(850, 150);
	Re4->setSize(70, 30);
	Re4->setInputValidator(tgui::EditBox::Validator::UInt);
	Re4->setText(to_string(options->repeatSpeedDown.asMilliseconds()));
	Re4->connect("TextChanged", &optionSet::setDelay, options, 4);
	GenOpt->add(Re4);

	tgui::Panel::Ptr VidOpt = tgui::Panel::create(); // Video Options
	VidOpt->setSize(960, 500);
	VidOpt->setPosition(0, 100);
	VidOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VidOpt->hide();
	gui.add(VidOpt, "VidOpt");

	tgui::Label::Ptr ViL = themeTG->load("Label");
	ViL->setPosition(350, 0);
	ViL->setText("Video Mode");
	VidOpt->add(ViL);

	tgui::Slider::Ptr ViS = themeTG->load("Slider");
	ViS->setPosition(50, 50);
	ViS->setSize(700, 30);
	ViS->setMaximum(options->modes.size()-1);
	if (options->currentmode == -1)
		ViS->setValue(0);
	else
		ViS->setValue(options->currentmode);
	ViS->connect("ValueChanged", &UI::vidSlide, this);
	ViS->disable();
	VidOpt->add(ViS, "VMSlider");

	tgui::Label::Ptr SvL = themeTG->load("Label");
	SvL->setPosition(340, 90);
	sf::String cvmname;
	cvmname = to_string(options->modes[ViS->getValue()].width) + "x" + to_string(options->modes[ViS->getValue()].height);
	SvL->setText(cvmname);
	VidOpt->add(SvL, "VideoMode");

	tgui::CheckBox::Ptr FsC = themeTG->load("CheckBox");
	FsC->setPosition(120, 130);
	FsC->setText("Fullscreen");
	FsC->connect("Checked Unchecked", &UI::fsChecked, this);
	VidOpt->add(FsC, "Fullscreen");
	if (options->fullscreen) {
		FsC->check();
		ViS->enable();
	}

	tgui::CheckBox::Ptr VsC = themeTG->load("CheckBox");
	VsC->setPosition(380, 130);
	VsC->setText("VSync");
	if (options->vSync)
		VsC->check();
	VidOpt->add(VsC, "vSync");

	tgui::Label::Ptr FdL = themeTG->load("Label");
	FdL->setPosition(150, 200);
	FdL->setText("Frame Rate");
	VidOpt->add(FdL);

	tgui::EditBox::Ptr FdE = themeTG->load("EditBox");
	FdE->setPosition(170, 230);
	FdE->setSize(70, 30);
	FdE->setInputValidator(tgui::EditBox::Validator::UInt);
	FdE->setText(to_string(1000000/options->frameDelay.asMicroseconds()));
	VidOpt->add(FdE, "FrameDelay");

	tgui::Label::Ptr IdL = themeTG->load("Label");
	IdL->setPosition(400, 200);
	IdL->setText("Frame Time [microseconds]");
	VidOpt->add(IdL);

	tgui::EditBox::Ptr IdE = themeTG->load("EditBox");
	IdE->setPosition(460, 230);
	IdE->setSize(110, 30);
	IdE->setInputValidator(tgui::EditBox::Validator::UInt);
	IdE->setText(to_string(options->inputDelay.asMicroseconds()));
	VidOpt->add(IdE, "InputDelay");

	tgui::Button::Ptr AvB = themeBB->load("Button");
	AvB->setText("Apply");
	AvB->setPosition(340, 300);
	AvB->setOpacity(0.7);
	AvB->connect("pressed", &UI::applyVideo, this);
	VidOpt->add(AvB);

	tgui::Label::Ptr InL = themeTG->load("Label");
	InL->setPosition(10, 390);
	InL->setText("Note: Enabling vSync will disable both FrameRate and FrameTime settings.\nLower FrameTime will increase how responsive the game is for input but will also increase CPU load.\nIf you experience the controls as too sensitive, try increasing FrameTime to 5000-10000\nRecommended settings FrameRate:100 FrameTime:1000 ");
	VidOpt->add(InL);

	tgui::Panel::Ptr SndOpt = tgui::Panel::create(); // Sound Options
	SndOpt->setSize(800, 500);
	SndOpt->setPosition(0, 100);
	SndOpt->setBackgroundColor(sf::Color(255,255,255,0));
	SndOpt->hide();
	gui.add(SndOpt, "SndOpt");

	tgui::CheckBox::Ptr EsC = themeTG->load("CheckBox");
	EsC->setPosition(150, 0);
	EsC->setText("Sound Enabled");
	EsC->connect("Checked Unchecked", &UI::sndChecked, this);
	SndOpt->add(EsC, "sndCheck");
	if (options->sound)
		EsC->check();

	tgui::Slider::Ptr MvS = themeTG->load("Slider");
	MvS->setPosition(50, 100);
	MvS->setSize(700,30);
	MvS->setMaximum(100);
	MvS->connect("ValueChanged", &UI::volSlide, this, 1);
	MvS->setValue(options->MusicVolume);
	SndOpt->add(MvS);

	tgui::Label::Ptr MvL = themeTG->load("Label");
	MvL->setText("Music Volume");
	MvL->setPosition(350, 50);
	SndOpt->add(MvL);

	tgui::Slider::Ptr EvS = themeTG->load("Slider");
	EvS->setPosition(50, 200);
	EvS->setSize(700,30);
	EvS->setMaximum(100);
	EvS->connect("ValueChanged", &UI::volSlide, this, 2);
	EvS->setValue(options->EffectVolume);
	SndOpt->add(EvS);

	tgui::Label::Ptr EvL = themeTG->load("Label");
	EvL->setText("Effect Volume");
	EvL->setPosition(350, 150);
	SndOpt->add(EvL);

	tgui::Slider::Ptr CvS = themeTG->load("Slider");
	CvS->setPosition(50, 300);
	CvS->setSize(700,30);
	CvS->setMaximum(100);
	CvS->connect("ValueChanged", &UI::volSlide, this, 3);
	CvS->setValue(options->ChatVolume);
	SndOpt->add(CvS);

	tgui::Label::Ptr CvL = themeTG->load("Label");
	CvL->setText("Chat Volume");
	CvL->setPosition(350, 250);
	SndOpt->add(CvL);

	// In-game UI starts here!

	tgui::Tab::Ptr IgT = themeBB->load("Tab");
	IgT->add("GameFields");
	IgT->add("Score");
	IgT->add("Chat");
	IgT->setPosition(465, 0);
	IgT->hide();
	IgT->select(0);
	IgT->connect("TabSelected", &UI::igtabSelect, this);
	IgT->setOpacity(0.7);
	gui.add(IgT, "InGameTab");

	tgui::Panel::Ptr GfP = tgui::Panel::create(); // GameFields panel
	GfP->setPosition(465, 40);
	GfP->setSize(490, 555);
	GfP->hide();
	GfP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(GfP, "GameFields");

	tgui::Panel::Ptr ScP = tgui::Panel::create(); // Score panel
	ScP->setPosition(465,40);
	ScP->setSize(490, 555);
	ScP->hide();
	ScP->setBackgroundColor(sf::Color(200,200,200, 50));
	gui.add(ScP, "Score");

	tgui::Label::Ptr SbL = themeTG->load("Label");
	SbL->setPosition(5, 5);
	SbL->setText("Name                  Score  Rank  BPM  Cmb  Sent  Adj  SPM  Blocked");
	SbL->setTextSize(16);
	ScP->add(SbL, "ScoreBoxLab");

	tgui::Panel::Ptr ChP = tgui::Panel::create(); // Chat panel
	ChP->setPosition(465,40);
	ChP->setSize(490, 555);
	ChP->hide();
	ChP->setBackgroundColor(sf::Color(200,200,200, 50));
	gui.add(ChP, "Chat");

	tgui::Tab::Ptr ChT = themeBB->load("Tab");
	ChT->add("Room");
	ChT->add("Lobby");
	ChT->setTabHeight(30);
	ChT->select(0);
	ChT->connect("TabSelected", &UI::chattabSelect, this);
	ChT->setOpacity(0.7);
	ChP->add(ChT, "ChatTab");

	tgui::ChatBox::Ptr RoC = themeBB->load("ChatBox");
	RoC->setPosition(5, 35);
	RoC->setSize(480, 475);
	ChP->add(RoC, "Room");

	tgui::ChatBox::Ptr LoC = themeBB->load("ChatBox");
	LoC->setPosition(5, 35);
	LoC->setSize(480, 475);
	LoC->hide();
	ChP->add(LoC, "Lobby");

	tgui::EditBox::Ptr ChE = themeTG->load("EditBox");
	ChE->setPosition(5, 520);
	ChE->setSize(480, 30);
	ChE->connect("ReturnKeyPressed", &UI::sendMsg, this, std::bind(&tgui::Tab::getSelected, ChT));
	ChE->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, ChE));
	ChP->add(ChE, "ChatBox");

	tgui::Panel::Ptr WcP = tgui::Panel::create(); // Connecting... Panel
	WcP->setPosition(330,250);
	WcP->setSize(300, 100);
	WcP->hide();
	WcP->setBackgroundColor(sf::Color(50,50,50,150));
	gui.add(WcP, "Connecting");

	tgui::Label::Ptr WcL = themeTG->load("Label");
	WcL->setPosition(0, 40);
	WcL->setSize(300, 50);
	WcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	WcL->setText("Connecting to server...");
	WcP->add(WcL, "ConnText");

	// Online Play UI starts here

	tgui::Tab::Ptr LoT = themeBB->load("Tab"); // Online play Tab
	LoT->add("Rooms");
	LoT->add("Lobby");
	LoT->add("Create room");
	LoT->add("Back");
	LoT->setTabHeight(60);
	LoT->setPosition(50, 20);
	LoT->select(0);
	LoT->connect("TabSelected", &UI::opTabSelect, this);
	LoT->hide();
	LoT->setOpacity(0.7);
	gui.add(LoT, "opTab");

	tgui::Panel::Ptr RoP = tgui::Panel::create(); // Rooms Panel
	RoP->setPosition(0,100);
	RoP->setSize(960, 500);
	RoP->hide();
	RoP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(RoP, "Rooms");

	tgui::Scrollbar::Ptr RoS = themeTG->load("Scrollbar");
	RoS->setSize(30, 490);
	RoS->setPosition(400, 5);
	RoS->setMaximum(0);
	RoS->setLowValue(1);
	RoS->connect("ValueChanged", &UI::roomScrolled, this);
	RoP->add(RoS, "RoomScroll");

	tgui::Panel::Ptr LoP = tgui::Panel::create(); // Lobby Panel
	LoP->setPosition(0,100);
	LoP->setSize(960, 500);
	LoP->hide();
	LoP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(LoP, "ServerLobby");

	tgui::ChatBox::Ptr SlC = themeBB->load("ChatBox");
	SlC->setPosition(5, 5);
	SlC->setSize(950, 455);
	LoP->add(SlC, "Lobby2");

	tgui::EditBox::Ptr SlE = themeTG->load("EditBox");
	SlE->setPosition(5, 465);
	SlE->setSize(950, 30);
	SlE->connect("ReturnKeyPressed", &UI::sendMsg, this, "Lobby");
	SlE->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, SlE));
	LoP->add(SlE, "slChatBox");

	tgui::Panel::Ptr CrP = tgui::Panel::create(); // Create room panel
	CrP->setPosition(0,100);
	CrP->setSize(960, 500);
	CrP->hide();
	CrP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(CrP, "CreateRoom");

	tgui::Label::Ptr NorL = themeTG->load("Label");
	NorL->setPosition(30, 30);
	NorL->setText("Room name");
	CrP->add(NorL);

	tgui::EditBox::Ptr NorE = themeTG->load("EditBox");
	NorE->setPosition(25, 60);
	NorE->setSize(250, 40);
	CrP->add(NorE);

	tgui::Label::Ptr NopL = themeTG->load("Label");
	NopL->setPosition(430, 30);
	NopL->setText("Max players (0=unlimited)");
	CrP->add(NopL);

	tgui::EditBox::Ptr NopE = themeTG->load("EditBox");
	NopE->setPosition(425, 60);
	NopE->setSize(250, 40);
	NopE->setInputValidator(tgui::EditBox::Validator::UInt);
	CrP->add(NopE);

	tgui::Button::Ptr CrB = themeBB->load("Button");
	CrB->setPosition(300, 150);
	CrB->setText("Create");
	CrB->setSize(100, 40);
	CrB->connect("Pressed", &UI::createRoom, this, std::bind(&tgui::EditBox::getText, NorE), std::bind(&tgui::EditBox::getText, NopE));
	CrP->add(CrB);

	tgui::Panel::Ptr AUS = tgui::Panel::create(); // Are you sure? Panel
	AUS->setPosition(330,250);
	AUS->setSize(300, 100);
	AUS->hide();
	AUS->setBackgroundColor(sf::Color(50,50,50, 200));
	gui.add(AUS, "AUS");

	tgui::Label::Ptr AUSL = themeTG->load("Label");
	AUSL->setPosition(0, 20);
	AUSL->setSize(300, 50);
	AUSL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	AUSL->setText("Are you sure?");
	AUS->add(AUSL, "AUSL");

	tgui::Button::Ptr AUSYB = themeBB->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &UI::ausY, this);
	AUS->add(AUSYB);

	tgui::Button::Ptr AUSNB = themeBB->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &UI::ausN, this);
	AUS->add(AUSNB);

	tgui::Label::Ptr QmL = themeTG->load("Label");
	QmL->setPosition(0,10);
	QmL->setSize(960,90);
	QmL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	QmL->setTextSize(32);
	QmL->setTextColor(sf::Color::Red);
	QmL->hide();
	gui.add(QmL, "QuickMsg");

	tgui::Button::Ptr DbG = themeBB->load("Button");
	DbG->setPosition(320, 565);
	DbG->setSize(120, 32);
	DbG->setText("Bug Report");
	DbG->connect("Pressed", &UI::bugReport, this);
	gui.add(DbG);
}

void UI::sendReport(sf::String happened, sf::String expected, sf::String reproduce, sf::String contact, tgui::ChildWindow::Ptr win) {
	if (!playonline) {
		quickMsg("You must connect to the server to send a bug-report");
		return;
	}
	net->packet.clear();
	sf::Uint8 packetid = 99;
	net->packet << packetid << happened << expected << reproduce << contact;
	net->sendTCP();
	win->destroy();
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
	ChW->add(WhT);

	tgui::Label::Ptr WhL2 = themeBB->load("Label");
	WhL2->setPosition(10, 125);
	WhL2->setText("What did you expect to happen?");
	ChW->add(WhL2);

	tgui::TextBox::Ptr WhT2 = themeBB->load("TextBox");
	WhT2->setPosition(5, 160);
	WhT2->setSize(390, 80);
	ChW->add(WhT2);

	tgui::Label::Ptr WhL3 = themeBB->load("Label");
	WhL3->setPosition(10, 245);
	WhL3->setText("How can we reproduce this?");
	ChW->add(WhL3);

	tgui::TextBox::Ptr WhT3 = themeBB->load("TextBox");
	WhT3->setPosition(5, 280);
	WhT3->setSize(390, 80);
	ChW->add(WhT3);

	tgui::Label::Ptr WhL4 = themeBB->load("Label");
	WhL4->setPosition(10, 365);
	WhL4->setText("How can we contact you with questions?");
	ChW->add(WhL4);

	tgui::TextBox::Ptr WhT4 = themeBB->load("TextBox");
	WhT4->setPosition(5, 400);
	WhT4->setSize(390, 40);
	ChW->add(WhT4);

	tgui::Button::Ptr send = themeBB->load("Button");
	send->setPosition(170, 445);
	send->setSize(60, 30);
	send->setText("Send");
	send->connect("Pressed", &UI::sendReport, this, std::bind(&tgui::TextBox::getText, WhT), std::bind(&tgui::TextBox::getText, WhT2), std::bind(&tgui::TextBox::getText, WhT3), std::bind(&tgui::TextBox::getText, WhT4), ChW);
	ChW->add(send);
}

void UI::createRoom(const sf::String& name, const sf::String& maxplayers) {
	if (!name.getSize())
		return;
	if (!maxplayers.getSize())
		return;
	sf::Uint8 max = stoi(maxplayers.toAnsiString()), packetid = 11; //11-Packet
	net->packet.clear();
	net->packet << packetid << name << max;
	net->sendTCP();
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

void UI::addRoom(const sf::String& name, sf::Uint8 curr, sf::Uint8 max, sf::Uint16 id) {
	playRoom newroom;
	playRooms.push_back(newroom);
	playRooms.back().name = name;
	playRooms.back().currentPlayers = curr;
	playRooms.back().maxPlayers = max;
	playRooms.back().button = themeTG->load("Button");
	playRooms.back().button->setText(name);
	playRooms.back().button->setSize(300, 100);
	playRooms.back().button->connect("Pressed", &UI::joinRoom, this, id);
	gui.get<tgui::Panel>("Rooms")->add(playRooms.back().button);

	cout << "adding room " << playRooms.back().name.toAnsiString() << " as " << playRooms.back().id;

	playRooms.back().label = themeTG->load("Label");
	playRooms.back().label->setText(to_string(curr) + "/" + to_string(max) + " players");

	playRooms.back().id = id;

	setRoomPos();
}

void UI::joinRoom(sf::Uint16 id) { //0-Packet
	net->packet.clear();
	sf::Uint8 packetid = 0;
	net->packet << packetid << id;
	net->sendTCP();
	away=false;
	game->autoaway=false;
}

void UI::leaveRoom() { //1-Packet
	net->packet.clear();
	sf::Uint8 packetid = 1;
	net->packet << packetid;
	net->sendTCP();
	inroom=false;
	setGameState(MainMenu);
}

void UI::removeRoom(sf::Uint16 id) {
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		if (it->id == id) {
			cout << "removing room " << it->name.toAnsiString() << " as " << it->id << endl;
			gui.get<tgui::Panel>("Rooms")->remove(it->button);
			it = playRooms.erase(it);
			setRoomPos();
			return;
		}
	}
}

void UI::removeAllRooms() {
	while (playRooms.size() > 0)
		playRooms.pop_front();
}

void UI::setRoomPos() {
	short height = playRooms.size() * 120 + 10;
	if (height > 500) {
		height-=500;
		height/=30;
		height++;
		gui.get<tgui::Scrollbar>("RoomScroll", 1)->setMaximum(height);
	}
	else
		gui.get<tgui::Scrollbar>("RoomScroll", 1)->setMaximum(0);
	short scrollpos = gui.get<tgui::Scrollbar>("RoomScroll", 1)->getValue();
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		int i = std::distance(playRooms.begin(), it);
		it->button->setPosition(50, i*120 - scrollpos*30 + 10);
	}
}

void UI::roomScrolled(int c) {
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		int i = std::distance(playRooms.begin(), it);
		it->button->setPosition(50, i*120 - c*30 + 10);
	}
}

void UI::login(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	gui.get("MainMenu")->hide();
	gui.get("Login")->hide();
	gui.get("Connecting")->show();
	window->draw(textureBase->background); //Update the screen so a block on connect will show the connecting screen
	gui.draw();
	window->display();
	net->serverAdd = gui.get<tgui::EditBox>("IPAddr", true)->getText().toAnsiString();
	if (net->connect() == sf::Socket::Done) {
		net->udpSock.unbind();
		net->udpSock.bind(sf::Socket::AnyPort);
		net->localUdpPort = net->udpSock.getLocalPort();
		net->packet.clear();
		sf::Uint8 packetid = 2; //2-Packet
		sf::Uint16 port = net->localUdpPort;
		net->packet << packetid << clientVersion << port << guest << name << pass;
		net->sendTCP();
		playonline=true;
		if (guest)
			game->field.setName(name, *printFont);
	}
	else {
		net->disconnect();
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
    game->startCountdown();
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
        game->sRKey();
        game->sLKey();
        game->sDKey();
        game->gameover=false;
	}
	else if (state == Game) {
		linesSent=0;
        garbageCleared=0;
        linesBlocked=0;
        gamedatacount=0;
		gamedata=sf::seconds(0);
        game->startGame();
	}
	else if (state == GameOver) {
        if (game->autoaway)
            goAway();
        if (game->sendgameover)
            sendGameOver();
        if (game->winner)
            sendGameWinner();
	}

	gamestate = state;
}

void UI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		gui.get("Rooms")->show();
		gui.get("ServerLobby")->hide();
		gui.get("CreateRoom")->hide();
	}
	else if (tab == "Lobby") {
		gui.get("ServerLobby")->show();
		gui.get("Rooms")->hide();
		gui.get("CreateRoom")->hide();
	}
	else if (tab == "Create room") {
		gui.get("ServerLobby")->hide();
		gui.get("Rooms")->hide();
		gui.get("CreateRoom")->show();
	}
	else if (tab == "Back") {
		gui.get("ServerLobby")->hide();
		gui.get("Rooms")->hide();
		gui.get("opTab")->hide();
		gui.get("CreateRoom")->hide();
		gui.get("MainMenu")->show();
		net->disconnect();
		playonline=false;
	}
}

void UI::ausY() {
	if (playonline)
		leaveRoom();
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
	quickMsgClock.restart();
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
	sf::Uint8 packetid = 10;
	if (msg[0]=='/' && msg[1]=='w' && msg[2]==' ') {
		short until = msg.find(' ', 3);
		sf::String privto = msg.substring(3, until-3);
		sf::String privmsg = msg.substring(until, sf::String::InvalidPos);
		net->packet.clear();
		net->packet << packetid;
		packetid = 3;
		net->packet << packetid << privto << privmsg;
		net->sendTCP();
		gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
		gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
		return;
	}
	sf::String postmsg = game->field.name + ": " + msg;
	gui.get<tgui::ChatBox>(to, 1)->addLine(postmsg, sf::Color(255, 0, 0));
	if (to == "Lobby")
		gui.get<tgui::ChatBox>("Lobby2", 1)->addLine(postmsg, sf::Color(255, 0, 0));
	gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
	gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
	net->packet.clear(); //10-Packet
	net->packet << packetid;
	if (to == "Room") {
		packetid = 1;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else if (to == "Lobby") {
		packetid = 2;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else {
		packetid = 3;
		net->packet << packetid << to << msg;
		net->sendTCP();
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

void UI::scoreRow(sf::String&& name, short score, short rank, short bpm, short combo, short sent, float adj, short spm, short received, short blocked) {
	sf::String rounding = to_string((int)adj); //A bit messy-looking way of rounding float to 1 decimal
	rounding += "." + to_string((int)((adj - (int)adj)*10));

	tgui::Label::Ptr label = themeTG->load("Label");
	label->setText(name);
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
	}
}

void UI::Score() {
	if (gui.get("Score")->isVisible()) {
		gui.get("Score")->hide();
		gui.get("GameFields")->show();
		gui.get<tgui::Tab>("InGameTab")->select(0);
	}
	else {
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
		if (options->fullscreen)
			gui.get<tgui::CheckBox>("Fullscreen", 1)->check();
		else
			gui.get<tgui::CheckBox>("Fullscreen", 1)->uncheck();
		if (options->vSync)
			gui.get<tgui::CheckBox>("vSync", 1)->check();
		else
			gui.get<tgui::CheckBox>("vSync", 1)->uncheck();
		gui.get<tgui::EditBox>("FrameDelay", 1)->setText(to_string(1000000/options->frameDelay.asMicroseconds()));
		gui.get<tgui::EditBox>("InputDelay", 1)->setText(to_string(options->inputDelay.asMicroseconds()));
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
		options->MusicVolume = vol;
		sounds->setMusicVolume(vol);
	}
	else if (i == 2) {
		options->EffectVolume = vol;
		sounds->setEffectVolume(vol);
	}
	else if (i == 3) {
		options->ChatVolume = vol;
		sounds->setChatVolume(vol);
	}
}

void UI::sndChecked(bool i) {
	if (gui.get<tgui::CheckBox>("sndCheck", 1)->isChecked())
		options->sound = true;
	else
		options->sound = false;
}

void UI::fsChecked(bool i) {
	if (gui.get<tgui::CheckBox>("Fullscreen", 1)->isChecked())
		gui.get("VMSlider", 1)->enable();
	else
		gui.get("VMSlider", 1)->disable();
}

void UI::applyVideo() {
	if (gui.get<tgui::CheckBox>("Fullscreen", 1)->isChecked()) {
		if (options->currentmode == -1 || options->currentmode != gui.get<tgui::Slider>("VMSlider", 1)->getValue()) {
			options->currentmode = gui.get<tgui::Slider>("VMSlider", 1)->getValue();
			window->close();
			window->create(options->modes[options->currentmode], "SpeedBlocks", sf::Style::Fullscreen);
			window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
			options->fullscreen=true;
		}
	}
	else if (options->currentmode != -1) {
		window->close();
		window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		options->currentmode = -1;
		window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		options->fullscreen=false;
	}

	if (gui.get<tgui::CheckBox>("vSync", 1)->isChecked()) {
		options->vSync = true;
		window->setVerticalSyncEnabled(true);
	}
	else {
		options->vSync = false;
		window->setVerticalSyncEnabled(false);
	}

	std::string fd = gui.get<tgui::EditBox>("FrameDelay", 1)->getText();
	short value=0;
	if (fd.size())
		value = stoi(fd);
	if (value)
		options->frameDelay = sf::microseconds(1000000/value);
	value=0;
	fd = gui.get<tgui::EditBox>("InputDelay", 1)->getText();
	if (fd.size())
		value = stoi(fd);
	if (value)
		options->inputDelay = sf::microseconds(value);
}

void UI::vidSlide(short i) {
	sf::String name;
	name = to_string(options->modes[i].width) + "x" + to_string(options->modes[i].height);
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

		if (event.key.code == options->left) {
			options->left = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindLeft", true)->setText("");
		}

		if (event.key.code == options->right) {
			options->right = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRight", true)->setText("");
		}

		if (event.key.code == options->down) {
			options->down = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindDown", true)->setText("");
		}

		if (event.key.code == options->rcw) {
			options->rcw = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRCW", true)->setText("");
		}

		if (event.key.code == options->rccw) {
			options->rccw = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindRCCW", true)->setText("");
		}

		if (event.key.code == options->r180) {
			options->r180 = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindR180", true)->setText("");
		}

		if (event.key.code == options->hd) {
			options->hd = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindHD", true)->setText("");
		}

		if (event.key.code == options->chat) {
			options->chat = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindChat", true)->setText("");
		}

		if (event.key.code == options->score) {
			options->score = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindScore", true)->setText("");
		}

		if (event.key.code == options->away) {
			options->away = sf::Keyboard::Unknown;
			gui.get<tgui::Button>("BindAway", true)->setText("");
		}



        	*key = event.key.code;
        	button->setText(SFKeyToString(event.key.code));
        }
    }
}

void UI::changeName(const sf::String& name) {
	options->name = name;
	game->field.setName(name, *printFont);
}

void UI::rotPiece(short i) {
	options->piecerotation[i]++;
	if (options->piecerotation[i]>3)
		options->piecerotation[i]=0;
	piece[i].setRotation(options->piecerotation[i]*90);
	game->updateBasePieces();
}

void UI::colPiece(short i) {
	options->basepiece[i].tile++;
	if (options->basepiece[i].tile>7)
		options->basepiece[i].tile=1;
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (options->basepiece[i].grid[y][x])
				options->basepiece[i].grid[y][x]=options->basepiece[i].tile;
	piece[i].setColor(pColor(options->basepiece[i].tile));
	game->updateBasePieces();
}

sf::Color pColor(short i) {
	sf::Color col;
	switch (i) {
		case 1: col.r=255; col.g=0; col.b=0; break;
		case 2: col.r=0; col.g=255; col.b=0; break;
		case 3: col.r=0; col.g=0; col.b=255; break;
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

	short value[112] = { 0, 4, 0, 0,
						 0, 4, 0, 0,
						 0, 4, 4, 0,
						 0, 0, 0, 0,

						 0, 3, 0, 0,
						 0, 3, 0, 0,
						 3, 3, 0, 0,
						 0, 0, 0, 0,

						 0, 5, 0, 0,
						 0, 5, 5, 0,
						 0, 0, 5, 0,
						 0, 0, 0, 0,

						 0, 7, 0, 0,
						 7, 7, 0, 0,
						 7, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 0,
						 0, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 6, 6, 0,
						 0, 6, 6, 0,
						 0, 0, 0, 0 };
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
			piece[p].setColor(pColor(options->basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options->piecerotation[p]*90);
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
			piece[p].setColor(pColor(options->basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options->piecerotation[p]*90);
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
