#include "GameFieldText.h"
#include "Resources.h"
#include "Textures.h"
#include <cmath>
#include <tuple>

TextObject::TextObject() {
	text.setCharacterSize(48);
	headerText.setCharacterSize(24);
}

TextObject& TextObject::set(const std::string& str) {
	visible=true;
	text.setString(str);
	update_origin();
	return *this;
}

TextObject& TextObject::set(int32_t val) {
	value = val;
	return set(std::to_string(val));
}

TextObject& TextObject::header(const std::string& str, uint16_t offset) {
	headerText.setString(str);
	useHeader = true;
	header_offset = offset;

	update_origin();

	auto size = headerText.getLocalBounds();
	auto pos = text.getPosition();
	pos.y -= size.height + header_offset;
	headerText.setPosition(pos);

	return *this;
}

TextObject& TextObject::size(uint8_t text_size, uint8_t header_size) {
	text.setCharacterSize(text_size);

	if (header_size)
		headerText.setCharacterSize(header_size);

	return *this;
}

TextObject& TextObject::setFont(const sf::Font& font) {
	text.setFont(font);
	headerText.setFont(font);
	return *this;
}

TextObject& TextObject::position(int x, int y) {
	text.setPosition(x, y);

	if (useHeader) {
		auto size = headerText.getLocalBounds();
		y -= size.height + header_offset;
		headerText.setPosition(x, y);
	}

	return *this;
}

TextObject& TextObject::color(const sf::Color& color) {
	text.setFillColor(color);
	headerText.setFillColor(color);

	return *this;
}

TextObject& TextObject::hide() {
	value = 0;
	visible = false;

	return *this;
}

TextObject& TextObject::show() {
	value = 1;
	visible = true;

	return *this;
}

int TextObject::getTextWidth() {
	return text.getLocalBounds().width;
}

int32_t TextObject::get() {
	return value;
}

void TextObject::draw(sf::RenderTexture& texture) {
	if (visible) {
		texture.draw(text);

		if (useHeader)
			texture.draw(headerText);
	}
}

void TextObject::update_origin() {
	auto text_origin = text.getLocalBounds().width / 2;
	if (std::abs(text.getOrigin().x - text_origin) > 5)
		text.setOrigin(text.getLocalBounds().width / 2, 0);

	auto header_origin = headerText.getLocalBounds().width / 2;
	if (std::abs(headerText.getOrigin().x - header_origin) > 5)
		headerText.setOrigin(headerText.getLocalBounds().width / 2, 0);
}

struct TextObjectInitializer {
	FieldText ft;
	int size;
	sf::Vector2f pos;
	std::string text, header;
	bool hide;
};

void initTextObjects(std::vector<TextObject>& vec, const std::vector<TextObjectInitializer>& data) {
	#ifdef DEBUG
		if (vec.size() != data.size())
			std::cout << "Size of TextObject vector does not match size of init-data" << std::endl;
	#endif

	for (unsigned i = 0; i < vec.size(); ++i) {
			auto& textObject = vec[static_cast<unsigned>(data[i].ft)];
			textObject.size(data[i].size, data[i].size/2);
			textObject.position(data[i].pos.x, data[i].pos.y);
			textObject.set(data[i].text);

			if (!data[i].header.empty())
				textObject.header(data[i].header);

			if (data[i].hide)
				textObject.hide();
	}
}

GameFieldText::GameFieldText(Resources& _resources, sf::RenderTexture& tex) : resources(_resources), texture(tex) {
	unsigned count = static_cast<unsigned>(FieldText::COUNT);
	text.resize(count);

	setFont(resources.gfx->font("print"));

	initTextObjects(text, {
		{FieldText::Name,		32, {150, 555},	"Player", "",	false},
		{FieldText::Away,		48, {150, 150}, "Away", "",		true},
		{FieldText::Position,	48, {150, 200}, "", "",			true},
		{FieldText::Ready,		32, {150, 75},	"Ready", "",	true},
		{FieldText::Combo,		28, {370, 330}, "0", "Combo",	false},
		{FieldText::Pending,	28, {370, 500}, "0", "Pending", false},
		{FieldText::Countdown,	96, {130, 210}, "", "",			true},
		{FieldText::BPM,		28, {370, 420}, "0", "BPM",		false},
		{FieldText::GameOver,	32, {150, 250},	"", "",			true}
	});

	text[static_cast<unsigned>(FieldText::Combo)].header("Combo", 50);

    comboTimer.setPosition(315, 290);
    comboTimer.setFillColor(sf::Color(255,0,0));
    setComboTimer(0);

    setColor(resources.gfx->color("GameFieldText"));
}

void GameFieldText::setFont(const sf::Font& font) {
	for (auto& t : text)
		t.setFont(font);
}

void GameFieldText::setName(const std::string& name_) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    name = name_;
	text[static_cast<unsigned>(FieldText::Name)].set(name);
}

void GameFieldText::setPosition(int8_t position) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	auto& positionText = text[static_cast<unsigned>(FieldText::Position)];
	if (position == 1)
        positionText.set("1st");
    else if (position == 2)
        positionText.set("2nd");
    else if (position == 3)
        positionText.set("3rd");
    else if (position)
        positionText.set(std::to_string((int)position) + "th");
	else
		positionText.hide();
}

const float PI = 3.14159265;
bool GameFieldText::setComboTimer(uint8_t count) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    if (count>100)
        count=100;
    if (comboTimer.getPointCount() == static_cast<unsigned int>(count+2))
        return false;
    comboTimer.setPointCount(count+2);

    comboTimer.setPoint(0, sf::Vector2f(60, 60));
    for (int x=1; x<(count+2); x++)
        comboTimer.setPoint(x, sf::Vector2f(60 + 60*cos((PI*2)/100 * (x-26)), 60 + 60*sin((PI*2)/100 * (x-26) )));

    return true;
}

void GameFieldText::setColor(sf::Color color) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);

	for (auto& t : text)
		t.color(color);
}

void GameFieldText::clear() {
	set<FieldText::Combo>(0);
	set<FieldText::Pending>(0);
	set<FieldText::BPM>(0);
	hide<FieldText::Position>();
	hide<FieldText::Countdown>();
	hide<FieldText::GameOver>();
	hide<FieldText::Ready>();
    setComboTimer(0);
}

void GameFieldText::drawText() {
    std::lock_guard<std::mutex> mute(fieldTextMutex);

	texture.draw(comboTimer);
	for (auto& t : text)
		t.draw(texture);
}
