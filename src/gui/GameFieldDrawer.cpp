#include "gui.h"
#include "GameFieldDrawer.h"
#include "gameField.h"
#include "GameplayUI.h"
#include <iostream>
using std::cout;
using std::endl;

GameFieldDrawer::GameFieldDrawer(UI& _gui) : gui(_gui), scaleup(nullptr) {
	setPosition(465, 40);
	setSize(490, 555);
}

void GameFieldDrawer::setPosition(short x, short y) { xPos = x; yPos = y; calFieldPos(); }

void GameFieldDrawer::setSize(short w, short h) { width = w; height = h; calFieldPos(); }

void GameFieldDrawer::addField(obsField& field) {
	fields.push_back(field);
	calFieldPos();
	drawOppField(fields.back());
}

void GameFieldDrawer::removeField(sf::Uint16 id) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == id) {
			it = fields.erase(it);
			break;
		}
	calFieldPos();
}

void GameFieldDrawer::removeAllFields() {
	fields.clear();
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

	field.drawNextPiece();
}

void GameFieldDrawer::drawFields() {
	for (auto&& field : fields)
		gui.window->draw(field.sprite);
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
			gui.window->draw(blackback);
			gui.window->draw(scaleup->sprite);
		}
	}
}

void GameFieldDrawer::enlargePlayfield(sf::Event& event) {
	if (gui.gameplayUI->GameFields->isVisible()) {
		if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f pos = gui.window->mapPixelToCoords(sf::Mouse::getPosition(*gui.window));
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
}