# cbeat
C program that creates raw 8-bit audio output, inspired by [bytebeat](https://dollchan.net/bytebeat).

Currently, it continuously outputs a raw audio loop on `stdout` based on the song used in the `salinewin.exe` virus (see it in bytebeat [here](https://dollchan.net/bytebeat/#4AAAA+kUtjNEKgDAIAL8m0SKYutkm4X7Kxz6+QT3ewV3uiAm1DJu5WWtqdxs6ZF6ecDkbHciQEVyJIlDhXLASKbVPcS5Ez2fYtNf5z7i4utAL)). You can edit `main.c` and use the functions in `looper.h` to make it play whatever you want.

## Building from source
If you use bash and have `gcc` on your system, simply run `compile.sh` from the repo's root directory; otherwise, use your compiler of choice with all the `.c` files in the repo.

## Playing audio
The program outputs raw (mono) audio data to `stdout`, as 8-bit unsigned integers with a sample rate of 8000Hz. If you have `ffplay` installed, you can just run `play.sh`, otherwise use whatever solution you want.

## Documentation
The code is documented with [doxygen](https://www.doxygen.nl/) comments in the header files.

## Plans
- Alternate looper that uses less memory while hopefully not using that much more CPU
- Reading note sequences/songs from files
- Performance improvements wherever possible
