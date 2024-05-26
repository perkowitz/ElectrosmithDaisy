#include <chrono>
#include <thread>

#include <iomanip>
#include <string>
#include "daisy_patch.h"
#include "daisysp.h"
#include "util/CpuLoadMeter.h"

using namespace daisy;
using namespace daisysp;

typedef unsigned char  u8;

#define MODULE_COUNT 14
#define INSTANCE_COUNT 1

DaisyPatch hw;
CpuLoadMeter meter;

u8 currentModule = 0;

Oscillator osc[INSTANCE_COUNT];
Svf svf[INSTANCE_COUNT];
WhiteNoise noise[INSTANCE_COUNT];
AdEnv adenv[INSTANCE_COUNT];
Adsr adsr[INSTANCE_COUNT];
OnePole onePole[INSTANCE_COUNT];
AnalogBassDrum bassDrum[INSTANCE_COUNT];
AnalogSnareDrum snareDrum[INSTANCE_COUNT];
SquareNoise squareNoise[INSTANCE_COUNT];
ZOscillator zosc[INSTANCE_COUNT];
ModalVoice modal[INSTANCE_COUNT];
Wavefolder waveFolder[INSTANCE_COUNT];
ClockedNoise clockedNoise[INSTANCE_COUNT];
FormantOscillator formant[INSTANCE_COUNT];


std::string moduleNames[MODULE_COUNT] = { 
    "Osc   ", "Svf   ", 
    "Noise", "AdEnv", 
    "Adsr   ", "OnePole",
    "AnaBass", "AnaSnare",
    "SquareNoise", "ZOscillator",
    "ModalVoice", "Wavefolder",
    "ClockedNoise", "FormantOscillator"
};



void OledMessage(std::string message, int row) 
{
    char* mstr = &message[0];
    hw.display.SetCursor(0, row * 10);
    hw.display.WriteString(mstr, Font_6x8, true);
    hw.display.Update();
}

void OledMessage(std::string message, int row, int column) 
{
    char* mstr = &message[0];
    hw.display.SetCursor(column * 8, row * 10);
    hw.display.WriteString(mstr, Font_6x8, true);
    hw.display.Update();
}

void ProcessEncoder() {
    int inc = hw.encoder.Increment();
    currentModule = (currentModule + inc) % MODULE_COUNT;
}


void ProcessControls()
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    ProcessEncoder();
    // ProcessKnobs();
    // ProcessGates();
}


void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    meter.OnBlockStart();

    ProcessControls();

    for(size_t i = 0; i < size; i++)
    {

        float sig = 0.0f;
        for (u8 i = 0; i < INSTANCE_COUNT; i++) {
            switch (currentModule) {
                case 0:
                    osc[i].Process();
                    break;
                case 1:
                    svf[i].Process(sig);
                    break;
                case 2:
                    noise[i].Process();
                    break;
                case 3:
                    adenv[i].Process();
                    break;
                case 4:
                    adsr[i].Process(true);
                    break;
                case 5:
                    onePole[i].Process(sig);
                    break;
                case 6:
                    bassDrum[i].Process();
                    break;
                case 7:
                    snareDrum[i].Process();
                    break;
                case 8:
                    squareNoise[i].Process(300);
                    break;
                case 9:
                    zosc[i].Process();
                    break;
                case 10:
                    modal[i].Process(true);
                    break;
                case 11:
                    waveFolder[i].Process(sig);
                    break;
                case 12:
                    clockedNoise[i].Process();
                    break;
                case 13:
                    formant[i].Process();
                    break;
            }
        }

        out[0][i] = out[1][i] = sig;
        out[2][i] = out[3][i] = sig;
    }

    meter.OnBlockEnd();
}


// Main -- Init, and Midi Handling
int main(void)
{
    // Init
    float samplerate;
    hw.Init();
    samplerate = hw.AudioSampleRate();

    meter.Init(samplerate, 128, 1.0f);

    for (u8 i = 0; i < INSTANCE_COUNT; i++) {
        osc[i].Init(samplerate);
        svf[i].Init(samplerate);
        noise[i].Init();
        adenv[i].Init(samplerate);
        adsr[i].Init(samplerate);
        onePole[i].Init();
        bassDrum[i].Init(samplerate);
        snareDrum[i].Init(samplerate);
        squareNoise[i].Init(samplerate);
        zosc[i].Init(samplerate);
        modal[i].Init(samplerate);
        waveFolder[i].Init();
        clockedNoise[i].Init(samplerate);
        formant[i].Init(samplerate);
    }

    // Start stuff.
    hw.SetAudioBlockSize(128);
    hw.midi.StartReceive();
    hw.StartAdc();
    hw.StartAudio(AudioCallback);
    for(;;)
    {
        float avgCpu = meter.GetAvgCpuLoad() / INSTANCE_COUNT;
        u8 avg1 = (int)(avgCpu * 100);
        u8 avg2 = (int)(avgCpu * 10000 - avg1 * 100);
        std::string num = std::to_string(avg1) + "." + std::to_string(avg2);
        OledMessage("m:" + moduleNames[currentModule] + "       ", 2);
        OledMessage("cpu:" +  num + "%" + "   ", 3);

        hw.display.Update();
    }
}
