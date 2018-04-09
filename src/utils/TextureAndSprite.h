#ifndef TEXTUREANDSPRITE_H
#define TEXTUREANDSPRITE_H

#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

namespace tgui
{
    class TextureAndSprite : public ClickableWidget
    {
    public:

        typedef std::shared_ptr<TextureAndSprite> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const TextureAndSprite> ConstPtr; ///< Shared constant widget pointer

        TextureAndSprite(const Layout2d& size = {100, 100});

        TextureAndSprite(const Layout& width, const Layout& height);

        TextureAndSprite(const TextureAndSprite& copy);

        TextureAndSprite& operator= (const TextureAndSprite& right);


        static TextureAndSprite::Ptr create(Layout2d size = {100, 100});

        static TextureAndSprite::Ptr copy(TextureAndSprite::ConstPtr canvas);


        virtual void setPosition(const Layout2d& position) override;
        using Transformable::setPosition;


        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;

        virtual void setOpacity(float opacity) override;

    protected:

        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<TextureAndSprite>(*this);
        }

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:

        sf::Texture     m_texture;
        sf::Sprite      m_sprite;
    };

    class Sprite : public ClickableWidget
    {
    public:

        typedef std::shared_ptr<Sprite> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const Sprite> ConstPtr; ///< Shared constant widget pointer

        Sprite(const Layout2d& size = {100, 100});

        Sprite(const Layout& width, const Layout& height);

        Sprite(const Sprite& copy);

        Sprite& operator= (const Sprite& right);


        static Sprite::Ptr create(Layout2d size = {100, 100});

        static Sprite::Ptr copy(Sprite::ConstPtr canvas);


        virtual void setPosition(const Layout2d& position) override;
        using Transformable::setPosition;


        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;

        virtual void setOpacity(float opacity) override;

    protected:

        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<Sprite>(*this);
        }

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:

        sf::Sprite      m_sprite;
    };
}

#endif