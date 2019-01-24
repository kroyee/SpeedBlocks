#include "GameField.h"
#include <SFML/Graphics.hpp>
#include "GameSignals.h"
#include "Options.h"
#include "Resources.h"
#include "Textures.h"

static constexpr int BASE_TILE_SIZE = 30;
static constexpr float TILE_SCALE = 0.90f;

static constexpr float TILE_SIZE = BASE_TILE_SIZE * TILE_SCALE;

static constexpr int X_POS = 5, Y_POS = 60, FIELD_ROWS = 18, FIELD_COLUMNS = 10, BORDER = 5;
static constexpr int WIDTH = FIELD_COLUMNS * TILE_SIZE, HEIGHT = FIELD_ROWS * TILE_SIZE;

BasicField::BasicField(Resources& _resources) : resources(_resources) {}

bool BasicField::possible() {
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            if (piece.grid[y][x]) {
                if (piece.posX + x < 0 || piece.posX + x > 9 || piece.posY + y < 0 || piece.posY + y > 21) return false;
                if (square[piece.posY + y][piece.posX + x]) return false;
            }
    return true;
}

bool BasicField::mRight() {
    piece.mright();
    if (possible()) return true;

    piece.mleft();
    return false;
}

bool BasicField::mLeft() {
    piece.mleft();
    if (possible()) return true;

    piece.mright();
    return false;
}

bool BasicField::mDown() {
    piece.mdown();
    if (possible()) return true;

    piece.mup();
    return false;
}

void BasicField::hd() {
    do {
        piece.mdown();
    } while (possible());
    piece.mup();
}

bool BasicField::rcw() {
    piece.rcw();
    if (possible()) return true;
    if (kickTest()) return true;

    piece.posX -= 2;
    piece.rccw();
    return false;
}

bool BasicField::rccw() {
    piece.rccw();
    if (possible()) return true;
    if (kickTest()) return true;

    piece.posX -= 2;
    piece.rcw();
    return false;
}

bool BasicField::r180() {
    piece.rccw();
    piece.rccw();
    if (possible()) return true;
    if (kickTest()) return true;

    piece.posX -= 2;
    piece.rcw();
    piece.rcw();
    return false;
}

bool BasicField::kickTest() {
    piece.posY++;
    if (possible()) return true;
    piece.posY--;
    piece.posX--;
    if (possible()) return true;
    piece.posX += 2;
    if (possible()) return true;
    piece.posY++;
    piece.posX -= 2;
    if (possible()) return true;
    piece.posX += 2;
    if (possible()) return true;
    piece.posX -= 3;
    piece.posY--;
    if (possible()) return true;
    piece.posX += 4;
    if (possible()) return true;

    return false;
}

void BasicField::addPiece() {
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            if (piece.grid[y][x]) square[piece.posY + y][piece.posX + x] = piece.tile;
}

void BasicField::removeline(short y) {
    for (; y > -1; y--)
        for (int x = 0; x < 10; x++) square[y][x] = square[y - 1][x];
    for (int x = 0; x < 10; x++) square[0][x] = 0;
}

sf::Vector2i BasicField::clearlines() {
    sf::Vector2i linescleared(0, 0);
    bool rm, gb;
    for (int y = 21; y > -1; y--) {
        if (piece.posY + y > 21 || piece.posY + y < 0) continue;
        rm = 1;
        gb = 0;
        for (int x = 0; x < 10; x++) {
            if (square[piece.posY + y][x] == 8) gb = 1;
            if (square[piece.posY + y][x] == 0) {
                rm = 0;
                break;
            }
        }
        if (rm) {
            removeline(piece.posY + y);
            y++;
            linescleared.x++;
            if (gb) linescleared.y++;
        }
    }
    return linescleared;
}

GameField::GameField(Resources& _resources) : BasicField(_resources), tile(resources.gfx->tileSet()), text(_resources, texture) {
    texture.create(440, 600);
    sprite.setTexture(texture.getTexture());
    backRect.setPosition(X_POS, Y_POS);
    backRect.setSize({WIDTH, HEIGHT});
    setBackColor(Options::get<uint8_t>("fieldBackground"));

    piece.piece = 7;
    pieceCopy.piece = 7;
    offset = 0;
    status = 0;

    for (auto& s : tile) s.setScale(TILE_SCALE, TILE_SCALE);

    for (int y = 0; y < 22; y++)
        for (int x = 0; x < 10; x++) {
            square[y][x] = 0;
            squareCopy[y][x] = 0;
        }
}

void GameField::clear() {
    for (int y = 0; y < 22; y++)
        for (int x = 0; x < 10; x++) {
            square[y][x] = 0;
            squareCopy[y][x] = 0;
        }
    text.clear();
    piece.piece = 7;
    pieceCopy.piece = 7;
    offset = 0;
}

bool GameField::possibleCopy() {
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            if (pieceCopy.grid[y][x]) {
                if (pieceCopy.posX + x < 0 || pieceCopy.posX + x > 9 || pieceCopy.posY + y < 0 || pieceCopy.posY + y > 21) return false;
                if (square[pieceCopy.posY + y][pieceCopy.posX + x]) return false;
            }
    return true;
}

void GameField::drawField() {
    texture.clear(sf::Color(255, 255, 255, 0));
    texture.draw(backRect);
    drawEdges();
    drawSquares();
    drawPiece();
    drawGhostPiece();
    text.drawText();
    status = 0;
}

