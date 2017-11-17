#include "Resources.h"
#include "GameFieldDrawer.h"
#include "optionSet.h"
#include "GameSignals.h"
#include <iostream>
using std::cout;
using std::endl;

GameFieldDrawer::GameFieldDrawer(Resources& _res) : resources(_res), scaleup(nullptr) {
	setPosition(465, 40);
	setSize(450, 555);

	Signals::ShowGameFields.connect(&GameFieldDrawer::show, this);
	Signals::HideGameFields.connect(&GameFieldDrawer::hide, this);
	Signals::SetFieldsBackColor.connect(&GameFieldDrawer::setBackgroundColor, this);
	Signals::AreThereFields.connect(&GameFieldDrawer::areThereFields, this);
	Signals::SetFieldsSize.connect(&GameFieldDrawer::setSize, this);
	Signals::GameFieldsIsVisible.connect(&GameFieldDrawer::isVisible, this);
	Signals::AddField.connect(&GameFieldDrawer::addField, this);
	Signals::RemoveField.connect(&GameFieldDrawer::removeField, this);
	Signals::RemoveAllFields.connect(&GameFieldDrawer::removeAllFields, this);

	Net::takeSignal(11, [&](sf::Uint16 id1){
		for (auto&& field : fields)
			if (field.id == id1) {
				field.text.away=true;
				field.drawMe=true;
				return;
			}
	});
	Net::takeSignal(12, [&](sf::Uint16 id1){
		for (auto&& field : fields)
			if (field.id == id1) {
				field.text.away=false;
				field.drawMe=true;
				return;
			}
	});
	Net::takeSignal(13, [&](sf::Uint16 id1, sf::Uint16 id2){
		for (auto&& field : fields)
			if (field.id == id1) {
				field.text.setPosition(id2);
				field.drawMe=true;
				return;
			}
	});
	Net::takeSignal(15, [&](sf::Uint16 id1){
		for (auto&& field : fields)
			if (field.id == id1) {
				field.text.ready=true;
				field.drawMe=true;
				return;
			}
	});
	Net::takeSignal(16, [&](sf::Uint16 id1){
		for (auto&& field : fields)
			if (field.id == id1) {
				field.text.ready=false;
				field.drawMe=true;
				return;
			}
	});
}

void GameFieldDrawer::setPosition(short x, short y) { xPos = x; yPos = y; calFieldPos(); }

void GameFieldDrawer::setSize(int w, int h) { width = w; height = h; calFieldPos(); }

obsField& GameFieldDrawer::addField(int id, const sf::String& name) {
	std::lock_guard<std::mutex> mute(fieldsMutex);
	if (unusedFields.empty())
		fields.emplace_back(resources);
	else
		fields.splice(fields.end(), unusedFields, unusedFields.begin());

	fields.back().clear();
	fields.back().id = id;
	fields.back().text.setName(name);
	if (resources.options->theme == 2)
		fields.back().text.setColor(sf::Color(255,255,255));
	calFieldPos();
	fields.back().texture.setActive(false);
	fields.back().launchDrawThread();
	fields.back().drawMe=true;

	return fields.back();
}

void GameFieldDrawer::removeField(int id) {
	std::lock_guard<std::mutex> mute(fieldsMutex);
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == id) {
			it->text.away=false;
			it->status = 5;
			if (it->drawThread.joinable())
				it->drawThread.join();
			unusedFields.splice(unusedFields.end(), fields, it);
			break;
		}
	calFieldPos();
}

void GameFieldDrawer::removeAllFields() {
	std::lock_guard<std::mutex> mute(fieldsMutex);
	for (auto& field : fields) {
		field.text.away=false;
		field.status = 5;
		if (field.drawThread.joinable())
			field.drawThread.join();
	}
	unusedFields.splice(unusedFields.end(), fields);
}

void GameFieldDrawer::updateField(obsField& field) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == field.id) {
			for (int y=0; y<22; y++)
				for (int x=0; x<10; x++)
					it->square[y][x] = field.square[y][x];
			it->nextpiece = field.nextpiece;
			it->nprot = field.nprot;
			it->npcol = field.npcol;
			drawOppField(*it);
		}
}

