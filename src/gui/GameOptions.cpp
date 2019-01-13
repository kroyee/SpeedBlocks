#include "GameOptions.h"
#include "Options.h"
#include "GameSignals.h"
#include "Resources.h"
using std::to_string;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

static auto& EnableBackground = Signal<void, const sf::Time&>::get("EnableBackground");
static auto& DisableBackground = Signal<void>::get("DisableBackground");
static auto& LightTheme = Signal<void>::get("LightTheme");
static auto& DarkTheme = Signal<void>::get("DarkTheme");
static auto& SetGameBackColor = Signal<void, int>::get("SetGameBackColor");
static auto& SetDrawMe = Signal<void>::get("SetDrawMe");
static auto& SetFieldsBackColor = Signal<void, int>::get("SetFieldsBackColor");
static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& SetMusicVolume = Signal<void, int>::get("SetMusicVolume");
static auto& SetEffectVolume = Signal<void, int>::get("SetEffectVolume");
static auto& SetAlertsVolume = Signal<void, int>::get("SetAlertsVolume");

GameOptions::GameOptions(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : GuiBase(_pos, _res, parentPanel) {

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
	resources.gfx->tGui.add(SelectKey);

	tgui::Panel::Ptr box = resources.gfx->load("Panel");
	box->setSize(300, 100);
	box->setPosition(330, 150);
	SelectKey->add(box);

	tgui::Label::Ptr CKPL = resources.gfx->load("Label");
	CKPL->setTextSize(32);
	CKPL->setText("Press any key");
	CKPL->setPosition(40, 30);
	box->add(CKPL);

	tgui::Label::Ptr LeL = resources.gfx->load("Label"); // Binding Keys
	LeL->setPosition(0, 63);
	LeL->setSize(90, 30);
	LeL->setText("Left");
	LeL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(LeL);
	BindLeft = resources.gfx->load("Button");
	BindLeft->setPosition(100, 60);
	BindLeft->connect("pressed", &GameOptions::setKey, this, BindLeft, std::ref(Options::get<sf::Keyboard::Key>("left")));
	BindLeft->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("left")));
	GenOpt->add(BindLeft);

	tgui::Label::Ptr RiL = resources.gfx->load("Label");
	RiL->setPosition(0, 103);
	RiL->setSize(90, 30);
	RiL->setText("Right");
	RiL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(RiL);
	BindRight = resources.gfx->load("Button");
	BindRight->setPosition(100, 100);
	BindRight->connect("pressed", &GameOptions::setKey, this, BindRight, std::ref(Options::get<sf::Keyboard::Key>("right")));
	BindRight->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("right")));
	GenOpt->add(BindRight);

	tgui::Label::Ptr DoL = resources.gfx->load("Label");
	DoL->setPosition(0, 143);
	DoL->setSize(90, 30);
	DoL->setText("Down");
	DoL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(DoL);
	BindDown = resources.gfx->load("Button");
	BindDown->setPosition(100, 140);
	BindDown->connect("pressed", &GameOptions::setKey, this, BindDown, std::ref(Options::get<sf::Keyboard::Key>("down")));
	BindDown->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("down")));
	GenOpt-> add(BindDown);

	tgui::Label::Ptr CwL = resources.gfx->load("Label");
	CwL->setPosition(0, 223);
	CwL->setSize(90, 30);
	CwL->setText("Rotate >");
	CwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CwL);
	BindRCW = resources.gfx->load("Button");
	BindRCW->setPosition(100, 220);
	BindRCW->connect("pressed", &GameOptions::setKey, this, BindRCW, std::ref(Options::get<sf::Keyboard::Key>("rcw")));
	BindRCW->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("rcw")));
	GenOpt->add(BindRCW);

	tgui::Label::Ptr CcL = resources.gfx->load("Label");
	CcL->setPosition(0, 183);
	CcL->setSize(90, 30);
	CcL->setText("Rotate <");
	CcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CcL);
	BindRCCW = resources.gfx->load("Button");
	BindRCCW->setPosition(100, 180);
	BindRCCW->connect("pressed", &GameOptions::setKey, this, BindRCCW, std::ref(Options::get<sf::Keyboard::Key>("rccw")));
	BindRCCW->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("rccw")));
	GenOpt->add(BindRCCW);

	tgui::Label::Ptr R1L = resources.gfx->load("Label");
	R1L->setPosition(200, 63);
	R1L->setSize(140, 30);
	R1L->setText("Rotate 180");
	R1L->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(R1L);
	BindR180 = resources.gfx->load("Button");
	BindR180->setPosition(350, 60);
	BindR180->connect("pressed", &GameOptions::setKey, this, BindR180, std::ref(Options::get<sf::Keyboard::Key>("r180")));
	BindR180->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("r180")));
	GenOpt->add(BindR180);

	tgui::Label::Ptr HdL = resources.gfx->load("Label");
	HdL->setPosition(200, 103);
	HdL->setSize(140, 30);
	HdL->setText("Hard Drop");
	HdL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(HdL);
	BindHD = resources.gfx->load("Button");
	BindHD->setPosition(350, 100);
	BindHD->connect("pressed", &GameOptions::setKey, this, BindHD, std::ref(Options::get<sf::Keyboard::Key>("hd")));
	BindHD->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("hd")));
	GenOpt->add(BindHD);

	tgui::Label::Ptr ChL = resources.gfx->load("Label");
	ChL->setPosition(250, 143);
	ChL->setSize(90, 30);
	ChL->setText("Menu");
	ChL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ChL);
	BindMenu = resources.gfx->load("Button");
	BindMenu->setPosition(350, 140);
	BindMenu->connect("pressed", &GameOptions::setKey, this, BindMenu, std::ref(Options::get<sf::Keyboard::Key>("menu")));
	BindMenu->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("menu")));
	GenOpt->add(BindMenu);

	tgui::Label::Ptr ScL = resources.gfx->load("Label");
	ScL->setPosition(250, 183);
	ScL->setSize(90, 30);
	ScL->setText("Score");
	ScL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ScL);
	BindScore = resources.gfx->load("Button");
	BindScore->setPosition(350, 180);
	BindScore->connect("pressed", &GameOptions::setKey, this, BindScore, std::ref(Options::get<sf::Keyboard::Key>("score")));
	BindScore->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("score")));
	GenOpt->add(BindScore);

	tgui::Label::Ptr AwL = resources.gfx->load("Label");
	AwL->setPosition(250, 223);
	AwL->setSize(90, 30);
	AwL->setText("Away");
	AwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(AwL);
	BindAway = resources.gfx->load("Button");
	BindAway->setPosition(350, 220);
	BindAway->connect("pressed", &GameOptions::setKey, this, BindAway, std::ref(Options::get<sf::Keyboard::Key>("away")));
	BindAway->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("away")));
	GenOpt->add(BindAway);

	tgui::Label::Ptr ReL = resources.gfx->load("Label");
	ReL->setPosition(250, 263);
	ReL->setSize(90, 30);
	ReL->setText("Ready");
	ReL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ReL);
	BindReady = resources.gfx->load("Button");
	BindReady->setPosition(350, 260);
	BindReady->connect("pressed", &GameOptions::setKey, this, BindReady, std::ref(Options::get<sf::Keyboard::Key>("ready")));
	BindReady->setText(SFKeyToString(Options::get<sf::Keyboard::Key>("ready")));
	GenOpt->add(BindReady);

	tgui::Button::Ptr Rp[7]; // Align Pieces
	tgui::Button::Ptr Cc[7];

	tgui::Texture rotate_n(resources.gfx->texture("rotate"));
	tgui::Texture rotate_h(resources.gfx->texture("rotate"));
	rotate_n.setColor({0,0,0});

	tgui::Texture color_n(resources.gfx->texture("color"));
	tgui::Texture color_h(resources.gfx->texture("color"));
	color_n.setColor({0,0,0});

	for (int i=0; i<7; i++) {
		Rp[i] = tgui::Button::create();
		Rp[i]->getRenderer()->setNormalTexture(rotate_n);
		Rp[i]->getRenderer()->setHoverTexture(rotate_h);
		Rp[i]->getRenderer()->setBackgroundColorHover({100,100,100});
		Rp[i]->getRenderer()->setBorders({0,0,0,0});
		Cc[i] = tgui::Button::create();
		Cc[i]->getRenderer()->setNormalTexture(color_n);
		Cc[i]->getRenderer()->setHoverTexture(color_h);
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

	tgui::Label::Ptr Rl1 = resources.gfx->load("Label"); // Repeat delay & speed
	Rl1->setPosition(50, 410);
	Rl1->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl1->setText("Left & Right Repeat\nDelay         Speed");
	GenOpt->add(Rl1);

	tgui::EditBox::Ptr Re1 = resources.gfx->load("EditBox");
	Re1->setPosition(50, 460);
	Re1->setSize(70, 30);
	Re1->setInputValidator(tgui::EditBox::Validator::UInt);
	Re1->setText(to_string(Options::get<sf::Time>("repeatdelay").asMilliseconds()));
	Re1->connect("TextChanged", &Options::setDelay, 1);
	GenOpt->add(Re1);

	tgui::EditBox::Ptr Re2 = resources.gfx->load("EditBox");
	Re2->setPosition(150, 460);
	Re2->setSize(70, 30);
	Re2->setInputValidator(tgui::EditBox::Validator::UInt);
	Re2->setText(to_string(Options::get<sf::Time>("repeatspeed").asMilliseconds()));
	Re2->connect("TextChanged", &Options::setDelay, 2);
	GenOpt->add(Re2);

	tgui::Label::Ptr Rl2 = resources.gfx->load("Label");
	Rl2->setPosition(300, 410);
	Rl2->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl2->setText("Down Repeat\n  Delay         Speed");
	GenOpt->add(Rl2);

	tgui::EditBox::Ptr Re3 = resources.gfx->load("EditBox");
	Re3->setPosition(300, 460);
	Re3->setSize(70, 30);
	Re3->setInputValidator(tgui::EditBox::Validator::UInt);
	Re3->setText(to_string(Options::get<sf::Time>("repeatdelaydown").asMilliseconds()));
	Re3->connect("TextChanged", &Options::setDelay, 3);
	GenOpt->add(Re3);

	tgui::EditBox::Ptr Re4 = resources.gfx->load("EditBox");
	Re4->setPosition(400, 460);
	Re4->setSize(70, 30);
	Re4->setInputValidator(tgui::EditBox::Validator::UInt);
	Re4->setText(to_string(Options::get<sf::Time>("repeatspeeddown").asMilliseconds()));
	Re4->connect("TextChanged", &Options::setDelay, 4);
	GenOpt->add(Re4);

	VidOpt = tgui::Panel::create(); // Video Options
	VidOpt->setSize(560, 560);
	VidOpt->setPosition(0, 0);
	VidOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VidOpt->hide();
	panel->add(VidOpt);

	tgui::Label::Ptr ViL = resources.gfx->load("Label");
	ViL->setPosition(250, 0);
	ViL->setText("Video Mode");
	VidOpt->add(ViL);

	VMSlider = resources.gfx->load("Slider");
	VMSlider->setPosition(50, 50);
	VMSlider->setSize(460, 30);
	VMSlider->setMaximum(Options::get_videomodes().size()-1);
	if (Options::get<short>("currentmode") == -1)
		VMSlider->setValue(0);
	else
		VMSlider->setValue(Options::get<short>("currentmode"));
	VMSlider->connect("ValueChanged", &GameOptions::vidSlide, this);
	VMSlider->disable();
	VidOpt->add(VMSlider);

	VideoMode = resources.gfx->load("Label");
	VideoMode->setPosition(240, 90);
	std::string cvmname;
	cvmname = to_string(Options::get_videomodes()[VMSlider->getValue()].width) + "x" + to_string(Options::get_videomodes()[VMSlider->getValue()].height);
	VideoMode->setText(cvmname);
	VidOpt->add(VideoMode);

	Fullscreen = resources.gfx->load("CheckBox");
	Fullscreen->setPosition(60, 130);
	Fullscreen->setText("Fullscreen");
	Fullscreen->connect("Checked Unchecked", &GameOptions::fsChecked, this);
	VidOpt->add(Fullscreen);
	if (Options::get<bool>("fullscreen")) {
		Fullscreen->check();
		VMSlider->enable();
	}

	vSync = resources.gfx->load("CheckBox");
	vSync->setPosition(220, 130);
	vSync->setText("VSync");
	if (Options::get<bool>("vsync"))
		vSync->check();
	VidOpt->add(vSync);

	performanceOutput = resources.gfx->load("CheckBox");
	performanceOutput->setPosition(320, 130);
	performanceOutput->setText("Performance output");
	if (Options::get<bool>("performanceOutput"))
		performanceOutput->check();
	VidOpt->add(performanceOutput);

	tgui::Label::Ptr FdL = resources.gfx->load("Label");
	FdL->setPosition(80, 200);
	FdL->setText("Frame Rate");
	VidOpt->add(FdL);

	FrameDelay = resources.gfx->load("EditBox");
	FrameDelay->setPosition(100, 230);
	FrameDelay->setSize(70, 30);
	FrameDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	FrameDelay->setText(to_string(1000000/Options::get<sf::Time>("framedelay").asMicroseconds()));
	VidOpt->add(FrameDelay);

	tgui::Label::Ptr IdL = resources.gfx->load("Label");
	IdL->setPosition(320, 200);
	IdL->setText("Input Rate");
	VidOpt->add(IdL);

	InputDelay = resources.gfx->load("EditBox");
	InputDelay->setPosition(310, 230);
	InputDelay->setSize(110, 30);
	InputDelay->setInputValidator(tgui::EditBox::Validator::UInt);
	InputDelay->setText(to_string(1000000/Options::get<sf::Time>("inputdelay").asMicroseconds()));
	VidOpt->add(InputDelay);

	tgui::Button::Ptr AvB = resources.gfx->load("Button");
	AvB->setText("Apply");
	AvB->setPosition(240, 300);
	AvB->setOpacity(0.7);
	AvB->connect("pressed", &GameOptions::applyVideo, this);
	VidOpt->add(AvB);

	tgui::Label::Ptr InL = resources.gfx->load("Label");
	InL->setPosition(10, 390);
	InL->setText("Frame Rate = frames drawn per second\nInput Rate = input parsed per second\nNote: Enabling vSync will disable both FrameRate and InputRate.");
	VidOpt->add(InL);

	SndOpt = tgui::Panel::create(); // Sound Options
	SndOpt->setSize(560, 560);
	SndOpt->setPosition(0, 0);
	SndOpt->setBackgroundColor(sf::Color(255,255,255,0));
	SndOpt->hide();
	if (Options::get<bool>("noSound"))
		SndOpt->disable();
	panel->add(SndOpt);

	tgui::CheckBox::Ptr EsC = resources.gfx->load("CheckBox");
	EsC->setPosition(50, 0);
	EsC->setText("Sound Enabled");
	EsC->connect("Checked Unchecked", &GameOptions::sndChecked, this);
	SndOpt->add(EsC, "sndCheck");
	if (Options::get<bool>("sound"))
		EsC->check();

	tgui::Slider::Ptr MvS = resources.gfx->load("Slider");
	MvS->setPosition(50, 100);
	MvS->setSize(460,30);
	MvS->setMaximum(100);
	MvS->connect("ValueChanged", &GameOptions::volSlide, this, 1);
	MvS->setValue(Options::get<short>("musicvolume"));
	SndOpt->add(MvS);

	tgui::Label::Ptr MvL = resources.gfx->load("Label");
	MvL->setText("Music Volume");
	MvL->setPosition(240, 50);
	SndOpt->add(MvL);

	tgui::Slider::Ptr EvS = resources.gfx->load("Slider");
	EvS->setPosition(50, 200);
	EvS->setSize(460,30);
	EvS->setMaximum(100);
	EvS->connect("ValueChanged", &GameOptions::volSlide, this, 2);
	EvS->setValue(Options::get<short>("effectvolume"));
	SndOpt->add(EvS);

	tgui::Label::Ptr EvL = resources.gfx->load("Label");
	EvL->setText("Effect Volume");
	EvL->setPosition(240, 150);
	SndOpt->add(EvL);

	tgui::Slider::Ptr CvS = resources.gfx->load("Slider");
	CvS->setPosition(50, 300);
	CvS->setSize(460,30);
	CvS->setMaximum(100);
	CvS->connect("ValueChanged", &GameOptions::volSlide, this, 3);
	CvS->setValue(Options::get<short>("chatvolume"));
	SndOpt->add(CvS);

	tgui::Label::Ptr CvL = resources.gfx->load("Label");
	CvL->setText("Alert Volume");
	CvL->setPosition(240, 250);
	SndOpt->add(CvL);

	VisOpt = tgui::Panel::create(); // Sound Options
	VisOpt->setSize(560, 560);
	VisOpt->setPosition(0, 0);
	VisOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VisOpt->hide();
	panel->add(VisOpt);

	tgui::Label::Ptr NL = resources.gfx->load("Label");
	NL->setPosition(20, 13);
	NL->setText("Name (for single-player)");
	VisOpt->add(NL);
	tgui::EditBox::Ptr NTB = resources.gfx->load("EditBox");
	NTB->setText(Options::get<std::string>("name"));
	NTB->setPosition(230, 10);
	NTB->setSize(270, 30);
	NTB->connect("TextChanged", &GameOptions::changeName, this);
	VisOpt->add(NTB);

	tgui::CheckBox::Ptr animBack = resources.gfx->load("CheckBox");
	animBack->setPosition(20, 50);
	animBack->setText("Show background animation");
	animBack->connect("Checked", [&](){ EnableBackground(resources.delayClock.getElapsedTime()); });
	animBack->connect("Unchecked", [&](){ DisableBackground(); });
	if (Options::get<bool>("animatedBackground"))
		animBack->check();
	VisOpt->add(animBack);

	tgui::Label::Ptr gpaLabel = resources.gfx->load("Label");
	gpaLabel->setPosition(100, 90);
	gpaLabel->setText("Ghost piece transparency");
	VisOpt->add(gpaLabel);

	tgui::Slider::Ptr ghostPieceAlpha = resources.gfx->load("Slider");
	ghostPieceAlpha->setPosition(20, 120);
	ghostPieceAlpha->setSize(520,30);
	ghostPieceAlpha->setMaximum(255);
	ghostPieceAlpha->setValue(Options::get<uint8_t>("ghostPieceAlpha"));
	ghostPieceAlpha->connect("ValueChanged", &GameOptions::setGhostPieceAlpha, this);
	VisOpt->add(ghostPieceAlpha);

	tgui::RadioButton::Ptr r_lightTheme = resources.gfx->load("RadioButton");
	r_lightTheme->setPosition(20, 180);
	r_lightTheme->setText("Light theme");
	VisOpt->add(r_lightTheme);

	tgui::RadioButton::Ptr r_darkTheme = resources.gfx->load("RadioButton");
	r_darkTheme->setPosition(20, 220);
	r_darkTheme->setText("Dark theme");
	VisOpt->add(r_darkTheme);

	if (Options::get<uint8_t>("theme") == 1)
		r_lightTheme->check();
	else
		r_darkTheme->check();

	r_lightTheme->connect("Checked", [&](){ LightTheme(); });
	r_darkTheme->connect("Checked", [&](){ DarkTheme(); });

	tgui::Label::Ptr fbcLabel = resources.gfx->load("Label");
	fbcLabel->setPosition(100, 260);
	fbcLabel->setText("Game field background color and lines");
	VisOpt->add(fbcLabel);

	tgui::Slider::Ptr fieldBackColor = resources.gfx->load("Slider");
	fieldBackColor->setPosition(20, 290);
	fieldBackColor->setSize(520,30);
	fieldBackColor->setMaximum(255);
	fieldBackColor->setValue(Options::get<uint8_t>("fieldBackground"));
	fieldBackColor->connect("ValueChanged", [&](int val){
		Options::get<uint8_t>("fieldBackground")=val;
		SetGameBackColor(val);
		SetDrawMe();
		SetFieldsBackColor(val);
	});
	VisOpt->add(fieldBackColor);

	tgui::CheckBox::Ptr mouseMenu = resources.gfx->load("CheckBox");
	mouseMenu->setPosition(20, 500);
	mouseMenu->setText("Menu responds to mouse hovering");
	if (Options::get<bool>("mouseMenu"))
		mouseMenu->check();
	mouseMenu->connect("Checked", [&](){ Options::get<bool>("mouseMenu")=true; });
	mouseMenu->connect("Unchecked", [&](){ Options::get<bool>("mouseMenu")=false; });
	VisOpt->add(mouseMenu);

	connectSignal("ShowOptions", &GameOptions::show, this);
}

