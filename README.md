# ngrok4ios
Patching ngrok macOS arm64 binary to run on iOS

## How does it work?
- The ngrok binary is taken from a macOS arm64 build.
- By default, some checks prevent a macOS binary from running on iOS. This repository contains a patcher to fix them.
- There are a few missing functions. Stubs can be found in `stubs.c`. `ngrok` doesn't appear to use these functions. Forcing dyld to the flat namespace mode and insert the stubs can be used as a workaround.

## Patching process
This can be done directly on a jailbroken iOS environment. You need to find and install iOS SDK (on Procursus, it is `build-essential`).

- Install required tools (`clang`, `Darwin CC Tools`, `git`, `ldid`, `build-essential`). One-line command: `sudo apt install -y build-essential clang git ldid odcctools`.
- Place `ngrok` into `/usr/bin/ngrok_bin`.
 
```bash
# Clone this repository
git clone https://github.com/khanhduytran0/ngrok4ios
cd ngrok4ios

# Set path to the SDK. The path below is an example, but is default in Procursus's build-essential
export SDK=/usr/share/SDKs/iPhoneOS.sdk

# Jump to root, this is required for doing ngrok modifications in /usr
su

# Change frameworks path
install_name_tool -change /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation /System/Library/Frameworks/CoreFoundation.framework/CoreFoundation -change /System/Library/Frameworks/Security.framework/Versions/A/Security /usr/lib/libngrokhelper.dylib /usr/bin/ngrok_bin

# Build and install the helper library
clang -isysroot $SDK stubs.c -o /usr/lib/libngrokhelper.dylib -dynamiclib -framework Security

# Build and install the patcher to /usr/bin
clang -isysroot $SDK patcher.c -o /usr/bin/ngrokpatcher

# Patch ngrok
ngrokpatcher /usr/bin/ngrok_bin

# Sign ngrok
ldid -Sent.xml /usr/bin/ngrok_bin

# Create a wrapper script
echo "DYLD_FORCE_FLAT_NAMESPACE=1 ngrok_bin \$@" > /usr/bin/ngrok

# Give executable permission
chmod +x /usr/bin/ngrok_bin /usr/bin/ngrok

# Run ngrok
ngrok ...
```

Note: since macOS's ngrok build is 11.0 (which is compatible with iOS 14.0), this might not work on iOS below 14.0.
