#pragma once
#include "ui_widget.h"
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>

class UIImage: public UIWidget{
public:
    UIImage(class UILayout* layout, SDL_Renderer* renderer);
    UIImage(const std::string& sprite, class UILayout* layout, SDL_Renderer* renderer);

    void Update(float deltatime) override;
    void Draw() override;
    void AddSprite(const std::string& sprite);
    void SetDestSize(SDL_FPoint size) { m_rect.w = size.x; m_rect.h = size.y;}
private:
    void load_texture(const std::string& sprite);

    SDL_Renderer* m_renderer;
    std::unordered_map<std::string, SDL_Texture*> m_sprites;
    std::string m_cur_sprite;
};