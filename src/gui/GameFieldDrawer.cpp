#include "GameFieldDrawer.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Packets.hpp"
#include "Options.h"
#include "Resources.h"

GameFieldDrawer::GameFieldDrawer(Resources& _res) : resources(_res), scaleup(nullptr) {
    setPosition(465, 40);
    setSize(450, 555);

    connectSignal("ShowGameFields", &GameFieldDrawer::show, this);
    connectSignal("HideGameFields", &GameFieldDrawer::hide, this);
    connectSignal("SetFieldsBackColor", &GameFieldDrawer::setBackgroundColor, this);
    connectSignal("AreThereFields", &GameFieldDrawer::areThereFields, this);
    connectSignal("SetFieldsSize", &GameFieldDrawer::setSize, this);
    connectSignal("GameFieldsIsVisible", &GameFieldDrawer::isVisible, this);
    connectSignal("AddField", &GameFieldDrawer::addField, this);
    connectSignal("RemoveField", &GameFieldDrawer::removeField, this);
    connectSignal("RemoveAllFields", &GameFieldDrawer::removeAllFields, this);

    PM::handle_packet([&](const NP_ClientAway& p) {
        for (auto&& field : fields)
            if (field.id == p.id) {
                field.text.show<FieldText::Away>();
                field.drawMe = true;
                return;
            }
    });
    PM::handle_packet([&](const NP_ClientNotAway& p) {
        for (auto&& field : fields)
            if (field.id == p.id) {
                field.text.hide<FieldText::Away>();
                field.drawMe = true;
                return;
            }
    });
    PM::handle_packet([&](const NP_PlayerPosition& p) {
        for (auto&& field : fields)
            if (field.id == p.id) {
                field.text.set<FieldText::Position>(p.position);
                field.drawMe = true;
                return;
            }
    });
    PM::handle_packet([&](const NP_ClientReady& p) {
        for (auto&& field : fields)
            if (field.id == p.id) {
                field.text.show<FieldText::Ready>();
                field.drawMe = true;
                return;
            }
    });
    PM::handle_packet([&](const NP_ClientNotReady& p) {
        for (auto&& field : fields)
            if (field.id == p.id) {
                field.text.hide<FieldText::Ready>();
                field.drawMe = true;
                return;
            }
    });
    PM::handle_packet([&](const NP_AvgBpm& p) {
        for (auto& field : fields)
            if (field.id == p.id) {
                field.text.set<FieldText::BPM>(p.average_bpm);
                field.drawMe = true;
            }
    });
}

void GameFieldDrawer::setPosition(short x, short y) {
    xPos = x;
    yPos = y;
    calFieldPos();
}

void GameFieldDrawer::setSize(int w, int h) {
    width = w;
    height = h;
    calFieldPos();
}

ObsField& GameFieldDrawer::addField(int id, const std::string& name) {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    if (unusedFields.empty())
        fields.emplace_back(resources);
    else
        fields.splice(fields.end(), unusedFields, unusedFields.begin());

    queueIt = fields.begin();

    ObsField& field = fields.back();
    field.clear();
    field.id = id;
    field.text.setName(name);
    if (Options::get<uint8_t>("theme") == 2) field.text.setColor(sf::Color(255, 255, 255));
    calFieldPos();
    field.texture.setActive(false);
    field.drawMe = true;
    field.datacount = 0;

    return field;
}

void GameFieldDrawer::removeField(int id) {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    for (auto it = fields.begin(); it != fields.end(); it++)
        if (it->id == id) {
            it->text.hide<FieldText::Away>();
            unusedFields.splice(unusedFields.end(), fields, it);
            queueIt = fields.begin();
            break;
        }
    calFieldPos();
}

void GameFieldDrawer::removeAllFields() {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    for (auto& field : fields) field.text.hide<FieldText::Away>();
    unusedFields.splice(unusedFields.end(), fields);
    queueIt = fields.end();
}

