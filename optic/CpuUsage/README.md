# CpuUsage

## Author

Mike Perkowitz

## Description

Uses CpuLoadMeter to estimate how much of CPU power each standard DaisySP module uses. Multiple instances of each module can be created,
to improve the accuracy of the measurement. However, the default is 1, since some modules take up a lot of CPU.

Processing measured is fairly simplified; filters are only sent a 0 signal, and the ADSR is only measured with gate=on. Processing required could vary with different inputs or parameter settings.


## Results
- Oscillator: 2.3%
- SVF: 1.3%
- WhiteNoise: 0.4%
- AdEnv: 1.3%
- ADSR: 0.8%
- OnePole: 0.6%
- AnalogBassDrum: 21.9%
- AnalogSnareDrum: 87.2%
- SquareNoise: 1.8%
- ZOscillator: 6.0%
- ModalVoice: 47.8%
- Wavefolder: 0.9%
- ClockedNoise: 1.4%
- FormantOscillator: 3.1%
