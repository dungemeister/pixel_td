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
    RemoveWidgets();
    GetGame()->remove_ui(this);
}

void UILayout::PushBackWidgetVertical(std::unique_ptr<UIWidget> widget){
    widget->SetLayout(this);
    widget->Update(0.f);

    SDL_FPoint pos = {};
    pos.x = m_rect.x;
    pos.y = m_rect.y + m_rect.h;
    widget->SetPosition(pos);

    m_rect.w = std::max(m_rect.w, widget->GetSizePadding().w);
    m_rect.h += widget->GetSizePadding().h;
    m_widgets.emplace_back(std::move(widget));
}

void UILayout::PushBackWidgetHorizontal(std::unique_ptr<UIWidget> widget){
    widget->SetLayout(this);
    widget->Update(0.f);

    SDL_FPoint pos = {};
    pos.x = m_rect.x + m_rect.w + widget->GetPadding().x;
    pos.y = m_rect.y + widget->GetPadding().y;
    widget->SetPosition(pos);

    m_rect.w += widget->GetSizePadding().w;
    m_rect.h = std::max(m_rect.h, widget->GetSizePadding().h);
    m_widgets.emplace_back(std::move(widget));
}

void UILayout::PushBackWidget(std::unique_ptr<UIWidget> widget){
    widget->SetLayout(this);
    widget->Update(0.f);

    m_rect.w += widget->GetSizePadding().w;
    m_rect.h = std::max(m_rect.h, widget->GetSizePadding().h);
    m_widgets.emplace_back(std::move(widget));
}

void UILayout::AddLayoutVertical(std::unique_ptr<UILayout> child){
    child->SetParent(this);

    auto rect = child->GetRect();
    rect.x = m_rect.x;
    rect.y = m_rect.y + m_rect.h;
    child->SetPosition({rect.x, rect.y});
    child->update(0.f);

    m_rect.h += rect.h;
    m_rect.w = std::max(m_rect.w, rect.w);
    m_children.emplace_back(std::move(child));
}

void UILayout::AddLayoutHorizontal(std::unique_ptr<UILayout> child){
    child->SetParent(this);
    child->update(0.f);
    m_children.emplace_back(std::move(child));
}

void UILayout::AddLayout(std::unique_ptr<UILayout> child, const SDL_FPoint& pos){
    child->SetParent(this);

    auto rect = child->GetRect();
    child->SetPosition(pos);
    child->update(0.f);

    m_rect.h += rect.h;
    m_rect.w = std::max(m_rect.w, rect.w);
    m_children.emplace_back(std::move(child));
}

UIWidget* UILayout::GetWidget(const std::string& id){
    for(auto& w: m_widgets){
        if(w->Id() == id){
            return w.get();
        }
    }
    for(auto& l: m_children){
        if(auto* w = l->GetWidget(id)){
            return w;
        }
    }

    return nullptr;
}