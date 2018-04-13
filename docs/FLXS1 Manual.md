# ZETAOHM FLXS1

![image-20180412215051015](/var/folders/hn/ch6s89_14b1fvg7y7pb4qlyr0000gn/T/abnerworks.Typora/image-20180412215051015.png)

# **USER MANUAL**

###### Firmware v18

#### Introduction

```
Fluxus One is a 4 channel, 64 step sequencer for the Eurorack Modular Synthesizer format. It is designed to provide maximum compositional capabilities with as simple of a user interface as possible in 26 HP. 
```

## **Front Panel Layout Overview**

![image-20180412213159969](/var/folders/hn/ch6s89_14b1fvg7y7pb4qlyr0000gn/T/abnerworks.Typora/image-20180412213159969.png)

#### Interface Elements

1. **Play Button** Toggle Play / Pause. Ch button + play copies channel data. Matrix button + play copy step data
2. **Stop button** Stops play and resets all sequences to first step.
3. **Record Button** Ch button + Rec pastes copied channel data. Step button + Rec pastes copied step data
4. **Matrix Buttons** Select a step. Ch + matrix buttons trigger channel functions (gold text). Matrix buttons also provide pattern select in pattern select and song mode. Matrix buttons allow selection of quantized notes in quantize mode
5. **Channel Buttons** Change selected channel. Allow you to mask channels in pattern select, save and song modes.
6. **5v Gate Inputs** 4 gate inputs allow you to clock FLXS1 with external clock sources. They are also selectable as modulation sources in channel modulation menus. 
7. **±5v CV inputs** 4 CV inputs are selectable as modulation sources in channel modulation menus.
8. **Gate Outputs** Gate Outputs send an 8v output signal each time a step is triggered. The signal will remain high until the gate length runs out.
9. **cv#a Pitch Output** Pitch outputs are capable of sending CV signals with a range of ±10v. These ports are configured in the first pitch and gate menu of step data configurations. The output for pitch CV is restricted to a 10v range, specified in the global menu.
10. **cv#b Modulation Output** Modulation outputs are capable of sending CV signals with a range of ±10v. They are configured in the LFO / ENV step data menu.
11. **Clock output** The clock port will send a clock signal whenever FLXS1 is playing. It will replicate an external clock signal if FLXS1 is being driven by an external clock.
12. **Data Knob** Allows you to change selected parameters, as well as values. There are two functionalities for this knob, specified in the global menu. `Momentary` (default) will make the knob change the value by default, and will change selected parameter while pressed in. `Toggle` will toggle between changing value and changing selected parameter each time the button is pressed
13. **Page Down** Changes the range of steps being represented by the matrix buttons. The direction of this shift is specified in the global menu. Shift-press enters multiselect mode. 
14. **Page Up**  Changes the range of steps being represented by the matrix buttons. The direction of this shift is specified in the global menu.  Shift-press enters song mode. 
15. **Pattern Button**  Allows you to load a saved pattern. Shift press allows you to save the current pattern. Pattern Select mode also allows you to fast chain patterns.
16. **Tempo Button**  Allows you to set the tempo when clock source is the internal clock. Also allows you to set all other clock parameters.
17. **Shift Button**  Allows you to access secondary functions for different buttons (white outlined text). Also allows you to do tap tempo when in tempo menu.
18. **3.5mm MIDI Input** Connect a MIDI clock source to FLXS1 using the included 3.5mm to MIDI dongle (Make Noise or Korg standard adapter. Other adapters will not work). 
19. **3.5mm MIDI Output** Sequence external MIDI gear using the included 3.5mm to MIDI dongle (Make Noise or Korg standard adapter. Other adapters will not work). 
20. **USB Port** Allows for easy firmware updating, as well as MIDI sync to FLXS1 over USB. Great for DAW syncing
21. **Program Button** This button is pressed to initiate a firmware update. If it is pressed accidentally when not attempting to update the firmware, it will be necessary to power cycle the device to return it to a normal state.



## **Programming a Sequence**

##### Connect a patch

- The gate output for each of the 4 channels is labeled gt1, gt2, gt3 and gt4. 
- The pitch CV output is to the right of the gate output.
- The modulation CV output is to the right of the pitch CV output.

##### **Select a channel**

- To select that channel, press the corresponding channel button on the left hand side.

##### **Selecting steps**

 - A step is selected by pressing a matrix button. The **`pg up`** and **`pg dn`** buttons allow you to page through the 4 pages of 16 steps to give you access to all 64 steps of a sequence. There is a global setting **pgup/dn direction** which  allows you to change the direction of the **`pg up`** and **`pg dn`** buttons.

##### **Changing values and selecting parameters**

- To change the value of the selected parameter, turn the **`data encoder`**

- To change the selected parameter, **press in, and simultaneously turn the rotary encoder**. 

