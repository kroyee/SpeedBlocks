#include "GameOptions.h"
#include "gui.h"
#include "gamePlay.h"
#include "optionSet.h"
#include "MainMenu.h"
#include "PerformanceOutput.h"
#include "AnimatedBackground.h"
#include "FieldBackMaker.h"
using std::to_string;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

void GameOptions::create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parentPanel) {
	createBase(_pos, _gui, parentPanel);

	GenOpt = tgui::Panel::create();
	GenOpt->setSize(560, 560);
	GenOpt->setPosition(0, 0);
	GenOpt->setBackgroundColor(sf::Color(255,255,255,0));
	GenOpt->hide();
	panel->add(GenOpt);

	initSprites();

	SelectKey = tgui::Panel::create();
	SelectKey->setBackgroundColor(sf::Color(255,255,255,0));
	SelectKey->setSize(960, 600);
	SelectKey->setPosition(0, 0);
	SelectKey->hide();
	gui->tGui.add(SelectKey);

	tgui::Panel::Ptr box = gui->themeTG->load("Panel");
	box->setSize(300, 100);
	box->setPosition(330, 150);
	SelectKey->add(box);

	tgui::Label::Ptr CKPL = gui->themeTG->load("Label");
	CKPL->setTextSize(32);
	CKPL->setText("Press any key");
	CKPL->setPosition(40, 30);
	box->add(CKPL);

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
	R1L->setPosition(200, 63);
	R1L->setSize(140, 30);
	R1L->setText("Rotate 180");
	R1L->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(R1L);
	BindR180 = gui->themeTG->load("Button");
	BindR180->setPosition(350, 60);
	BindR180->connect("pressed", &GameOptions::setKey, this, BindR180, std::ref(gui->options.r180));
	BindR180->setText(SFKeyToString(gui->options.r180));
	GenOpt->add(BindR180);

	tgui::Label::Ptr HdL = gui->themeTG->load("Label");
	HdL->setPosition(200, 103);
	HdL->setSize(140, 30);
	HdL->setText("Hard Drop");
	HdL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(HdL);
	BindHD = gui->themeTG->load("Button");
	BindHD->setPosition(350, 100);
	BindHD->connect("pressed", &GameOptions::setKey, this, BindHD, std::ref(gui->options.hd));
	BindHD->setText(SFKeyToString(gui->options.hd));
	GenOpt->add(BindHD);

	tgui::Label::Ptr ChL = gui->themeTG->load("Label");
	ChL->setPosition(250, 143);
	ChL->setSize(90, 30);
	ChL->setText("Menu");
	ChL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ChL);
	BindMenu = gui->themeTG->load("Button");
	BindMenu->setPosition(350, 140);
	BindMenu->connect("pressed", &GameOptions::setKey, this, BindMenu, std::ref(gui->options.menu));
	BindMenu->setText(SFKeyToString(gui->options.menu));
	GenOpt->add(BindMenu);

	tgui::Label::Ptr ScL = gui->themeTG->load("Label");
	ScL->setPosition(250, 183);
	ScL->setSize(90, 30);
	ScL->setText("Score");
	ScL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ScL);
	BindScore = gui->themeTG->load("Button");
	BindScore->setPosition(350, 180);
	BindScore->connect("pressed", &GameOptions::setKey, this, BindScore, std::ref(gui->options.score));
	BindScore->setText(SFKeyToString(gui->options.score));
	GenOpt->add(BindScore);

	tgui::Label::Ptr AwL = gui->themeTG->load("Label");
	AwL->setPosition(250, 223);
	AwL->setSize(90, 30);
	AwL->setText("Away");
	AwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(AwL);
	BindAway = gui->themeTG->load("Button");
	BindAway->setPosition(350, 220);
	BindAway->connect("pressed", &GameOptions::setKey, this, BindAway, std::ref(gui->options.away));
	BindAway->setText(SFKeyToString(gui->options.away));
	GenOpt->add(BindAway);

	tgui::Label::Ptr ReL = gui->themeTG->load("Label");
	ReL->setPosition(250, 263);
	ReL->setSize(90, 30);
	ReL->setText("Ready");
	ReL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ReL);
	BindReady = gui->themeTG->load("Button");
	BindReady->setPosition(350, 260);
	BindReady->connect("pressed", &GameOptions::setKey, this, BindReady, std::ref(gui->options.ready));
	BindReady->setText(SFKeyToString(gui->options.ready));
	GenOpt->add(BindReady);

	tgui::Button::Ptr Rp[7]; // Align Pieces
	tgui::Button::Ptr Cc[7];
	for (int i=0; i<7; i++) {
		Rp[i] = tgui::Button::create();
		Rp[i]->getRenderer()->setNormalTexture(gui->resources.gfx.rotate_n);
		Rp[i]->getRenderer()->setHoverTexture(gui->resources.gfx.rotate_h);
		Rp[i]->getRenderer()->setBorders({0,0,0,0});
		Cc[i] = tgui::Button::create();
		Cc[i]->getRenderer()->setNormalTexture(gui->resources.gfx.color_n);
		Cc[i]->getRenderer()->setHoverTexture(gui->resources.gfx.color_h);
		Cc[i]->getRenderer()->setBorders({0,0,0,0});

		Rp[i]->setPosition(i*80+10, 365);
		Rp[i]->setSize(25,25);
		Rp[i]->connect("pressed", &GameOptions::rotPiece, this, i);

		Cc[i]->setPosition(i*80+40, 365);
		Cc[i]->setSize(25,25);
		Cc[i]->connect("pressed", &GameOptions::colPiece, this, i);

		GenOpt->add(Rp[i]);
		GenOpt->add(Cc[i]);
	}

	tgui::Label::Ptr Rl1 = gui->themeTG->load("Label"); // Repeat delay & speed
	Rl1->setPosition(50, 410);
	Rl1->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl1->setText("Left & Right Repeat\nDelay         Speed");
	GenOpt->add(Rl1);

	tgui::EditBox::Ptr Re1 = gui->themeTG->load("EditBox");
	Re1->setPosition(50, 460);
	Re1->setSize(70, 30);
	Re1->setInputValidator(tgui::EditBox::Validator::UInt);
	Re1->setText(to_string(gui->options.repeatDelay.asMilliseconds()));
	Re1->connect("TextChanged", &optionSet::setDelay, &gui->options, 1);
	GenOpt->add(Re1);

	tgui::EditBox::Ptr Re2 = gui->themeTG->load("EditBox");
	Re2->setPosition(150, 460);
	Re2->setSize(70, 30);
	Re2->setInputValidator(tgui::EditBox::Validator::UInt);
	Re2->setText(to_string(gui->options.repeatSpeed.asMilliseconds()));
	Re2->connect("TextChanged", &optionSet::setDelay, &gui->options, 2);
	GenOpt->add(Re2);

	tgui::Label::Ptr Rl2 = gui->themeTG->load("Label");
	Rl2->setPosition(300, 410);
	Rl2->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl2->setText("Down Repeat\n  Delay         Speed");
	GenOpt->add(Rl2);

	tgui::EditBox::Ptr Re3 = gui->themeTG->load("EditBox");
	Re3->setPosition(300, 460);
	Re3->setSize(70, 30);
	Re3->setInputValidator(tgui::EditBox::Validator::UInt);
	Re3->setText(to_string(gui->options.repeatDelayDown.asMilliseconds()));
	Re3->connect("TextChanged", &optionSet::setDelay, &gui->options, 3);
	GenOpt->add(Re3);

	tgui::EditBox::Ptr Re4 = gui->themeTG->load("EditBox");
	Re4->setPosition(400, 460);
	Re4->setSize(70, 30);
	Re4->setInputValidator(tgui::EditBox::Validator::UInt);
	Re4->setText(to_string(gui->options.repeatSpeedDown.asMilliseconds()));
	Re4->connect("TextChanged", &optionSet::setDelay, &gui->options, 4);
	GenOpt->add(Re4);

	VidOpt = tgui::Panel::create(); // Video Options
	VidOpt->setSize(560, 560);
	VidOpt->setPosition(0, 0);
	VidOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VidOpt->hide();
	panel->add(VidOpt);

	tgui::Label::Ptr ViL = gui->themeTG->load("Label");
	ViL->setPosition(250, 0);
	ViL->setText("Video Mode");
	VidOpt->add(ViL);

	VMSlider = gui->themeTG->load("Slider");
	VMSlider->setPosition(50, 50);
	VMSlider->setSize(460, 30);
	VMSlider->setMaximum(gui->options.modes.size()-1);
	if (gui->options.currentmode == -1)
		VMSlider->setValue(0);
	else
		VMSlider->setValue(gui->options.currentmode);
	VMSlider->connect("ValueChanged", &GameOptions::vidSlide, this);
	VMSlider->disable();
	VidOpt->add(VMSlider);

	VideoMode = gui->themeTG->load("Label");
	VideoMode->setPosition(240, 90);
	sf::String cvmname;
	cvmname = to_string(gui->options.modes[VMSlider->getValue()].width) + "x" + to_string(gui->options.modes[VMSlider->getValue()].height);
	VideoMode->setText(cvmname);
	VidOpt->add(VideoMode);

	Fullscreen = gui->themeTG->load("CheckBox");
	Fullscreen->setPosition(60, 130);
	Fullscreen->setText("Fullscreen");
	Fullscreen->connect("Checked Unchecked", &GameOptions::fsChecked, this);
	VidOpt->add(Fullscreen);
	if (gui->options.fullscreen) {
		Fullscreen->check();
		VMSlider->enable();
	}

	vSync = gui->themeTG->load("CheckBox");
	vSync->setPosition(220, 130);
	vSync->setText("VSync");
	if (gui->options.vSync)
		vSync->check();
	VidOpt->add(vSync);

	performanceOutput = gui->themeTG->load("CheckBox");
	performanceOutput->setPosition(320, 130);
	performanceOutput->setText("Performance output");
	if (gui->options.performanceOutput)
		performanceOutput->check();
	VidOpt->add(performanceOutput);

	tgui::Label::Ptr FdL = gui->themeTG->load("Label");
	FdL->setPosition(80, 200);
	FdL->setText("Frame Rate");
	VidOpt->add(FdL);

	FrameDelay = gui->themeTG->load("EditBox");
	FrameDelay->setPosition(100, 230);
	FrameDelay->setSize(70, 30);
	FrameDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	FrameDelay->setText(to_string(1000000/gui->options.frameDelay.asMicroseconds()));
	VidOpt->add(FrameDelay);

	tgui::Label::Ptr IdL = gui->themeTG->load("Label");
	IdL->setPosition(320, 200);
	IdL->setText("Input Rate");
	VidOpt->add(IdL);

	InputDelay = gui->themeTG->load("EditBox");
	InputDelay->setPosition(310, 230);
	InputDelay->setSize(110, 30);
	InputDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	InputDelay->setText(to_string(1000000/gui->options.inputDelay.asMicroseconds()));
	VidOpt->add(InputDelay);

	tgui::Button::Ptr AvB = gui->themeTG->load("Button");
	AvB->setText("Apply");
	AvB->setPosition(240, 300);
	AvB->setOpacity(0.7);
	AvB->connect("pressed", &GameOptions::applyVideo, this);
	VidOpt->add(AvB);

	tgui::Label::Ptr InL = gui->themeTG->load("Label");
	InL->setPosition(10, 390);
	InL->setText("Frame Rate = frames drawn per second\nInput Rate = input parsed per second\nNote: Enabling vSync will disable both FrameRate and InputRate.");
	VidOpt->add(InL);

	SndOpt = tgui::Panel::create(); // Sound Options
	SndOpt->setSize(560, 560);
	SndOpt->setPosition(0, 0);
	SndOpt->setBackgroundColor(sf::Color(255,255,255,0));
	SndOpt->hide();
	panel->add(SndOpt);

	tgui::CheckBox::Ptr EsC = gui->themeTG->load("CheckBox");
	EsC->setPosition(50, 0);
	EsC->setText("Sound Enabled");
	EsC->connect("Checked Unchecked", &GameOptions::sndChecked, this);
	SndOpt->add(EsC, "sndCheck");
	if (gui->options.sound)
		EsC->check();

	tgui::Slider::Ptr MvS = gui->themeTG->load("Slider");
	MvS->setPosition(50, 100);
	MvS->setSize(460,30);
	MvS->setMaximum(100);
	MvS->connect("ValueChanged", &GameOptions::volSlide, this, 1);
	MvS->setValue(gui->options.MusicVolume);
	SndOpt->add(MvS);

	tgui::Label::Ptr MvL = gui->themeTG->load("Label");
	MvL->setText("Music Volume");
	MvL->setPosition(240, 50);
	SndOpt->add(MvL);

	tgui::Slider::Ptr EvS = gui->themeTG->load("Slider");
	EvS->setPosition(50, 200);
	EvS->setSize(460,30);
	EvS->setMaximum(100);
	EvS->connect("ValueChanged", &GameOptions::volSlide, this, 2);
	EvS->setValue(gui->options.EffectVolume);
	SndOpt->add(EvS);

	tgui::Label::Ptr EvL = gui->themeTG->load("Label");
	EvL->setText("Effect Volume");
	EvL->setPosition(240, 150);
	SndOpt->add(EvL);

	tgui::Slider::Ptr CvS = gui->themeTG->load("Slider");
	CvS->setPosition(50, 300);
	CvS->setSize(460,30);
	CvS->setMaximum(100);
	CvS->connect("ValueChanged", &GameOptions::volSlide, this, 3);
	CvS->setValue(gui->options.ChatVolume);
	SndOpt->add(CvS);

	tgui::Label::Ptr CvL = gui->themeTG->load("Label");
	CvL->setText("Chat Volume");
	CvL->setPosition(240, 250);
	SndOpt->add(CvL);

	VisOpt = tgui::Panel::create(); // Sound Options
	VisOpt->setSize(560, 560);
	VisOpt->setPosition(0, 0);
	VisOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VisOpt->hide();
	panel->add(VisOpt);

	tgui::Label::Ptr NL = gui->themeTG->load("Label");
	NL->setPosition(20, 13);
	NL->setText("Name (for single-player)");
	VisOpt->add(NL);
	tgui::EditBox::Ptr NTB = gui->themeTG->load("EditBox");
	NTB->setText(gui->options.name);
	NTB->setPosition(230, 10);
	NTB->setSize(270, 30);
	NTB->connect("TextChanged", &GameOptions::changeName, this);
	VisOpt->add(NTB);

	tgui::CheckBox::Ptr animBack = gui->themeTG->load("CheckBox");
	animBack->setPosition(20, 50);
	animBack->setText("Show background animation");
	animBack->connect("Checked", [&](){ gui->options.animatedBackground=true; gui->animatedBackground->enable(gui->delayClock.getElapsedTime()); });
	animBack->connect("Unchecked", [&](){ gui->options.animatedBackground=false; gui->animatedBackground->disable(); });
	if (gui->options.animatedBackground)
		animBack->check();
	VisOpt->add(animBack);

	tgui::Label::Ptr gpaLabel = gui->themeTG->load("Label");
	gpaLabel->setPosition(100, 90);
	gpaLabel->setText("Ghost piece transparency");
	VisOpt->add(gpaLabel);

	tgui::Slider::Ptr ghostPieceAlpha = gui->themeTG->load("Slider");
	ghostPieceAlpha->setPosition(20, 120);
	ghostPieceAlpha->setSize(520,30);
	ghostPieceAlpha->setMaximum(255);
	ghostPieceAlpha->setValue(gui->options.ghostPieceAlpha);
	ghostPieceAlpha->connect("ValueChanged", &GameOptions::setGhostPieceAlpha, this);
	VisOpt->add(ghostPieceAlpha);

	tgui::RadioButton::Ptr r_lightTheme = gui->themeTG->load("RadioButton");
	r_lightTheme->setPosition(20, 180);
	r_lightTheme->setText("Light theme");
	VisOpt->add(r_lightTheme);

	tgui::RadioButton::Ptr r_darkTheme = gui->themeTG->load("RadioButton");
	r_darkTheme->setPosition(20, 220);
	r_darkTheme->setText("Dark theme");
	VisOpt->add(r_darkTheme);

	if (gui->options.theme == 1)
		r_lightTheme->check();
	else
		r_darkTheme->check();

	r_lightTheme->connect("Checked", &UI::lightTheme, gui);
	r_darkTheme->connect("Checked", &UI::darkTheme, gui);

	tgui::Label::Ptr fbcLabel = gui->themeTG->load("Label");
	fbcLabel->setPosition(100, 260);
	fbcLabel->setText("Game field background color and lines");
	VisOpt->add(fbcLabel);

	tgui::Slider::Ptr fieldBackColor = gui->themeTG->load("Slider");
	fieldBackColor->setPosition(20, 290);
	fieldBackColor->setSize(520,30);
	fieldBackColor->setMaximum(255);
	fieldBackColor->setValue(gui->options.fieldBackground);
	fieldBackColor->connect("ValueChanged", [&](int val){
		gui->options.fieldBackground=val;
		gui->game.field.setBackColor(val);
		gui->game.drawMe=true;
		for (auto& field : gui->gameFieldDrawer.fields)
			field.setBackColor(val);
	});
	VisOpt->add(fieldBackColor);

	tgui::CheckBox::Ptr vlines = gui->themeTG->load("CheckBox");
	vlines->setText("Vertical lines");
	vlines->setPosition(20, 340);
	if (gui->options.fieldVLines)
		vlines->check();
	vlines->connect("Checked", [&](){
		gui->options.fieldVLines=true;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	vlines->connect("Unchecked", [&](){
		gui->options.fieldVLines=false;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	VisOpt->add(vlines);

	tgui::CheckBox::Ptr hlines = gui->themeTG->load("CheckBox");
	hlines->setText("Vertical lines");
	hlines->setPosition(20, 370);
	if (gui->options.fieldHLines)
		hlines->check();
	hlines->connect("Checked", [&](){
		gui->options.fieldHLines=true;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	hlines->connect("Unchecked", [&](){
		gui->options.fieldHLines=false;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	VisOpt->add(hlines);

	tgui::Panel::Ptr linestyle = tgui::Panel::create();
	linestyle->setBackgroundColor(sf::Color(255,255,255,0));
	linestyle->setPosition(175, 340);
	linestyle->setSize(100,90);
	VisOpt->add(linestyle);

	tgui::RadioButton::Ptr style1 = gui->themeTG->load("RadioButton");
	style1->setPosition(0,0);
	style1->setText("Full");
	if (gui->options.lineStyle==1)
		style1->check();
	style1->connect("Checked", [&](){
		gui->options.lineStyle=1;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	linestyle->add(style1);

	tgui::RadioButton::Ptr style2 = gui->themeTG->load("RadioButton");
	style2->setPosition(0,30);
	style2->setText("Intersections");
	if (gui->options.lineStyle==2)
		style2->check();
	style2->connect("Checked", [&](){
		gui->options.lineStyle=2;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	linestyle->add(style2);

	tgui::RadioButton::Ptr style3 = gui->themeTG->load("RadioButton");
	style3->setPosition(0,60);
	style3->setText("Faded");
	if (gui->options.lineStyle==3)
		style3->check();
	style3->connect("Checked", [&](){
		gui->options.lineStyle=3;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	linestyle->add(style3);

	tgui::Panel::Ptr linecolor = tgui::Panel::create();
	linecolor->setBackgroundColor(sf::Color(255,255,255,0));
	linecolor->setPosition(320, 340);
	linecolor->setSize(100,60);
	VisOpt->add(linecolor);

	tgui::RadioButton::Ptr darkLines = gui->themeTG->load("RadioButton");
	darkLines->setPosition(0,0);
	darkLines->setText("Dark");
	if (gui->options.lineColor)
		darkLines->check();
	darkLines->connect("Checked", [&](){
		gui->options.lineColor=true;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	linecolor->add(darkLines);

	tgui::RadioButton::Ptr lightLines = gui->themeTG->load("RadioButton");
	lightLines->setPosition(0,30);
	lightLines->setText("Light");
	if (!gui->options.lineColor)
		lightLines->check();
	lightLines->connect("Checked", [&](){
		gui->options.lineColor=false;
		gui->game.drawMe=true;
		gui->game.field.backgroundTexture = makeBackground(gui->options.fieldVLines, gui->options.fieldHLines, gui->options.lineStyle, gui->options.lineColor);
	});
	linecolor->add(lightLines);

	tgui::CheckBox::Ptr mouseMenu = gui->themeTG->load("CheckBox");
	mouseMenu->setPosition(20, 500);
	mouseMenu->setText("Menu responds to mouse hovering");
	if (gui->options.mouseMenu)
		mouseMenu->check();
	mouseMenu->connect("Checked", [&](){ gui->options.mouseMenu=true; });
	mouseMenu->connect("Unchecked", [&](){ gui->options.mouseMenu=false; });
	VisOpt->add(mouseMenu);
}

void GameOptions::show(sf::Uint8 index) {
	panel->show();
	GenOpt->hide();
	VidOpt->hide();
	SndOpt->hide();
	VisOpt->hide();
	if (index == 0)
		VisOpt->show();
	if (index == 1)
		GenOpt->show();
	else if (index == 2) {
		VidOpt->show();
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
	else if (index == 3)
		SndOpt->show();
}

void GameOptions::changeName(const sf::String& name) {
	gui->options.name = name;
	gui->game.field.text.setName(name);
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
		if (!gui->options.fullscreen || gui->options.currentmode != VMSlider->getValue()) {
			gui->options.fullscreen=true;
			gui->options.currentmode = VMSlider->getValue();
			gui->window->close();
			gui->window->create(gui->options.modes[gui->options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
			gui->window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		}
	}
	else if (gui->options.fullscreen) {
		gui->options.fullscreen=false;
		gui->window->close();
		gui->window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		gui->window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
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
		gui->performanceOutput->show();
	}
	else {
		gui->options.performanceOutput = false;
		gui->performanceOutput->hide();
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
	key=&_key;
	button=_button;
}

bool GameOptions::putKey(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == -1 )
        	SelectKey->hide();
        else {
        	SelectKey->hide();

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

			if (event.key.code == gui->options.menu) {
				gui->options.menu = sf::Keyboard::Unknown;
				BindMenu->setText("");
			}

			if (event.key.code == gui->options.score) {
				gui->options.score = sf::Keyboard::Unknown;
				BindScore->setText("");
			}

			if (event.key.code == gui->options.away) {
				gui->options.away = sf::Keyboard::Unknown;
				BindAway->setText("");
			}

			if (event.key.code == gui->options.ready) {
				gui->options.ready = sf::Keyboard::Unknown;
				BindReady->setText("");
			}



        	*key = event.key.code;
        	button->setText(SFKeyToString(event.key.code));
        }
        return true;
    }
    return false;
}

void GameOptions::rotPiece(short i) {
	gui->options.piecerotation[i]++;
	if (gui->options.piecerotation[i]>3)
		gui->options.piecerotation[i]=0;
	piece[i].setRotation(gui->options.piecerotation[i]*90);
	canvas[i]->clear(sf::Color(255,255,255,0));
	canvas[i]->draw(piece[i]);
	gui->game.updateBasePieces();
}

void GameOptions::colPiece(short i) {
	if (gui->options.basepiece[i].tile+1>7)
        gui->options.setPieceColor(i, 1);
    else
        gui->options.setPieceColor(i, gui->options.basepiece[i].tile+1);
	piece[i].setColor(pColor(gui->options.basepiece[i].tile));
	canvas[i]->clear(sf::Color(255,255,255,0));
	canvas[i]->draw(piece[i]);
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
		}
		else {
			for (int x=0; x<3; x++)
				for (int y=0; y<3; y++)
					if (value[16*p + 4*y + x]) {
						tile.setPosition(x*30+15, y*30+15);
						rendtex.draw(tile);
					}
		}
		rendtex.display();
		texture[p] = rendtex.getTexture();
		piece[p].setTexture(texture[p]);
		piece[p].setScale(0.5, 0.5);
		piece[p].setPosition(30, 30);
		piece[p].setColor(pColor(gui->options.basepiece[p].tile));
		piece[p].setOrigin(60,60);
		piece[p].setRotation(gui->options.piecerotation[p]*90);
		sf::FloatRect size = piece[p].getGlobalBounds();
		canvas[p] = tgui::Canvas::create({size.width, size.height});
		canvas[p]->clear(sf::Color(255,255,255,0));
		canvas[p]->draw(piece[p]);
		canvas[p]->setPosition(80*p+10, 300);
		GenOpt->add(canvas[p]);
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

void GameOptions::setGhostPieceAlpha(sf::Uint8 alpha) {
	gui->options.ghostPieceAlpha = alpha;
	gui->resources.gfx.setGhostPieceAlpha(alpha);
	gui->game.drawMe=true;
}