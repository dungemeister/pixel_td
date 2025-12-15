#pragma once
#include "SDL3/SDL.h"

class UIWidget{
public:
    typedef SDL_FRect UISize;
    UIWidget(class UILayout*);
    UIWidget();
    virtual ~UIWidget();

    void        SetLayout(UILayout* layout) { m_layout = layout; }
    UILayout*   GetLayout() const           { return m_layout; }

    void    SetSize(const UISize& size)     { m_rect = size; }
    UISize  GetSize() const                 { return m_rect; }
    UISize  GetSizePadding() const          { return m_rect_padding; }
    
    virtual void SetPosition(const SDL_FPoint& pos) {
        m_rect.x = pos.x;
        m_rect.y = pos.y;
        UpdatePadding();
    }

    SDL_FPoint GetPosition() const { return {m_rect.x, m_rect.y}; }

    bool    IsHovered() const               { return m_hovered;}
    void    SetHovered(bool state)          { m_hovered = state; }

    virtual bool СontainsPoint(const SDL_FPoint& point) const {
        return SDL_PointInRectFloat(&point, &m_rect);
    }
    void Hovered(const SDL_FPoint& mouse_pos);

    void UpdatePaddingX(int pad) {
        m_padding_x = pad;
        m_rect_padding.x -= pad;
        m_rect_padding.w += pad * 2;
    }
    void UpdatePaddingY(int pad) {
        m_padding_y = pad;
        m_rect_padding.y -= pad;
        m_rect_padding.h += pad * 2;
    }
    void UpdatePadding(){
        m_rect_padding = m_rect;
        UpdatePaddingX(m_padding_x);
        UpdatePaddingY(m_padding_y);
    }
    void SetHiden(bool state) { m_hiden = state; }
    bool GetHiden() const { return m_hiden; }
    
    virtual void Update(float deltatime) {}
    virtual void Draw() {}
    virtual void HandleEvent(const SDL_Event& event) {}
protected:
    class UILayout* m_layout;
    UISize m_rect;
    UISize m_rect_padding;

    bool m_hovered;
    int m_padding_x;
    int m_padding_y;

    bool m_hiden;
};