- This is referred to as **Momentary Encoder Mode.** 
  - In **Momentary Encoder Mode,** you can also press shift + turn to change the selected parameter.
  - There is an alternate **Toggle Encoder Mode** which is available in the global menu (shift-tempo)
  - **Toggle Encoder Mode** allows you to toggle back and forth between changing the value and changing the selected parameter by pressing the data encoder knob in. The shift button does nothing in this mode.

- When in the parameter selection state, the highlighted value will be boxed, and the text will be a light color. 

- When in change value state,  the highlighted value is solid bright, and the text is black.   

### **Entering step data**

##### Activating and deactivating steps

- Tapping an inactive step will activate the step
- Double tapping an active step will deactivate that step
- There are three pages of settings for each step

##### Pitch + Gate Settings

Selecting a step will allow you to begin editing step specific parameters

- **Pitch** specifies the voltage sent from the pitch CV output

- **Gate** specifies the length that the gate is open when the step is triggered
- **Type**  specifies the behavior of the gate. 

|                              On                              |                             Tie                              |                             Hold                             |                          **1-Hit**                           |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| gate turns on and off normally. Arpeggios are retriggered.gate turns on and off normally. Arpeggios are retriggered. | gate is held open through the next step, and glide is turned on | gate is held open through the end of the step (arpeggio is not retriggered) | gate acts as a trigger, and closes immediately after opening. Arpeggios do not retrigger the gate in this state |

 - **Glide** Each channel has an analog, variable glide circuit. A value of 0 disconnects the circuit entirely. Turning up the value connects the circuit and increases the slew up to a maximum of about 5 seconds of settle time.  


##### Arpeggio Settings

|                        **Algorithm**                         |                          **Speed**                           |                       **Octave**                        |                           Interval                           |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-----------------------------------------------------: | :----------------------------------------------------------: |
| the direction of the arpeggio: up, down, up down1, up down 2, random | the speed which the arpeggio will arpeggiate, in relation to step length. You can specify numerator and denominator of the fraction to achieve polyrhythmic arpeggios. | the number of octaves over which the arpeggio will span | the intervals of notes which will be present in the arpeggio |


   - ##### **LFO / Env  Settings**

     - **Type** the type of modulation output that will be sent from **cv#b** on this step

     |                          Envelopes                           |                           Signals                            |                             LFO                              |                             LFO                              |
     | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
     | **Env asr** - an envelope will rise to the amp voltage, hold, then slope down to zero | **Skip** - The default setting. Current LFO or env will continue |      **Sine** - a sine wave LFO will be sent from cv#b       |  **Triangle** - a triangle wave LFO will be sent from cv#b   |
     | **Env dec** - an envelope will be triggered at the amp voltage and slope down to zero |  **Trigger** - A 10v signal will open and immediately close  | **Sawup** - a rising sawtooth wave LFO will be sent from cv#b | **Sawdn** - a falling sawtooth wave LFO will be sent from cv#b |
     | **Env ar**  - an envelope will rise to the amp voltage, and then slope down to zero | **Quantized Voltage** - A steady quantized voltage will be sent from cv#b |    **Square** - a square wave LFO will be sent from cv#b     | **S+H** - a sample and hold wave LFO will be sent from cv#b  |
     | **Env atta** - an envelope will rise to the amp voltage and then reset to zero. | **Voltage** - A steady unquantized voltage will be sent from cv#b | **RndSq** - a rounded square wave LFO will be sent from cv#b |                                                              |

      - **Amplitude -** Specifies the amplitude of the LFO being sent in volts. Envelopes are unipolar voltages, and LFOs are bipolar voltages

     - **Speed -** Specifies the speed of the LFO or envelope. For LFOs, a speed of 64 is 1 wavelength per step. 

      - **Offset -** Specifies a voltage offset added or subtracted to the cv#b output. 

## Channel Configuration

- The channel menus contain all channel specific settings on Fluxus One.
- Tapping a channel button changes the selected channel if it is not currently selected. 
- If the channel is currently selected, tapping the channel button will page through channel settings.
- There are three channel menus spread over a few pages

### Transport Menu

The transport menu contains parameters that define how the sequencer advances

| First Step                     | Step Count                                                   | Clock Div                                                    | Direction                                                    | Swing                               |
| ---------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ----------------------------------- |
| The first step of the sequence | The number of steps the sequence will advance before it resets back to the first step | The division of the main clock time with which this channel will advance | The direction in which the sequencer advances. Can set to Fwd, Rev, Pend, Rand | the amount of swing on this channel |

###**Quantizer Menu**

- Each channel has its own built in quantizer which can be configured individually

- **Scale** indicates the type of scale which is being selected. Semitone indicates a classic 12 note equally divided octave scale. Pythagorean is a scaled based on natural harmonics (also 12 note) 

