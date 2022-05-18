#include <stdio.h>
#include <string.h>
#include "daisy_field.h"

using namespace daisy;

DaisyField     field;
SdmmcHandler   sdh;
FatFSInterface fsi;
WavPlayer      sampler;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in, AudioHandle::InterleavingOutputBuffer out, size_t size){
	for(size_t i = 0; i < size; i += 2) {
		out[i] = out[i + 1] = s162f(sampler.Stream());
	}
}

void sdcard_init() {
	daisy::SdmmcHandler::Config sdconfig;
	sdconfig.Defaults(); 
	sdconfig.speed           = daisy::SdmmcHandler::Speed::FAST; 
	sdconfig.width           = daisy::SdmmcHandler::BusWidth::BITS_4;
	sdh.Init(sdconfig);
	fsi.Init(FatFSInterface::Config::MEDIA_SD);
	f_mount(&fsi.GetSDFileSystem(), "/", 1);
	sampler.Init(fsi.GetSDPath());

}

uint32_t display_time=0, led_time=0, adsr_time=0, wf_time=0, octave_time=0;
void UpdateDisplay(){
	//display
	char line1[32];
	if(System::GetNow()+1000>display_time){
		display_time=System::GetNow()+1000;
		snprintf(line1,32,"%2d %2d %2d\n",sampler.GetLooping(),sampler.GetNumberFiles(),sampler.GetCurrentFile());
		field.display.Fill(false);
		field.display.SetCursor(0,0);
		field.display.WriteString(line1, Font_11x18 , true);
		snprintf(line1,32,"%s",sampler.file_info_[0].name); 
		field.display.SetCursor(0,20);
		field.display.WriteString(line1, Font_11x18 , true);
	}
	field.display.Update();
}

int main(void) {
	size_t blocksize = 4;
	// set up the field
	field.Init();
	// set up the sd card handler class to defaults
	sdcard_init();
	sampler.Open(0);
	sampler.SetLooping(true);
        field.SetAudioBlockSize(blocksize);
	field.StartAudio(AudioCallback);
	for(;;)
	{
		UpdateDisplay();
		sampler.Prepare();
	}
}
