# Archived
According to [this page](https://ciscocollabcustomer.ideas.aha.io/ideas/WXCUST-I-626), it seems that Cisco added a feature to change the flashing behaviour on the 14th of November 2021, as part of update 41.11 of Cisco WebEx. I'm also not using this program anymore and do not intend to maintain it any further. Therefore, there is no reason for this repo not to be archived.

# WebEx AntiFlash

**Disclaimer: We are not affiliated, associated, authorized, endorsed by, or in any way officially connected with Cisco, or any of its subsidiaries or its affiliates. No program analysis was done in order to develop this.**

The goal of this tiny project is to prevent Cisco WebEx from flashing in the taskbar whenever an event occurs. I have to use this piece of software to attend online classes and I've found the taskbar popping up when typing bash commands pretty annoying, so I botched this together. You can use AntiFlash on other programs too as detailed in the *Usage* section, but it might not work as expected.

## Installation

Binary releases used to be provided, but they were getting flagged by multiple AV vendors so I decided to stop distributing them in case my build environment was compromised when I built them. If you want to use WebexAntiFlash, you'll unfortunately have to compile it yourself; I used Visual Studio 2019.
After building the program, you will need to copy `WebexAntiFlash.dll` into the folder containing `atmgr.exe`, which was `C:\Users\USERNAME_HERE\AppData\Local\WebEx\WebEx\Meetings` in my case.

You can then copy `WebexInjector.exe` to whatever location suits you best.

## Usage

The main way of using this program is to simply run `WebexInjector.exe` once you've joined a WebEx meeting.

On the other hand, if you want to check if this work with some other piece of software, you can provide the PID or process name of the target program. Note that the `WebexAntiFlash.dll` file should still be placed in the target program's folder. The command-line should look something like this:

`.\WebexInjector.exe pid <TargetPID>`

## Compiling

### Requirements

- This was made using Visual Studio Community 2019, but the source code should work on any version released in the past years

### Actual compiling

`git clone https://github.com/lacaulac/webex-anti-flash.git`

Then open `WebExAntiFlash.sln` with Visual Studio, select *Release* and *x86* and then hit *Build -> Build Solution*. You should find both files in a Release folder at the root of the cloned repo.

## Technical details

### WebexInjector.exe

This program injects the `WebexAntiFlash.dll` module into a target process, by default the first `atmgr.exe` instance that could be found.

### WebexAntiFlash.dll

This module places trampoliney hooks on the `FlashWindowEx` and `FlashWindow` functions, from `user32.dll`. The hook redirects function calls to a function that just returns `FALSE`, which prevents Webex and any other application from making its taskbar icon flash.
