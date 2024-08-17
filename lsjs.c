#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>
#include <stdio.h>

typedef unsigned char ubyte;

static void writeHex(char *out, const ubyte *data, size_t numBytes) {
  for (size_t i = 0; i < numBytes; i++) {
    const ubyte upperNibble = data[i] >> 4;
    const ubyte lowerNibble = data[i] & 0xF;
    out[i * 2] = upperNibble + (upperNibble > 9 ? ('a' - (char)10) : '0');
    out[i * 2 + 1] = lowerNibble + (lowerNibble > 9 ? ('a' - (char)10) : '0');
  }
}

static inline void writeUuid(const ubyte *data, char *template) {
  writeHex(template, data, 4);
  writeHex(&template[9], &data[4], 2);
  writeHex(&template[14], &data[6], 2);
  writeHex(&template[19], &data[8], 2);
  writeHex(&template[24], &data[10], 6);
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_JOYSTICK);
  SDL_JoystickUpdate();

  char uuid[37] = "00000000-0000-0000-0000-000000000000";
  const int numConnected = SDL_NumJoysticks();
  for (int i = 0; i < numConnected; i++) {
    writeUuid(SDL_JoystickGetDeviceGUID(i).data, uuid);
#ifdef _WIN32
    /* Fun fact: Windows Defender is so hilariously inept at virus detection
     * that calling printf, a function provided by their own signed library,
     * makes it inexplicably think the program is a trojan when compiled as
     * a 32-bit application (64-bit is fine).
     *
     * This is not a meme.
     * Windows Defender is actually that stupid.
     *
     * Printing it one character at a time, however, doesn't trigger a false
     * positive, so I do that instead.
     */
    for (int j = 0; j < 36; j++)
      putc(uuid[j], stdout);
    putc('\n', stdout);
#else
    printf("%s\n", uuid);
#endif
  }

  SDL_Quit();
  return 0;
}
