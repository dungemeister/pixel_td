#pragma once
#include "ui_widget.h"
#include <SDL3/SDL.h>

class UISlider: public UIWidget{
public:
    UISlider(class UILayout*);

    void Update(float deltatime) override;
    void Draw() override;
    void set_range(float min, float max){
        m_min_value = min;
        m_max_value = max;
    }
    void set_value(float value){
        m_value = value;
    }
    float get_factor() const { return m_value / (m_max_value - m_min_value);}
    
private:
    float m_value;
    float m_max_value;    
    float m_min_value;    
    void render_rectangle(SDL_FRect dest_rect, float width, bool filled);
};