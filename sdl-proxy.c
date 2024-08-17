#define SDL_MAIN_HANDLED 1

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <threads.h>
#include <time.h>
#endif

#define SPE_CONNECT 0
#define SPE_DISCONNECT 1
#define SPE_BUTTON 2
#define SPE_AXIS 3
#define SPE_HAT 4

typedef unsigned char ubyte;
typedef unsigned short ushort;

static FILE *s_output = NULL;

static inline void writeByte(ubyte x) {
  if (fputc(x, s_output) == EOF)
    exit(32);
}

static inline void writeShort(short x) {
  if (fwrite(&x, 2, 1, s_output) != 1)
    exit(32);
}

static inline void writeUShort(ushort x) {
  if (fwrite(&x, 2, 1, s_output) != 1)
    exit(32);
}

static inline void writeInt(int x) {
  if (fwrite(&x, 4, 1, s_output) != 1)
    exit(32);
}

static inline void writeString(const char *str) {
  if (str != NULL && fputs(str, s_output) == EOF)
    exit(32);
  writeByte(0);
}

static void se_connected(int port) {
  const ushort vendorId = SDL_JoystickGetDeviceVendor(port);
  const ushort productId = SDL_JoystickGetDeviceProduct(port);
  const SDL_JoystickGUID uuid = SDL_JoystickGetDeviceGUID(port);

  SDL_GameController *controller = NULL;
  SDL_Joystick *joypad = NULL;
  const char *name = NULL;

  ushort numButtons = 15;
  ushort numAxes = 6;
  ushort numHats = 0;

  if (SDL_IsGameController(port)) {
    controller = SDL_GameControllerOpen(port);
    if (controller == NULL)
      return;

    joypad = SDL_GameControllerGetJoystick(controller);
    if (joypad == NULL)
      return;

    name = SDL_GameControllerName(controller);
  } else {
    joypad = SDL_JoystickOpen(port);
    if (joypad == NULL)
      return;

    name = SDL_JoystickNameForIndex(port);
    numButtons = (ushort)SDL_JoystickNumButtons(joypad);
    numAxes = (ushort)SDL_JoystickNumAxes(joypad);
    numHats = (ushort)SDL_JoystickNumHats(joypad);
  }

  const int id = SDL_JoystickInstanceID(joypad);

  writeByte(SPE_CONNECT);
  writeInt(id);
  writeString(name);
  writeUShort(vendorId);
  writeUShort(productId);
  fwrite(uuid.data, 1, 16, s_output);
  writeUShort(numButtons);
  writeUShort(numAxes);
  writeUShort(numHats);

  if (controller) {
    for (ushort i = 0; i < numButtons; i++) {
      writeByte(
          SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)i));
    }
    for (ushort i = 0; i < numAxes; i++) {
      writeShort(
          SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis)i));
    }
  } else {
    for (ushort i = 0; i < numButtons; i++) {
      writeByte(SDL_JoystickGetButton(joypad, i));
    }
    for (ushort i = 0; i < numAxes; i++) {
      writeShort(SDL_JoystickGetAxis(joypad, i));
    }
    for (ushort i = 0; i < numHats; i++) {
      writeByte(SDL_JoystickGetHat(joypad, i));
    }
  }

  fflush(s_output);
}

static inline void se_disconnected(int id) {
  writeByte(SPE_DISCONNECT);
  writeInt(id);
  fflush(s_output);
}

static inline void se_button(int id, ubyte button, ubyte state) {
  writeByte(SPE_BUTTON);
  writeInt(id);
  writeByte(button);
  writeByte(state);
  fflush(s_output);
}

static inline void se_axis(int id, ubyte axis, short state) {
  writeByte(SPE_AXIS);
  writeInt(id);
  writeByte(axis);
  writeShort(state);
  fflush(s_output);
}

static inline void se_hat(int id, ubyte hat, ubyte state) {
  writeByte(SPE_HAT);
  writeInt(id);
  writeByte(hat);
  writeByte(state);
  fflush(s_output);
}

int main(int argc, char *argv[]) {
  if (argc < 2 || !argv[1] || !argv[1][0])
    return 22;

  s_output = fopen(argv[1], "wb");
  if (!s_output)
    return 2;
#ifdef _WIN32
  setvbuf(s_output, NULL, _IOFBF, 4096);
#endif

  writeByte(0x06);
  fflush(s_output);

  SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
  SDL_JoystickEventState(SDL_ENABLE);
  SDL_GameControllerEventState(SDL_ENABLE);

  SDL_LockJoysticks();
  const int numConnected = SDL_NumJoysticks();
  for (int i = 0; i < numConnected; i++) {
    se_connected(i);
  }
  SDL_UnlockJoysticks();

  if (argc >= 3 && strcmp(argv[2], "--sync") == 0) {
    SDL_Quit();
    fflush(s_output);
    return 0;
  }

  SDL_Event event;
  while (1) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_JOYDEVICEADDED:
        SDL_LockJoysticks();
        se_connected(event.jdevice.which);
        SDL_UnlockJoysticks();
        break;
      case SDL_JOYDEVICEREMOVED:
        se_disconnected(event.jdevice.which);
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        se_button(event.cbutton.which, event.cbutton.button,
                  event.cbutton.state);
        break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        if (SDL_GameControllerFromInstanceID(event.jbutton.which) != NULL)
          break;
        se_button(event.cbutton.which, event.cbutton.button,
                  event.cbutton.state);
        break;
      case SDL_CONTROLLERAXISMOTION:
        se_axis(event.caxis.which, event.caxis.axis, event.caxis.value);
        break;
      case SDL_JOYAXISMOTION:
        if (SDL_GameControllerFromInstanceID(event.jbutton.which) != NULL)
          break;
        se_axis(event.caxis.which, event.caxis.axis, event.caxis.value);
        break;
      case SDL_JOYHATMOTION:
        if (SDL_GameControllerFromInstanceID(event.jbutton.which) != NULL)
          break;
        se_hat(event.jhat.which, event.jhat.hat, event.jhat.value);
        break;
      default:
        break;
      }
    }

#ifdef _WIN32
    Sleep(10);
#else
    thrd_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 10000000ll}, NULL);
#endif
  }
}
