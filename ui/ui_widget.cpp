#include "ui_widget.h"
#include "ui_layout.h"

UIWidget::UIWidget(class UILayout* layout)
    :m_layout(layout)
    ,m_rect()
    ,m_rect_padding()
    ,m_hovered(false)
    ,m_padding_x(5)
    ,m_padding_y(5)
    ,m_hiden(false)
{
    if(m_layout)
        m_layout->AddWidget(this);
}

UIWidget::UIWidget():
    UIWidget(nullptr)
{

}

UIWidget::~UIWidget()
{
    if(m_layout)
        m_layout->RemoveWidget(this);
}

void UIWidget::Hovered(const SDL_FPoint& mouse_pos){

    if(СontainsPoint(mouse_pos)){
        SetHovered(true);
    }
    else{
        SetHovered(false);

    }
}