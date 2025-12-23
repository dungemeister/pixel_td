#pragma once
#include "ui_widget.h"
#include <vector>
#include <memory>

class UILayout{
public:
    enum State{
        EActive,
        EClosed,
    };
    UILayout(class Game* game, SDL_Renderer* renderer, SDL_FRect rect);
    virtual ~UILayout();
    void SetState(UILayout::State state) { m_state = state; }
    UILayout::State GetState() const { return m_state; }

    Game*           GetGame() const { return m_game; }
    SDL_Renderer*   GetRenderer() const { return m_renderer; }
    SDL_FPoint      GetPosition() const { return {m_rect.x, m_rect.y}; }

    void SetPosition(const SDL_FPoint& pos) { m_rect.x = pos.x; m_rect.y = pos.y;}
    void SetParent(UILayout* parent) { m_parent = parent; }

    virtual void update(float deltatime) {
        for(auto& child: m_children){
            child->update(deltatime);
        }
        for(auto& widget: m_widgets){
            widget->Update(deltatime);
        }
    };
    virtual void draw() {
        for(auto& child: m_children){
            child->draw();
        }
        for(auto& widget: m_widgets){
            widget->Draw(GetRenderer());
        }};
    virtual void ProcessInput(const bool* keys) {};
    virtual void HandleEvent(const SDL_Event&) {};

    void AddWidget(std::unique_ptr<UIWidget> widget){
         m_widgets.emplace_back(std::move(widget));
    }
    
    void PushBackWidgetVertical  (std::unique_ptr<UIWidget> widget);
    void PushBackWidgetHorizontal(std::unique_ptr<UIWidget> widget);
    void PushBackWidget(std::unique_ptr<UIWidget> widget);

    UIWidget* GetWidget(const std::string& id);
    const SDL_FRect& GetRect() const { return m_rect; }
    const SDL_FPoint GetSize() const { return {m_rect.w, m_rect.h}; }

    void RemoveWidgets(){
        for(auto& l: m_children){
            l->RemoveWidgets();
        }
        
        m_widgets.clear();
        
        m_rect.w = 0;
        m_rect.h = 0;
    }
    void AddLayoutVertical(std::unique_ptr<UILayout> child);
    void AddLayoutHorizontal(std::unique_ptr<UILayout> child);
    void AddLayout(std::unique_ptr<UILayout> child, const SDL_FPoint& pos);
protected:
    Game* m_game;
    SDL_Renderer* m_renderer;
    SDL_FRect m_rect;
    UILayout::State m_state;
    std::vector<std::unique_ptr<UIWidget>> m_widgets;
    std::vector<std::unique_ptr<UILayout>> m_children;
    UILayout* m_parent;
};