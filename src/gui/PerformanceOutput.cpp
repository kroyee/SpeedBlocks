#include "PerformanceOutput.h"
#include "Options.h"
#include "Resources.h"
using std::to_string;

void set_properties(Resources& resources, os::Label& label) {
    label->setAutoSize(false);
    label->getRenderer()->setFont(resources.gfx->font("print"));
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
}

PerformanceOutput::PerformanceOutput(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    pingIdCount = 0;
    pingReturned = false;

    longestFrame = sf::seconds(0);
    secCount = sf::seconds(0);
    frameCount = 0;
    frameRate = 0;

    longest.pos(0, 13).size(21, 15).text_size(12).center().add_to(panel)->getRenderer()->setTextColor(sf::Color::Black);
    set_properties(resources, longest);

    set_properties(resources, longestHeader);
    longestHeader.pos(0, 1).size(21, 13).text_size(10).center().add_to(panel).text("LFT");

    input.pos(23, 13).size(28, 15).text_size(12).center().add_to(panel)->getRenderer()->setTextColor(sf::Color::Black);
    set_properties(resources, input);

    set_properties(resources, inputHeader);
    inputHeader.pos(23, 1).size(28, 13).text_size(10).center().add_to(panel).text("IPS");

    frame.pos(53, 13).size(28, 15).text_size(12).center().add_to(panel)->getRenderer()->setTextColor(sf::Color::Black);
    set_properties(resources, frame);

    set_properties(resources, frameHeader);
    frameHeader.pos(53, 1).size(28, 13).text_size(10).center().add_to(panel).text("FPS");

    ping.pos(83, 13).size(28, 15).text_size(12).center().add_to(panel)->getRenderer()->setTextColor(sf::Color::Black);
    set_properties(resources, ping);

    set_properties(resources, pingHeader);
    pingHeader.pos(83, 1).size(28, 13).text_size(10).center().add_to(panel).text("MS");
}

void PerformanceOutput::update(sf::Time current, sf::Time lastFrame) {
    if (Options::get<bool>("performanceOutput")) {
        if (current - lastFrame > longestFrame) longestFrame = current - lastFrame;
        frameCount++;

        if (current - secCount > sf::seconds(1)) {
            setFrameRate(frameRate);
            setInputRate(frameCount);
            setLongestFrame(longestFrame);
            frameRate = 0;
            frameCount = 0;
            longestFrame = sf::seconds(0);
            secCount = current;
        }
    }
}

void PerformanceOutput::setFrameRate(int fr) {
    frame.text(to_string(fr));
    int frameRateColor;
    if (fr < 40)
        frameRateColor = 0;
    else if (fr < 100)
        frameRateColor = (fr - 40) * 4.25;
    else
        frameRateColor = 255;
    frame->getRenderer()->setBackgroundColor(sf::Color(255 - frameRateColor, frameRateColor, 0));
}

void PerformanceOutput::setInputRate(int fc) {
    input.text(to_string(fc));
    int inputRateColor;
    if (fc > 999)
        input.text_size(9);
    else
        input.text_size(12);
    if (fc < 255)
        inputRateColor = fc;
    else
        inputRateColor = 255;
    input->getRenderer()->setBackgroundColor(sf::Color(255 - inputRateColor, inputRateColor, 0));
}

void PerformanceOutput::setLongestFrame(sf::Time& lf) {
    longest.text(to_string(lf.asMilliseconds()));
    int longestFrameColor;
    if (lf.asMilliseconds() > 21)
        longestFrameColor = 0;
    else if (lf.asMilliseconds() > 4)
        longestFrameColor = 255 - (lf.asMilliseconds() - 4) * 15;
    else
        longestFrameColor = 255;
    longest->getRenderer()->setBackgroundColor(sf::Color(255 - longestFrameColor, longestFrameColor, 0));
}

void PerformanceOutput::setPing(int pingResult) {
    if (pingResult == -1) return;
    ping.text(to_string(pingResult));
    int pingColor;
    if (pingResult > 255)
        pingColor = 0;
    else
        pingColor = 255 - pingResult;
    ping->getRenderer()->setBackgroundColor(sf::Color(255 - pingColor, pingColor, 0));
}
