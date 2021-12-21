# Audio Network Analyzer

This program is a network analyzer for audio on [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/).
Using this program, you can measure a frequency response of your audio device.

## Mechanism
This program has both audio source and audio sink.
The audio source generates tone that will gradually increase.
The audio sink will capture the audio genenerated by the audio source, and goes through your speaker and microphone,
and will measure the gain and phase.

## Arguments

### `--freq-vmixer`
Set measuremnt frequency points as same as Roland VMixer.

### `--period` *second*
Set each period in second to ring the tone.

### `--guardband` *second*
Set time in second before starting to capture the tone after changing the frequency of the source.
This value should be much larger than the latency of your audio network.

### `--min-cycle` *cycle*
Set minimum number of cycles in an integer to ensure the capturing period.
If the period is too short, the period will be automatically extended to have the specified cycles.
