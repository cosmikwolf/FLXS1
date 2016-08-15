refactor to-dos

- globals to refactor:

  - previousState -- display (might be able to remove and instance inline)

  - stepmode -- display, input
  - notePage -- display, input

  - selectedChannel --    display, input, LED
  - selectedStep -        display, input, LED
  - currentState -- display, input, LEDArray

  - patternChannelSelector - input, LED

  - tempoX100  -- display, input, masterclock, sequencer
  - extClock - display, input, MasterClock

  - sequenceCount -- flash, input, clock, midi // Global definition?

  - currentPattern -- display, flash, clock, midi
  - queuePattern -- flash, clock, midi

  - playing -- display, flash, input, clock, midi, noteDatum, Sequencer

  - tempoBlip -- clock, midi
  - masterPulseCount - clock, midi
  - masterTempoTimer - clock, midi

  - beatLength - clock, midi, sequencer,
  - startTime - clock, midi, sequencer,

  - calibrationBuffer - display, input, output

  - wasPlaying -- clock
  - lastTimer - clock
  - internalClockTimer - clock

  - blipTimer - midi
  - testTimer - midi
  - pulseTimer - midi
  - firstRun - midi
  - lastBeatLength - midi