void GameOptions::show(int index) {
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
		if (Options::get<bool>("fullscreen"))
			Fullscreen->check();
		else
			Fullscreen->uncheck();
		if (Options::get<bool>("vsync"))
			vSync->check();
		else
			vSync->uncheck();
		FrameDelay->setText(to_string(1000000/Options::get<sf::Time>("framedelay").asMicroseconds()));
		InputDelay->setText(to_string(1000000/Options::get<sf::Time>("inputdelay").asMicroseconds()));
	}
	else if (index == 3)
		SndOpt->show();
}

void GameOptions::changeName(const std::string& name) {
	Options::get<std::string>("name") = name;
	//gui->game.field.text.setName(name);
}

void GameOptions::vidSlide(short i) {
	std::string name;
	name = to_string(Options::get_videomodes()[i].width) + "x" + to_string(Options::get_videomodes()[i].height);
	VideoMode->setText(name);
}

void GameOptions::fsChecked(bool i) {
	if (i)
		VMSlider->enable();
	else
		VMSlider->disable();
}

void GameOptions::sndChecked(bool i) {
	Options::get<bool>("sound") = i;
}

void GameOptions::applyVideo() {
	if (Fullscreen->isChecked()) {
		if (!Options::get<bool>("fullscreen") || Options::get<short>("currentmode") != VMSlider->getValue()) {
			Options::get<bool>("fullscreen")=true;
			Options::get<short>("currentmode") = VMSlider->getValue();
			resources.window.close();
			resources.window.create(Options::get_videomodes()[Options::get<short>("currentmode")], "SpeedBlocks", sf::Style::Fullscreen);
			resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		}
	}
	else if (Options::get<bool>("fullscreen")) {
		Options::get<bool>("fullscreen")=false;
		resources.window.close();
		resources.window.create(sf::VideoMode(960, 600), "SpeedBlocks");
		resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
	}

	if (vSync->isChecked()) {
		Options::get<bool>("vsync") = true;
		resources.window.setVerticalSyncEnabled(true);
	}
	else {
		Options::get<bool>("vsync") = false;
		resources.window.setVerticalSyncEnabled(false);
	}

	if (performanceOutput->isChecked()) {
		Options::get<bool>("performanceOutput") = true;
		Show(6);
	}
	else {
		Options::get<bool>("performanceOutput") = false;
		Hide(6);
	}

	std::string fd = FrameDelay->getText();
	short value=0;
	if (fd.size())
		value = stoi(fd);
	if (value)
		Options::get<sf::Time>("framedelay") = sf::microseconds(1000000/value);
	value=0;
	fd = InputDelay->getText();
	if (fd.size())
		value = stoi(fd);
	if (value)
		Options::get<sf::Time>("inputdelay") = sf::microseconds(1000000/value);
}