void GameField::drawEdges() {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color(127, 127, 127, 200));

    rect.setPosition(X_POS - BORDER, Y_POS - BORDER);
    rect.setSize({WIDTH + BORDER * 2, BORDER});
    texture.draw(rect);

    rect.setPosition(X_POS - BORDER, Y_POS + HEIGHT);
    texture.draw(rect);

    rect.setPosition(X_POS - BORDER, Y_POS);
    rect.setSize({BORDER, HEIGHT});
    texture.draw(rect);

    rect.setPosition(X_POS + WIDTH, Y_POS);
    texture.draw(rect);
}

void GameField::drawTile(uint8_t color, uint8_t x, uint8_t y) {
    if (y < 3 || color == 0) return;
    color--;

    if (y == 3) {
        if (!offset) return;
        tile[color].setTextureRect({0, BASE_TILE_SIZE - base_offset, BASE_TILE_SIZE, base_offset});
        tile[color].setPosition(sf::Vector2f(X_POS + x * TILE_SIZE, Y_POS));
        texture.draw(tile[color]);
        tile[color].setTextureRect({0, 0, BASE_TILE_SIZE, BASE_TILE_SIZE});
    } else if (y == 21 && offset) {
        tile[color].setTextureRect({0, 0, BASE_TILE_SIZE, BASE_TILE_SIZE - base_offset});
        tile[color].setPosition(sf::Vector2f(X_POS + x * TILE_SIZE, Y_POS + HEIGHT - TILE_SIZE + offset));
        texture.draw(tile[color]);
        tile[color].setTextureRect({0, 0, BASE_TILE_SIZE, BASE_TILE_SIZE});
    } else {
        tile[color].setPosition(sf::Vector2f(X_POS + x * TILE_SIZE, Y_POS + (y - 4) * TILE_SIZE + offset));
        texture.draw(tile[color]);
    }
}

void GameField::drawSquares() {
    for (int y = 3; y < 22; y++)
        for (int x = 0; x < 10; x++) drawTile(squareCopy[y][x], x, y);
}

void GameField::drawPiece() {
    if (pieceCopy.piece == 7) return;
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (pieceCopy.grid[y][x] != 0) drawTile(pieceCopy.tile, pieceCopy.posX + x, pieceCopy.posY + y);
}

void GameField::drawGhostPiece() {
    if (pieceCopy.piece == 7) return;

    static bool& ghostpiece = Options::get<bool>("ghostpiece");
    if (ghostpiece) {
        short posY = pieceCopy.posY;
        while (possibleCopy()) {
            pieceCopy.mdown();
        }
        pieceCopy.mup();
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++)
                if (pieceCopy.grid[y][x] != 0) drawTile(pieceCopy.tile + 8, pieceCopy.posX + x, pieceCopy.posY + y);

        pieceCopy.posY = posY;
    }
}

void GameField::drawPiecePreview(const PiecePreviewInfo& data, sf::Vector2f pos) {
    auto ppiece = Options::get_basepieces()[data.piece];
    sf::Sprite& ptile = tile[data.color - 1];
    ptile.setScale(data.scale, data.scale);
    float tile_offset = 30 * data.scale;

    if (ppiece.lpiece) pos += sf::Vector2f{tile_offset / 2.f, tile_offset / 2.f};

    while (ppiece.current_rotation != data.rotation) ppiece.rcw();

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (ppiece.grid[y][x]) {
                ptile.setPosition(sf::Vector2f(pos.x + x * tile_offset, pos.y + y * tile_offset));
                texture.draw(ptile);
            }

    ptile.setScale(TILE_SCALE, TILE_SCALE);
}

void GameField::setBackColor(uint8_t val) { backRect.setFillColor(sf::Color(val, val, val, 225)); }

bool GameField::setOffset(uint8_t val) {
    base_offset = val;
    return (offset = val * TILE_SCALE);
}

void ObsField::makeDrawCopy() {
    makeNextPieceCopy();
    squareCopy = square;
    pieceCopy = piece;
    drawMe = false;
    status = 1;
}

void ObsField::drawField() {
    texture.clear(sf::Color(255, 255, 255, 0));
    texture.draw(backRect);

    drawEdges();
    drawSquares();
    drawPiece();
    drawGhostPiece();
    drawNextPiece();
    text.drawText();
    texture.display();
}

void ObsField::drawNextPiece() { drawPiecePreview({npPiece.piece, npPiece.rotation, npPiece.tile}, {330, 45}); }

void ObsField::updatePiece() {
    static std::array<basePieces, 7>& basepiece = Options::get_basepieces();
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++) {
            if (basepiece[piece.piece].grid[y][x])
                piece.grid[y][x] = piece.tile;
            else
                piece.grid[y][x] = 0;
        }
    piece.lpiece = basepiece[piece.piece].lpiece;
    piece.current_rotation = 0;
    while (piece.current_rotation != piece.rotation) piece.rcw();
}

void ObsField::makeNextPieceCopy() {
    npPiece.piece = nextpiece;
    npPiece.tile = npcol;
    npPiece.rotation = nprot;
}

ObsField::ObsField(Resources& _resources) : GameField(_resources) {
    id = 0;
    nextpiece = 0;
    nprot = 0;
    scale = 0;
    npcol = 1;
    mouseover = 0;
    piece.posX = 0;
    piece.posY = 0;
    npPiece.piece = 7;
}