- **Key** indicates the root note for the quantize scale

- **Mode** indicates the selection of notes that will comprise the quantized scale. 

	- Mode allows the user to select pre-defined modes from the list by selecting with the encoder, however when Mode is selected, the behavior of the Matrix buttons changes, and allows the user to turn on and off individual notes to create a custom quantize scale. 

###Modulation Menus

 - Fluxus One has extensive modulation matrix capabilities. Modulation works on a **destination subscribes to source** methodology, where any number of destinations can subscribe to a single modulation source, since it is all routed internally. 
  - The left column indicates the modulation destination, 
  - The right column selection indicates the modulation source
  	- **cv#** or **gt#** indicates an external input
  	- **ch#** indicates internal gate modulation
  	- **cv#a** or **cv#b** indicates an **internal cv modulation**
  	- On gate inputs, turning left will display a percentage. This indicates a percentage chance that the modulation will be high, without any input, when that step is triggered
- **Skip # -** Skip Step Modulation.
  - When a high signal is received from the modulation source, the current channel will skip the specified number of steps
  - If a Gate Input is selected, every time a rising edge of a HIGH signal is detected on the Gate Input specified, this specific channel will advance the number of steps specified.
  - If a Gate Output is selected, this indicates internal modulation, so every time a gate is fired on the output selected, this specific channel will advance the number of steps specified.
- **Gate Length -** CV input
  - Modulate the gate length of each step
- **Random pitch** - Gate Input 
  - If gate is high when a step is fired, the pitch will be randomized
  - add + sub: the number of steps that can be added or subtracted for randomization - note: this is done **before quantization** so all notes are still in key
- **Transpose** - 1v/oct CV input
  - Transpose the channel based upon CV input 
- **Glide Time** - CV input
- **Arp Type** - CV input 
- **Arp Speed** - CV input
- **Arp Octave** - CV input
- **Arp Intvl** - CV Input

## **Tempo Menu**

###**Changing Clock Source**

- Highlight the current clock source to change its value. You can select **Internal Clock, Gate Input 1-4, 3.5mm MIDI, or USB MIDI.**
- External Gate Clock inputs are currently 4ppq. MIDI clock inputs are 24ppq (midi standard)

###**Changing Tempo**

- If the clock source is set to **Internal Clock** you can change the tempo. Highlight tempo to change it.

###**Reset Input**

- There is a reset input which will reset the current sequence when an external gate signal is received. 

###**Tap Tempo**

- When in the tempo menu, the shift button can be tapped to manually enter a tempo. 

##**Pattern Save, Select and Chain**


###**Press shift-pattern to enter pattern save mode**

- The matrix buttons each represent a save slot
- Matrix buttons that are not highlighted represent empty save slots
- Pressing the channel buttons will mask the save. If a channel button that channel will not be saved.
- Press a matrix button to execute the save operation

###**Press the Pattern button to enter pattern select mode**

- Again, the matrix buttons represent which slots have saved data in them. The display also shows you which slots have saved data in which channels. If a slot channel has an underscore on the display, that means that specific channel has no data saved. If there is a number in that slot, it means there are steps programmed in that save slot. A slot that is represented by 1__4 means that channels 1 and 4 have saved data in them, while channels 2 and 3 are empty.
- You can mask the loading operation with the channel buttons. Press a channel button to disable loading that channel.
- If a channel is masked, that means that whatever sequence is currently loaded into that channel will continue playing.
- **Trigger**  - this allows you to set exactly when the saved pattern will be loaded. **Instant** means that the pattern will be loaded as soon as the matrix button is pressed and released. **ch#reset** indicates that the pattern will be loaded when the specified channel next resets. if **ch1reset** is selected, the pattern will change all selected channels when the sequence running on channel 1 ends its current iteration, and resets to the beginning. 
- To execute a pattern load operation, press a matrix button to select which save slot will be loaded.

###**Fast Chain Mode**

- Pattern select mode has the ability to allow you chain multiple patterns in quick succession.
- When in pattern select mode, press and hold a matrix button. While holding at least one matrix button down at all times, enter a sequence of patterns you wish to play. You can enter up to 16 patterns in this way, and FLXS1 will play through them in sequence. 

##**Song Mode**


###**Using Song Mode**

- To enter song edit menu, press shift-page up.


- To begin playing a song, press play while in song edit menu.
	- If sequencer is currently is already playing, it will begin playing the first song pattern once channel 1 resets
	- If sequencer is not currently playing, it will begin playing the song immediately.
- Pressing stop twice from any menu will disable song mode. 

###**Editing a song**

- First, compose a few patterns outside of song mode, and save them into different save slots.
- Enter song edit menu by pressing **shift-page up**
	- You will see a short list of song events.
     You can select different song events using a press-turn on the data encoder. (if you are using toggle mode, this behaves the same way as other menus) 