void GameFieldDrawer::updateFields() {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    for (auto& field : fields)
        if (field.status == 1) field.drawField();
}

bool GameFieldDrawer::drawNextField() {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    if (queueIt == fields.end()) {
        queueIt = fields.begin();
        return false;
    }

    queueIt->drawField();
    queueIt++;

    return true;
}

void GameFieldDrawer::drawScaleup() {}

void GameFieldDrawer::calFieldPos() {
    float r = 600 / 440.0;
    short total = fields.size(), placed = 0;

    short field_w = width;
    short field_h = r * field_w;
    if (field_h > height) {
        field_h = height;
        field_w = field_h / r;
    }

    bool done = false;
    short x, y, rows;

    while (!done) {
        x = 0;
        y = 0;
        rows = 1;
        for (placed = 0; placed < total; placed++) {
            if (x + field_w > width) {
                x = 0;
                y += field_h;
                rows++;
            }
            if (y + field_h > height) break;
            x += field_w;
        }
        if (placed >= total)
            done = true;
        else {
            field_w--;
            field_h = field_w * r;
        }
    }

    short perrow = (float)total / rows + 0.999999;

    r = field_w / 440.0;
    short onrow = 0, currow = 0;

    short(*origin)[2] = new short[total][2];

    short c;
    for (c = 0; c < total; c++) {
        origin[c][0] = xPos + (onrow + 1) * ((width - field_w * perrow) / (perrow + 1)) + onrow * field_w;
        origin[c][1] = yPos + (currow + 1) * ((height - field_h * rows) / (rows + 1)) + field_h * currow;
        onrow++;
        if (onrow == perrow) {
            onrow = 0;
            currow++;
        }
    }

    c = 0;
    onrow = 0;
    currow = 0;
    for (auto&& it : fields) {  // 440 is the width of a playfield in full size
        float xpos = onrow * field_w + field_w / 2.0;
        float xposrat = xpos / width;
        xpos = (width - 440) * xposrat + xPos;

        short ypos = currow * field_h;
        float yposrat = (float)ypos / ((float)height / 2.0);
        ypos = yposrat * yPos - yPos;

        short originX = (xpos - origin[c][0]) / (r - 1);
        short originY = ((ypos + origin[c][1]) / (r - 1)) * -1;

        it.sprite.setOrigin(originX, originY);
        it.sprite.setPosition(xpos + originX, originY - ypos);
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
        field.datacount = 250;
        field.clear();
        drawOppField(field);
    }
}

void GameFieldDrawer::drawOppField(ObsField& field) { field.drawField(); }

void GameFieldDrawer::drawFields() {
    std::lock_guard<std::mutex> mute(fieldsMutex);
    for (auto&& field : fields) resources.window.draw(field.sprite);
    if (scaleup) {
        scaleup->scale += sclock.restart().asMilliseconds() / 5.0;
        if (scaleup->scale > 100) scaleup->scale = 100;
        float maxRat = 0.85, add;
        add = (maxRat - currentR) / 100;
        if (add > 0) {
            scaleup->sprite.setScale(currentR + add * scaleup->scale, currentR + add * scaleup->scale);
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
                scaleup->scale = 0;
                scaleup->sprite.setScale(currentR, currentR);
                scaleup = nullptr;
            }
        }
        for (auto&& it : fields) {
            box = it.sprite.getGlobalBounds();
            if (box.contains(pos)) {
                if (&it != scaleup) {
                    if (scaleup) {
                        scaleup->scale = 0;
                        scaleup->sprite.setScale(currentR, currentR);
                    }
                    scaleup = &it;
                    scaleup->scale = 0;
                    sclock.restart();
                    break;
                }
            }
        }
    } else if (event.type == sf::Event::MouseLeft && scaleup) {
        scaleup->scale = 0;
        scaleup->sprite.setScale(currentR, currentR);
        scaleup = nullptr;
    }
}

void GameFieldDrawer::setBackgroundColor(int val) {
    for (auto& field : fields) field.setBackColor(val);
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