void GameFieldDrawer::calFieldPos() {
	float r = 600/440.0;
	short total = fields.size(), placed = 0;

	short field_w = width;
	short field_h = r*field_w;
	if (field_h>height) {
		field_h=height;
		field_w=field_h/r;
	}

	bool done = false;
	short x, y, rows;

	while (!done) {
		x=0; y=0; rows=1;
		for (placed = 0; placed < total; placed++) {
			if (x+field_w > width) { x=0; y+=field_h; rows++; }
			if (y+field_h > height)
				break;
			x+=field_w;
		}
		if (placed >= total)
			done=true;
		else {
			field_w--;
			field_h=field_w*r;
		}
	}

	short perrow = (float)total/rows + 0.999999;

	r = field_w/440.0;
	short onrow=0, currow=0;

	short(*origin)[2] = new short[total][2];

	short c;
	for (c=0; c<total; c++) {
		origin[c][0] = xPos + (onrow+1)*((width-field_w*perrow) / (perrow+1)) + onrow * field_w;
		origin[c][1] = yPos + (currow+1)*((height-field_h*rows) / (rows+1)) + field_h*currow;
		onrow++;
		if (onrow==perrow) {
			onrow=0;
			currow++;
		}
	}

	c=0; onrow=0; currow=0;
	for (auto &&it : fields) { //440 is the width of a playfield in full size
		float xpos = onrow*field_w + field_w/2.0;
		float xposrat = xpos / width;
		xpos = (width - 440)*xposrat + xPos;

		short ypos = currow*field_h;
		float yposrat = (float)ypos / ((float)height / 2.0);
		ypos = yposrat*yPos-yPos;

		short originX = (xpos-origin[c][0]) / (r-1);
		short originY = ((ypos+origin[c][1]) / (r-1))*-1;

		it.sprite.setOrigin(originX, originY);
		it.sprite.setPosition(xpos+originX, originY-ypos);
		it.sprite.setScale(r, r);
		c++;
		onrow++;
		if (onrow == perrow) {
			onrow = 0;
			currow++;
		}
	}

	delete[] origin;
	origin = nullptr;

	currentR = r;
}

void GameFieldDrawer::resetOppFields() {
	for (auto&& field : fields) {
		field.datacount=250;
		field.clear();
		drawOppField(field);
	}
}

void GameFieldDrawer::drawOppField(obsField& field) {
	field.drawField();
}

void GameFieldDrawer::drawFields() {
	std::lock_guard<std::mutex> mute(fieldsMutex);
	for (auto&& field : fields)
		resources.window.draw(field.sprite);
	if (scaleup) {
		scaleup->scale += sclock.restart().asMilliseconds() / 5.0;
		if (scaleup->scale > 100)
			scaleup->scale = 100;
		float maxRat = 0.85, add;
		add = (maxRat-currentR)/100;
		if (add>0) {
			scaleup->sprite.setScale(currentR+add*scaleup->scale, currentR+add*scaleup->scale);
			sf::FloatRect pos = scaleup->sprite.getGlobalBounds();
			sf::RectangleShape blackback;
			blackback.setPosition(pos.left, pos.top);
			blackback.setSize({pos.width, pos.height});
			blackback.setFillColor(sf::Color::Black);
			resources.window.draw(blackback);
			resources.window.draw(scaleup->sprite);
		}
	}
}

void GameFieldDrawer::enlargePlayfield(sf::Event& event) {
	if (event.type == sf::Event::MouseMoved) {
		sf::Vector2f pos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
		sf::FloatRect box;
		if (scaleup) {
			box = scaleup->sprite.getGlobalBounds();
			if (!box.contains(pos)) {
				scaleup->scale=0;
				scaleup->sprite.setScale(currentR, currentR);
				scaleup=0;
			}
		}
		for (auto &&it : fields) {
			box = it.sprite.getGlobalBounds();
			if (box.contains(pos)) {
				if (&it != scaleup) {
					if (scaleup) {
						scaleup->scale=0;
						scaleup->sprite.setScale(currentR, currentR);
					}
					scaleup=&it;
					scaleup->scale=0;
					sclock.restart();
					break;
				}
			}
		}
	}
	else if (event.type == sf::Event::MouseLeft && scaleup) {
		scaleup->scale=0;
		scaleup->sprite.setScale(currentR, currentR);
		scaleup=0;
	}
}

void GameFieldDrawer::setBackgroundColor(int val) {
	for (auto& field : fields)
		field.setBackColor(val);
}

int GameFieldDrawer::areThereFields(int type) {
	if (type) {
		if (fields.size())
			return fields.front().id;
		else
			return 0;
	}
	
	return fields.size();
}