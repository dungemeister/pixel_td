#include "ui_widget.h"
#include "ui_layout.h"

UIWidget::UIWidget(const std::string& id)
    :m_layout(nullptr)
    ,m_rect()
    ,m_rect_padding()
    ,m_hovered(false)
    ,m_padding_x(5)
    ,m_padding_y(5)
    ,m_hiden(false)
    ,m_id(id)
{

}

UIWidget::UIWidget():
    UIWidget("empty")
{

}

UIWidget::~UIWidget()
{

}

void UIWidget::Hovered(const SDL_FPoint& mouse_pos){

    if(СontainsPoint(mouse_pos)){
        SetHovered(true);
    }
    else{
        SetHovered(false);

    }
}