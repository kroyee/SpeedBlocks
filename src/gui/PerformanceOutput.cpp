#include "PerformanceOutput.h"
#include "gui.h"
#include "Resources.h"
using std::to_string;

void PerformanceOutput::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);
	pingIdCount=0;
	pingReturned=false;

	longestFrame=sf::seconds(0);
	secCount=sf::seconds(0);
	frameCount=0;
	frameRate=0;

	longest = tgui::Label::create();
	longest->setAutoSize(false);
	longest->setPosition(0,13);
	longest->setSize(21,15);
	longest->setText("");
	longest->setTextSize(12);
	longest->setTextColor(sf::Color::Black);
	longest->setFont(gui->resources.gfx.printFont);
	longest->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	longest->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(longest);

	longestHeader = gui->themeTG->load("Label");
	longestHeader->setAutoSize(false);
	longestHeader->setPosition(0,1);
	longestHeader->setSize(21,13);
	longestHeader->setText("LFT");
	longestHeader->setTextSize(10);
	longestHeader->setFont(gui->resources.gfx.printFont);
	longestHeader->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	longestHeader->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(longestHeader);

	input = tgui::Label::create();
	input->setAutoSize(false);
	input->setPosition(23,13);
	input->setSize(28,15);
	input->setText("");
	input->setTextSize(12);
	input->setTextColor(sf::Color::Black);
	input->setFont(gui->resources.gfx.printFont);
	input->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	input->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(input);

	inputHeader = gui->themeTG->load("Label");
	inputHeader->setAutoSize(false);
	inputHeader->setPosition(23,1);
	inputHeader->setSize(28,13);
	inputHeader->setText("IPS");
	inputHeader->setTextSize(10);
	inputHeader->setFont(gui->resources.gfx.printFont);
	inputHeader->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	inputHeader->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(inputHeader);

	frame = tgui::Label::create();
	frame->setAutoSize(false);
	frame->setPosition(53,13);
	frame->setSize(28,15);
	frame->setText("");
	frame->setTextSize(12);
	frame->setFont(gui->resources.gfx.printFont);
	frame->setTextColor(sf::Color::Black);
	frame->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	frame->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(frame);

	frameHeader = gui->themeTG->load("Label");
	frameHeader->setAutoSize(false);
	frameHeader->setPosition(53,1);
	frameHeader->setSize(28,13);
	frameHeader->setText("FPS");
	frameHeader->setTextSize(10);
	frameHeader->setFont(gui->resources.gfx.printFont);
	frameHeader->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	frameHeader->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(frameHeader);

	ping = tgui::Label::create();
	ping->setAutoSize(false);
	ping->setPosition(83,13);
	ping->setSize(28,15);
	ping->setText("");
	ping->setTextSize(12);
	ping->setTextColor(sf::Color::Black);
	ping->setFont(gui->resources.gfx.printFont);
	ping->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	ping->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(ping);

	pingHeader = gui->themeTG->load("Label");
	pingHeader->setAutoSize(false);
	pingHeader->setPosition(83,1);
	pingHeader->setSize(28,13);
	pingHeader->setText("MS");
	pingHeader->setTextSize(10);
	pingHeader->setFont(gui->resources.gfx.printFont);
	pingHeader->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	pingHeader->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	panel->add(pingHeader);
}

void PerformanceOutput::update(sf::Time current, sf::Time lastFrame) {
	if (gui->options.performanceOutput) {
        if (current-lastFrame > longestFrame)
            longestFrame = current-lastFrame;
        frameCount++;

        if (current-secCount > sf::seconds(1)) {
            setFrameRate(frameRate);
            setInputRate(frameCount);
            setLongestFrame(longestFrame);
            frameRate=0;
            frameCount=0;
            longestFrame=sf::seconds(0);
            secCount=current;
        }
    }
}

void PerformanceOutput::setFrameRate(int fr) {
	frame->setText(to_string(fr));
	int frameRateColor;
    if (fr < 40)
        frameRateColor = 0;
    else if (fr < 100)
        frameRateColor = (fr-40)*4.25;
    else
    	frameRateColor = 255;
    frame->getRenderer()->setBackgroundColor(sf::Color(255-frameRateColor, frameRateColor, 0));
}

void PerformanceOutput::setInputRate(int fc) {
	input->setText(to_string(fc));
	int inputRateColor;
    if (fc > 999)
        input->setTextSize(9);
    else
        input->setTextSize(12);
    if (fc < 255)
        inputRateColor = fc;
    else
    	inputRateColor = 255;
    input->getRenderer()->setBackgroundColor(sf::Color(255-inputRateColor, inputRateColor, 0));
}

void PerformanceOutput::setLongestFrame(sf::Time& lf) {
	longest->setText(to_string(lf.asMilliseconds()));
	int longestFrameColor;
    if (lf.asMilliseconds() > 21)
        longestFrameColor = 0;
    else if (lf.asMilliseconds() > 4)
        longestFrameColor = 255 - (lf.asMilliseconds() - 4) * 15;
    else
    	longestFrameColor = 255;
    longest->getRenderer()->setBackgroundColor(sf::Color(255-longestFrameColor, longestFrameColor, 0));
}

void PerformanceOutput::setPing(int pingResult) {
	ping->setText(to_string(pingResult));
	int pingColor;
	if (pingResult > 255)
		pingColor = 0;
	else
		pingColor = 255-pingResult;
	ping->getRenderer()->setBackgroundColor(sf::Color(255-pingColor, pingColor, 0));
}