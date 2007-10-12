#pragma once
#include <vector>

struct SAMPLE;

class Music_Looper
{
public:
	Music_Looper(void);
	~Music_Looper(void);

	void Play(int Volume); // Plays the music

protected:
	std::vector<SAMPLE *> Loops;	// Stores the loops
	
	int Loop_Playing;	// The loop playing at the moment
	int Actual_Volume;	// The volume of the music that is playing at the moment
};