void GameOptions::volSlide(short i, short vol) {
	if (i == 1) {
		Options::get<short>("musicvolume") = vol;
		SetMusicVolume(vol);
	}
	else if (i == 2) {
		Options::get<short>("effectvolume") = vol;
		SetEffectVolume(vol);
	}
	else if (i == 3) {
		Options::get<short>("chatvolume") = vol;
		SetAlertsVolume(vol);
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

			if (event.key.code == Options::get<sf::Keyboard::Key>("left")) {
				Options::get<sf::Keyboard::Key>("left") = sf::Keyboard::Unknown;
				BindLeft->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("right")) {
				Options::get<sf::Keyboard::Key>("right") = sf::Keyboard::Unknown;
				BindRight->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("down")) {
				Options::get<sf::Keyboard::Key>("down") = sf::Keyboard::Unknown;
				BindDown->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("rcw")) {
				Options::get<sf::Keyboard::Key>("rcw") = sf::Keyboard::Unknown;
				BindRCW->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("rccw")) {
				Options::get<sf::Keyboard::Key>("rccw") = sf::Keyboard::Unknown;
				BindRCCW->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("r180")) {
				Options::get<sf::Keyboard::Key>("r180") = sf::Keyboard::Unknown;
				BindR180->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("hd")) {
				Options::get<sf::Keyboard::Key>("hd") = sf::Keyboard::Unknown;
				BindHD->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("menu")) {
				Options::get<sf::Keyboard::Key>("menu") = sf::Keyboard::Unknown;
				BindMenu->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("score")) {
				Options::get<sf::Keyboard::Key>("score") = sf::Keyboard::Unknown;
				BindScore->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("away")) {
				Options::get<sf::Keyboard::Key>("away") = sf::Keyboard::Unknown;
				BindAway->setText("");
			}

			if (event.key.code == Options::get<sf::Keyboard::Key>("ready")) {
				Options::get<sf::Keyboard::Key>("ready") = sf::Keyboard::Unknown;
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
	auto& basepiece = Options::get_basepieces();

	basepiece[i].setRotation(++basepiece[i].rotation);

	piecePreview[i]->m_sprite.setRotation(basepiece[i].rotation*90);
}

void GameOptions::colPiece(short i) {
	auto& basepiece = Options::get_basepieces();

	basepiece[i].setColor(++basepiece[i].tile);

	piecePreview[i]->m_sprite.setColor(pColor(basepiece[i].tile));
}

void GameOptions::initSprites() {
	sf::Texture tileT;
	sf::Sprite tile;
	tileT.loadFromFile(resourcePath() + "media/tile.png");
	tile.setTexture(tileT);

	sf::RenderTexture rendtex;
	rendtex.create(120,120);

	std::vector<short> value = Options::pieceArray();

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
		piecePreview[p] = tgui::TextureAndSprite::create();
		piecePreview[p]->m_texture = rendtex.getTexture();
		piecePreview[p]->m_sprite.setTexture(piecePreview[p]->m_texture);
		piecePreview[p]->m_sprite.setScale(0.5, 0.5);
		piecePreview[p]->m_sprite.setPosition(80*p+40, 330);
		piecePreview[p]->m_sprite.setColor(pColor(Options::get_basepieces()[p].tile));
		piecePreview[p]->m_sprite.setOrigin(60,60);
		piecePreview[p]->m_sprite.setRotation(Options::get<uint8_t>("piece_" + std::to_string(p) + "_rotation")*90);
		GenOpt->add(piecePreview[p]);
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

std::string SFKeyToString(unsigned int keycode) {
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
        return std::string{(char)(keycode+65)};
    else
    	return "";
    }
}

void GameOptions::setGhostPieceAlpha(uint8_t alpha) {
	Options::get<uint8_t>("ghostPieceAlpha") = alpha;
	resources.gfx->setGhostPieceAlpha(alpha);
	SetDrawMe();
}
