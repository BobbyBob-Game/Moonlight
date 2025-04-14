#ifndef DIALOGUE_H
#define DIALOGUE_H
#include "defs.h"

class DialogueManager {
    public:
    void trigger(const std::vector<std::string>& msg, bool showChoice);
    void update(float deltaTime);
    void skipOrNext();
    void render(SDL_Renderer* renderer, TTF_Font* font);
    bool isActive() const {return active;}
    void handleKey(SDL_Keycode key);
    bool isModeEnabled() const { return mode; }
    void setMode(bool value) {mode = value;}

    private:
    bool active = false;
    std::vector<std::string> messages;
    std::string displayedText;
    int messageIndex = 0;
    int charIndex = 0;
    float timer = 0.0f;
    float typeSpeed = 0.03f; //NOTE: this is in second nha Bach
    bool fullyDisplayed = false;
    bool awaitingChoice = false;
    bool mode = false;
};


#endif