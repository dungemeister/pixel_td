#pragma once
#include "ui_widget.h"
#include <string>
#include <unordered_map>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

class UILabel: public UIWidget{
public:
    UILabel(const std::string& id);
    UILabel(const std::string& text, const std::string& id);
    virtual ~UILabel();
    void UpdateTexture();
    void SetText(const std::string& text);
    std::string GetText() const { return m_text; }
    void SetFontSize(int size);
    void SetColor(const SDL_Color& col) {
        m_color = col;
        UpdateTexture();
    }
    void SetBackgroundColor(const SDL_Color& col) {
        m_background_color = col;
    }
    void SetBorderColor(const SDL_Color& col) {
        m_border_color = col;
    }
    void SetBackgroundState(bool state){
        m_background = state;
    }
    void SetBorderState(bool state){
        m_border = state;
    }
    void Update(float deltatime) override;
    void Draw(SDL_Renderer* renderer) override;
    // void HandleEvent(const SDL_Event& event) override;
private:
    SDL_Texture* m_texture;
    TTF_Font* m_font;
    int m_font_size;
    std::string m_text;
    SDL_Color m_color;
    bool m_border;
    bool m_background;
    SDL_Color m_background_color;
    SDL_Color m_border_color;

    const std::string m_font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    const std::unordered_map<int, SDL_Texture*> m_sized_textures;

    SDL_Surface* CreateSurface(const std::string& text);
    
    void DrawBackground(SDL_Renderer* renderer);
    void DrawBorder(SDL_Renderer* renderer);
    void DrawText(SDL_Renderer* renderer);
};