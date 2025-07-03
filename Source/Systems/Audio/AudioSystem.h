#pragma once
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include "SDL2/SDL_stdinc.h"

class SoundHandle {
    public:
	    bool IsValid() const { return mID != 0; }
	    void Reset() { mID = 0; }

	    SoundHandle& operator++() {
		    mID++;
		    return *this;
	    }

	    SoundHandle operator++(int) {
		    SoundHandle temp(*this);
		    mID++;
		    return temp;
	    }

	    const char* GetDebugStr() const {
		    static std::string tempStr;
		    tempStr = std::to_string(mID);
		    return tempStr.c_str();
	    }

	    bool operator==(const SoundHandle& rhs) const { return mID == rhs.mID; }
	    bool operator!=(const SoundHandle& rhs) const { return mID != rhs.mID; }
	    bool operator<(const SoundHandle& rhs) const { return mID < rhs.mID; }
	    bool operator<=(const SoundHandle& rhs) const { return mID <= rhs.mID; }
	    bool operator>(const SoundHandle& rhs) const { return mID > rhs.mID; }
	    bool operator>=(const SoundHandle& rhs) const { return mID >= rhs.mID; }

	    static SoundHandle Invalid;

    private:
	    unsigned int mID = 0;
};

enum class SoundState {
	Stopped,
	Playing,
	Paused
};

class AudioSystem {
    public:
        AudioSystem(int numChannels = 8);
        ~AudioSystem();

        void Update(float deltaTime);
        void ProcessInput(const Uint8* keyState);

        SoundHandle PlaySound(const std::string& soundName, bool looping = false);
        void StopSound(SoundHandle sound);
	    void PauseSound(SoundHandle sound);
	    void ResumeSound(SoundHandle sound);
	    void StopAllSounds();

    	SoundState GetSoundState(SoundHandle sound);

        void CacheAllSounds();
	    void CacheSound(const std::string& soundName);

    private:
	    struct HandleInfo {
		    std::string mSoundName;
		    int mChannel = -1;
		    bool mIsLooping = false;
		    bool mIsPaused = false;
	    };

	    struct Mix_Chunk* GetSound(const std::string& soundName);

        std::vector<SoundHandle> mChannels;
	    std::map<SoundHandle, HandleInfo> mHandleMap;
        std::unordered_map<std::string, Mix_Chunk*> mSounds;
        SoundHandle mLastHandle;
	    bool mLastDebugKey = false;
};
