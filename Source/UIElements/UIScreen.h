#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <list>

#include "../Utils/Math.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"

class UIScreen
{
public:
    // Tracks if the UI is active or closing
    enum class UIState {
        Active,
    	Idle,
        Closing
    };

	UIScreen(class Game* game, const std::string& fontName);
	virtual ~UIScreen();

	// UIScreen subclasses can override these
	virtual void Update(float deltaTime);
	virtual void Draw(class SDL_Renderer *renderer);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key);

    // Set state to closing
	void Close();

    // Get state of UI screen
	UIState GetState() const { return mState; }
	void SetState(const UIState state) { mState = state; }

    // Game getter
    class Game* GetGame() { return mGame; }

    // Add a button to this screen
	UIButton* AddButton(const std::string& text, const Vector2& pos, const Vector2& dims, const std::function<void()> &onClick);
    UIText* AddText(const std::string& text, const Vector2& pos, const Vector2& dims, const int pointSize = 40, const int unsigned wrapLength = 1024);
    UIImage* AddImage(const std::string& imagePath, const Vector2& pos, const Vector2& dims, const Vector3& color = Color::White);

protected:
    // Sets the mouse mode to relative or not
	class Game* mGame;
	class UIFont* mFont;

	// Configure positions
	Vector2 mPos;
	Vector2 mSize;

	// State
	UIState mState;

	// List of buttons, texts, and images
    int mSelectedButtonIndex;
	std::vector<UIButton *> mButtons;
    std::vector<UIText *> mTexts;
    std::vector<UIImage *> mImages;
};
