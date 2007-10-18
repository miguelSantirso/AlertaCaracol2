#include "Music_Looper.h"
#include <allegro.h>

Music_Looper::Music_Looper(void) : Loop_Playing(-1), Actual_Volume(255)
{
	// Resize the Loops size to the number of loops
	Loops.resize(3);
	// Load the loops
	Loops[0] = load_wav("musica\\loop1.wav");
	Loops[1] = load_wav("musica\\loop2.wav");
	Loops[2] = load_wav("musica\\loop3.wav");
}

Music_Looper::~Music_Looper(void)
{
	for(int i=0; i<Loops.size(); i++)
		destroy_sample(Loops[i]);
}

void Music_Looper::Play(int Volume)
{
	if(Volume > 255)
		Volume = 255;
	else if(Volume < 0)
		Volume = 0;

	if(Loop_Playing != -1 && Volume != voice_get_volume(Loop_Playing))
		voice_set_volume(Loop_Playing, Volume);

	if(Loop_Playing == -1 || voice_check(Loop_Playing) == NULL)
		Loop_Playing = play_sample(Loops[rand()%3], Volume, 127, 1000, 0);

}
