#pragma once
#include "ui_widget.h"
#include <vector>

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
    
    virtual void update(float deltatime) {};
    virtual void draw() {
        for(auto widget: m_widgets){
            widget->Draw();
        }};
    virtual void ProcessInput(const bool* keys) {};
    virtual void HandleEvent(const SDL_Event&) {};

    void AddWidget(UIWidget* widget){ m_widgets.push_back(widget); }
    void RemoveWidget(UIWidget* widget);
    void PushBackWidgetVertical(UIWidget* widget);
    void PushBackWidgetHorizontal(UIWidget* widget);
    const SDL_FRect& GetRect() const { return m_rect; }

    void RemoveWidgets(){
        while(!m_widgets.empty()){
            delete m_widgets.back();
        }
        
        m_widgets.clear();
        
        m_rect.w = 0;
        m_rect.h = 0;
    }
protected:
    Game* m_game;
    SDL_Renderer* m_renderer;
    SDL_FRect m_rect;
    UILayout::State m_state;
    std::vector<UIWidget*> m_widgets;
};