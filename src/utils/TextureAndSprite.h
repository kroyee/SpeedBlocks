#ifndef TEXTUREANDSPRITE_H
#define TEXTUREANDSPRITE_H

#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

namespace tgui {
class TextureAndSprite : public ClickableWidget {
   public:
    typedef std::shared_ptr<TextureAndSprite> Ptr;             ///< Shared widget pointer
    typedef std::shared_ptr<const TextureAndSprite> ConstPtr;  ///< Shared constant widget pointer

    TextureAndSprite(const Layout2d& size = {100, 100});

    TextureAndSprite(const Layout& width, const Layout& height);

    TextureAndSprite(const TextureAndSprite& copy);

    TextureAndSprite& operator=(const TextureAndSprite& right);

    static TextureAndSprite::Ptr create(Layout2d size = {100, 100});

    static TextureAndSprite::Ptr copy(TextureAndSprite::ConstPtr canvas);

    virtual void setPosition(const Layout2d& position) override;
    // using Transformable::setPosition;

    virtual void setSize(const Layout2d& size) override;
    // using Transformable::setSize;

    void setOpacity(float opacity);

   protected:
    virtual Widget::Ptr clone() const override { return std::make_shared<TextureAndSprite>(*this); }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

   public:
    sf::Texture m_texture;
    sf::Sprite m_sprite;
};

class MySprite : public ClickableWidget {
   public:
    typedef std::shared_ptr<MySprite> Ptr;             ///< Shared widget pointer
    typedef std::shared_ptr<const MySprite> ConstPtr;  ///< Shared constant widget pointer

    MySprite(const Layout2d& size = {100, 100});

    MySprite(const Layout& width, const Layout& height);

    MySprite(const MySprite& copy);

    MySprite& operator=(const MySprite& right);

    static MySprite::Ptr create(Layout2d size = {100, 100});

    static MySprite::Ptr copy(MySprite::ConstPtr canvas);

    virtual void setPosition(const Layout2d& position) override;
    // using Transformable::setPosition;

    virtual void setSize(const Layout2d& size) override;
    // using Transformable::setSize;

    void setOpacity(float opacity);

   protected:
    virtual Widget::Ptr clone() const override { return std::make_shared<MySprite>(*this); }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

   public:
    sf::Sprite m_sprite;
};
}  // namespace tgui

#endif