After selecting a song event, turning the data encoder will change the event parameter.
  - Positive values change the **repeat count**. When FLXS1 hits this event, it will repeat the specified pattern X times according to the count.
  - The zero value triggers **Repeat Song**, which will go back to the first event in this song.
  - The value below this one triggers **Stop Song** which will stop the song, pause and reset the sequencer.
  - Negative values specify the **Jump** trigger which will jump to a specified index X times according to the count, after which, the sequencer will continue on to the next event
- Currently, you can specify up to **16 song events.** 
- Currently, you can only have one song loaded
- Song data is saved upon exiting the song edit menu. 
  - If song changes are made, and the sequencer is shut off while still in the song edit menu, the song will not be saved
- If FLXS1 is shut off while simultaneously exiting the song edit menu, song data may be corrupted and may not load properly.
  *Notice: When playing a song, patterns are automatically loaded and will overwrite unsaved data. Additionally, saving data while in song mode may be difficult as it must be saved quickly, before pattern changes.*





## **Button Shortcuts** 

Many buttons have gold text labels. These are channel button shortcuts. Many still have to be implemented properly. The following are currently working. If it is not listed here, the function may not be fully working yet. 

### To copy and paste a step

- Hold the step you wish to copy, and press play
- To paste, hold the step you wish to overwrite and press record

### To clear a step

- Hold the step you wish to clear
- Press stop twice while holding down the step button

### To copy and paste a channel

- Hold the channel button you wish to copy, and press play
- To paste, hold the channel button for the channel you wish to overwrite and press record

### To initialize a channel

- Hold the channel button you wish to clear
- Press stop twice while holding down the channel button
- You can erase multiple channels simultaneously in this manner


### To mute a channel
- Hold **ch + `mute gt`**
  - Mute the gate of the selected channel
- Hold **ch + `mute a`**
  - Mutes the Pitch CV (CVxA) for the selected channel
- Hold **ch +`mute b`**
  - Mutes the Modulation CV (CVxB) for the selected channel


### Using the randomize shortcut:

- To enter randomize menu, press **ch+`rndm`** (9th matrix button, left column, 3rd down) 
- You can select to randomize pitch and gate together, just pitch, or just gate
- min allows you to select the low note in the quantization operation.
- span allows you to select the number of octaves which the randomization operation will select from
- Press **ch+`rndm`** again to execute the randomization operation.

### **Other Channel Shortcuts**

		**Are not implemented yet! These are coming soon!** 

##Updating Firmware

Firmware can be updated using a computer, a MicroUSB cable and a paperclip
First, download the latest version of Teensy Loader on to your computer from PJRC:

[**https://www.pjrc.com/teensy/loader.html**](https://www.pjrc.com/teensy/loader.html)
**Download the latest firmware for Fluxus One**
**Version 16d:** [**https://www.dropbox.com/s/ehyd9fjpwzjz7n5/flxs1_beta16d.hex.zip?dl=1**](https://www.dropbox.com/s/ehyd9fjpwzjz7n5/flxs1_beta16d.hex.zip?dl=1)
**If the link is broken, please check** [**zetaohm.com**](http://zetaohm.com) **for the latest.** 

1. Launch Teensy Loader
2. Unzip the Firmware Download
3. Drag the firmware file (flxs1_beta16d.hex) to the Teensy Loader App
4. You should see the correct firmware file loaded at the bottom of the teensy  loader screen
5. Connect your FLXS1 to your computer with a MicroUSB cable.
6. Using a paperclip (or a whittled toothpick) press the PRGM button that is behind the front panel. Try to to align the paperclip to be perpendicular to the front panel while you press in. You can feel a haptic click once you press it.
7. You will see a progress bar appear in the teensy loader app,   and then a message that says “Reboot OK”.
8. Your Fluxus One should reboot and you should be able to see the firmware version appear on the splash screen
9. If it does not reboot properly, you can try flashing the firmware again, or shutting off power manually and turning it back on.
####TROUBLESHOOTING
- If your firmware flashing does not seem to be working properly, make sure that the button labeled “Auto” on Teensy Loader is bright green and lit up (as opposed to dim).
- If it is dimmed, then press that button to make sure it will automatically program when you press the PGRM button on your Fluxus One
- If you attempted to flash the firmware, but the splash screen did not seem to update firmware versions, try the firmware update again until it updates properly. Make sure you are properly pressing the program button in (once, for about 1 second, and then release)
- If you receive a "File too large" message, try downloading the Teensy loader and FLXS1 firmware from the website again
- If you are reciving ANY unexplained errors or improper flashing, try using a different USB cable. A bad USB connection has been isolated for a number of FLXS1 firmware update failures.