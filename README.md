# WebEx AntiFlash

**Disclaimer: I'm not affiliated with Cisco and/or WebEx in any way, nor was any analysis of the software needed to make this program.**

The goal of this tiny project is to prevent Cisco WebEx from flashing in the taskbar whenever an event occurs. I have to use this piece of software to attend online classes and I've found the taskbar popping up when typing bash commands pretty annoying, so I botched this together. You can use AntiFlash on other programs too by providing a PID as a command-line argument, but it might not work as expected.

## Installation

There's a release related to the first commit containing two files: `WebexInjector.exe` and `WebexAntiFlash.dll`.
In order for the program to work, you need to copy `WebexAntiFlash.dll` into the folder containing `atmgr.exe`, which is generally `C:\Users\USERNAME_HERE\AppData\Local\WebEx\WebEx\Meetings`.

You can then copy `WebexInjector.exe` to whatever location suits you best.

## Usage

The main way of using this program is to simply run `WebexInjector.exe` once you've joined a WebEx meeting.

On the other hand, if you want to check if this work with some other piece of software, you can provide the PID of the target program. Note that the `WebexAntiFlash.dll` file should still be placed in the target program's folder. The command-line should look something like this:

`.\WebexInjector.exe <TargetPID>`

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

This module places a trampoliney hook on the `FlashWindowEx` function, from `user32.dll`. The hook redirects function calls to a function that just returns `FALSE`, which prevents Webex from making its taskbar icon flash.