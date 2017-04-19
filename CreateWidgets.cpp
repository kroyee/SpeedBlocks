#include "gui.h"
#include "sounds.h"
#include "gamePlay.h"
#include "network.h"
#include <iostream>
#include "MingwConvert.h"
using namespace std;

void UI::createAllWidgets() {
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