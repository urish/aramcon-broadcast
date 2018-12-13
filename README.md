# aramcon-broadcast

Use this project with [Segger Embeded Studio](https://www.segger.com/products/development-tools/embedded-studio/).

## Setup
You need to configure your nRF5 SDK path in Segger Studio. Go to the Options interface (Tools -> Options), search there for "Global Macros" (it's under the Build section of the Building pane), and add a line that configures the macro `NRF5_SDK` to point to your SDK installation directory, e.g.

```
NRF5_SDK=C:/Dev/nRF5_SDK_15.2.0_9412b96
```

## Running The Code

Open the project, connect your device through the JLink interface and press F5 to build and run the code. If the debugger stops at the first line of `main()`, simply hit F5 again to resume.

Once the code is running, you will see a new USB serial port in your computer. Every time you write 32 bytes to this serial port, the firmware will transmit these bytes using ESB. 

You can then use the provided python script to stream an MP3 file over USB, e.g.:

```
python sender\sender.py COM30 test.mp3
```
