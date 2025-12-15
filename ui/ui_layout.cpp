#include "ui_layout.h"
#include "game.h"

UILayout::UILayout(Game* game, SDL_Renderer* renderer, SDL_FRect rect)
    :m_game(game)
    ,m_renderer(renderer)
    ,m_state(UILayout::State::EActive)
    ,m_rect(rect)
{
    GetGame()->add_ui(this);
}

UILayout::~UILayout(){
    while(!m_widgets.empty()){
        delete m_widgets.back();
    }
    GetGame()->remove_ui(this);
}

void UILayout::RemoveWidget(UIWidget* widget){
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    m_widgets.erase(it);
}

void UILayout::PushBackWidgetVertical(UIWidget* widget){
    SDL_FPoint pos = {};
    pos.x = m_rect.x;
    pos.y = m_rect.y + m_rect.h;
    widget->SetPosition(pos);

    m_rect.w = std::max(m_rect.w, widget->GetSizePadding().w);
    m_rect.h += widget->GetSizePadding().h;
}

void UILayout::PushBackWidgetHorizontal(UIWidget* widget){
    SDL_FPoint pos = {};
    pos.x = m_rect.x + m_rect.w;
    pos.y = m_rect.y;
    widget->SetPosition(pos);

    m_rect.w += widget->GetSizePadding().w;
    m_rect.h = std::max(m_rect.h, widget->GetSizePadding().h);;
}