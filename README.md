# EU4UniversalConsole
EU4UniversalConsole is an alternative console implementation for Europa Universalis IV. For simplicity, it can be referred to as a console enabler. In short, it allows you to use the game console in normal and Ironman games. Albeit very limited, the console also can be used in multiplayer games.

TL;DR overview: you can now use console commands in Ironman games without disabling achievements. You can also use the `event` command in multiplayer with most available events. Press the INSERT key to use the console.

## Overview
Europa Universalis IV features an in-game console that allows you to modify the game state: changing countries' stats, modifying province data, accessing certain debug information or even switching countries in real time. However, this built-in console has some limitations:
* the console is unavailable when Ironman is enabled
* the console cannot be used in multiplayer games
* even in normal, non-Ironman games, certain commands are simply inaccessible (for example `build_all`)

EU4UniversalConsole removes these restrictions by providing a substitute for the in-game console. The word "substitute" means that rather than attempting to enable the game console in normally unsupported game modes, EU4UniversalConsole fetches the command list directly from the game's executable and renders a completely separate console window, with the usual restrictions omitted.

## Usage
The latest version of EU4UniversalConsole [can be found here](https://github.com/Taikelenn/EU4UniversalConsole/releases/download/v1.0.0/EU4UniversalConsole.zip). Extract the .zip file anywhere. Then:
1. Launch Europa Universalis IV.
2. Launch EU4ConsoleInjector.exe. If the injection fails, try launching the executable with administrative rights.
3. While in game, press the INSERT key to toggle the console.
4. Enter commands in the textbox at the bottom of the console. Press ENTER to execute.

## Features
EU4UniversalConsole implements all the basic features that an in-game console should have, plus a few extra:
* can be used in normal and Ironman games
* does not disable achievements in Ironman mode
* can be used in multiplayer games (however, inherent restrictions apply: see the **Multiplayer compatibility** section down below)
* allows the usage of developer-only commands
* supports CTRL+C and CTRL+V
* enables command history traversal with the up/down arrow keys
* implements an interactive listing of all in-game commands
* uses a neat monospace font, ideal for consoles :)

The graphical interface is handled by the [dear ImGui](https://github.com/ocornut/imgui/) library.

## Settings
Various settings are available that change the console's behaviour. These are available in the top left corner of the console window.
* **Intercept all input**: when the console is enabled, block mouse and keyboard input from reaching the game. Toggled on by default; it is recommended to leave this enabled.
* **Allow developer commands**: certain commands such as `build_all` or `build_forts` are marked as "unavailable in release" by EU4 developers. These commands are officially unsupported and thus the console does not let you execute these. Nevertheless, you might want to override this restriction and enable the option, granting you full access to all commands.
* **Preserve random state**: ensure that commands do not affect the EU4's random number generator. This is mostly relevant for multiplayer users; see the **Multiplayer compatibility** section down below. Toggled on by default; you should leave this enabled unless you have an important reason to do otherwise.
* **Clear console**: clears the console output.
* **Inspect entire command list**: lists all commands in a separate window; see **Gallery** for preview.
* **Unload**: unloads the console from the game. The console can be then reinjected by launching EU4ConsoleInjector.exe again.

## Multiplayer compatibility
Due to the way multiplayer works in Europa Universalis IV, console usage is generally not possible. This is not an arbitrary restriction that can be simply disabled. In multiplayer games, every player evaluates the game state by themselves. Changes done using console commands will not be propagated to other players, which will very quickly result in the infamous message "Player X is no longer in synch". **However**, there are two exceptions, with the latter being very promising to many console users:
* non-invasive commands, such as `debug_mode`, `help`, `print_flags`
* the `event` command.

The `event` command is an exception to the restriction mentioned previously because selecting an event option internally broadcasts your selection to everybody, ensuring that all players stay in sync. **However, for the event command to work, the setting *Preserve random state* must be enabled**. Otherwise, the random number generator in your game instance will desynchronize.

Most events can be triggered safely (examples: `5015`, `4042`, `742`, `random_event.7`, `6015`, `colonial.1`, `aspiration_for_liberty.3`). The only events that cannot be triggered (i.e. you risk desynchronization) are events which trigger invasive immediate effect (such as `civil_war.1`, which spawns units directly upon event appearance).

You do not need to be the game host to use the `event` command.

## Gallery
The main console window:  
![A console screenshot](https://i.imgur.com/OJ8JfIH.png)

Available settings:  
![Console settings](https://i.imgur.com/8lfgCAx.png)

Command listing window:  
![Command listing window](https://i.imgur.com/w8z5c7p.png)
