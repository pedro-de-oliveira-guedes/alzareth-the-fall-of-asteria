#include "AudioSystem.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;

AudioSystem::AudioSystem(int numChannels) {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("Failed to initialize SDL_mixer: %s", Mix_GetError());
		return;
	}

	Mix_AllocateChannels(numChannels);
	mChannels.resize(numChannels);
}

AudioSystem::~AudioSystem() {
	for (auto &[fst, snd] : mSounds) {
		Mix_FreeChunk(snd);
	}
	mSounds.clear();

	Mix_CloseAudio();
}

void AudioSystem::Update(float deltaTime) {
	for (const auto &channel : mChannels) {
		if (channel.IsValid()) {
			if (Mix_Playing(mHandleMap.at(channel).mChannel) == 0) {
				StopSound(channel);
			}
		}
	}
}

SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping) {
    Mix_Chunk *sound = GetSound(soundName);

	if (!sound) {
		SDL_Log("[AudioSystem] Failed to load sound: %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

    int availableChannel = -1;

	for (int i = 0; i < mChannels.size(); ++i) {
		if (!mChannels[i].IsValid()) {
			availableChannel = i;
			break;
		}
	}

	if (availableChannel == -1) {
		for (const auto& [handle, info] : mHandleMap) {
			if (info.mSoundName == soundName) {
				availableChannel = info.mChannel;
				SDL_Log("[AudioSystem] Stopping sound %s on channel %d", soundName.c_str(), availableChannel);
				StopSound(handle);
				break;
			}
		}
	}

	if (availableChannel == -1) {
		for (const auto& [handle, info] : mHandleMap) {
			if (info.mIsLooping) {
				availableChannel = info.mChannel;
				SDL_Log("[AudioSystem] Stopping looping sound %s on channel %d", info.mSoundName.c_str(), availableChannel);
				StopSound(handle);
				break;
			}
		}
	}

	if (availableChannel == -1) {
		const HandleInfo info = mHandleMap.begin()->second;
		availableChannel = info.mChannel;
		SDL_Log("[AudioSystem] Stopping oldest sound %s on channel %d", info.mSoundName.c_str(), availableChannel);
		StopSound(mHandleMap.begin()->first);
	}

	++mLastHandle;

	mHandleMap.insert(
		std::make_pair(mLastHandle, HandleInfo{ soundName, availableChannel, looping, false })
	);
	mChannels[availableChannel] = mLastHandle;

	Mix_PlayChannel(availableChannel, sound, looping ? -1 : 0);

    return mLastHandle;
}

void AudioSystem::StopSound(SoundHandle sound) {
	if (mHandleMap.find(sound) == mHandleMap.end()) {
		SDL_Log("[AudioSystem] Attempted to stop non-existent sound with handle %d", sound);
		return;
	}

	Mix_HaltChannel(mHandleMap[sound].mChannel);
	mChannels[mHandleMap[sound].mChannel].Reset();
	mHandleMap.erase(sound);
}

void AudioSystem::PauseSound(SoundHandle sound) {
	if (mHandleMap.find(sound) == mHandleMap.end()) {
		SDL_Log("[AudioSystem] Attempted to pause non-existent sound with handle %d", sound);
		return;
	}

	if (!mHandleMap[sound].mIsPaused) {
		Mix_Pause(mHandleMap[sound].mChannel);
		mHandleMap[sound].mIsPaused = true;
	} else {
		SDL_Log("[AudioSystem] Sound with handle %d is already paused", sound);
	}
}

void AudioSystem::ResumeSound(SoundHandle sound) {
	if (mHandleMap.find(sound) == mHandleMap.end()) {
		SDL_Log("[AudioSystem] Attempted to resume non-existent sound with handle %d", sound);
		return;
	}

	if (mHandleMap[sound].mIsPaused) {
		Mix_Resume(mHandleMap[sound].mChannel);
		mHandleMap[sound].mIsPaused = false;
	} else {
		SDL_Log("[AudioSystem] Sound with handle %d is not paused", sound);
	}
}

SoundState AudioSystem::GetSoundState(SoundHandle sound) {
    if(mHandleMap.find(sound) == mHandleMap.end()) {
        return SoundState::Stopped;
    }

    if(mHandleMap[sound].mIsPaused) {
        return SoundState::Paused;
    }

	return SoundState::Playing;
}

void AudioSystem::StopAllSounds() {
    Mix_HaltChannel(-1);

    for(auto & mChannel : mChannels) {
        mChannel.Reset();
    }

    mHandleMap.clear();
}

void AudioSystem::CacheAllSounds() {
    #ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec}) {
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav") {
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
    #endif
}

void AudioSystem::CacheSound(const std::string& soundName) {
	GetSound(soundName);
}

Mix_Chunk* AudioSystem::GetSound(const std::string& soundName) {
	std::string fileName = "../Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end()) {
		chunk = iter->second;
	} else {
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk) {
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}

	return chunk;
}

void AudioSystem::ProcessInput(const Uint8* keyState) {
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey) {
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++) {
			if (mChannels[i].IsValid()) {
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end()) {
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
							hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else {
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i), mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}
