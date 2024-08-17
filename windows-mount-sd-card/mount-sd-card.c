#include <Windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <virtdisk.h>

#define STATUS_SUCCESS 0
#define STATUS_OPEN_FAILED 1
#define STATUS_ATTACH_FAILED 2
#define STATUS_DRIVE_NOT_FOUND 3

static inline void writeStatusCode(FILE *output, DWORD code) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  code = _byteswap_ulong(code);
#endif
  fwrite(&code, sizeof(DWORD), 1, output);
  fflush(output);
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    return (int)ERROR_BAD_ARGUMENTS;

  FILE *output = fopen(argv[2], "w");
  if (!output)
    return (int)ERROR_FILE_NOT_FOUND;
  if (fputc(6, output) == EOF)
    return (int)ERROR_BROKEN_PIPE;
  fflush(output);

  FILE *input = fopen(argv[1], "r");
  if (!input)
    return (int)ERROR_FILE_NOT_FOUND;
  if (fgetc(input) != 6)
    return (int)ERROR_BROKEN_PIPE;

  unsigned short pathLen;
  if (fread(&pathLen, sizeof(unsigned short), 1, input) != 1) {
    fclose(input);
    fclose(output);
    return (int)ERROR_BROKEN_PIPE;
  }

#if __BYTE_ORDER == __LITTLE_ENDIAN
  pathLen = _byteswap_ushort(pathLen);
#endif

  wchar_t *vhdPath = malloc((pathLen + 1) * sizeof(wchar_t));
  if (fread(vhdPath, sizeof(wchar_t) * pathLen, 1, input) != 1) {
    fclose(input);
    fclose(output);
    return (int)ERROR_BROKEN_PIPE;
  }

  vhdPath[pathLen] = (wchar_t)0;

  VIRTUAL_STORAGE_TYPE storageType;
  storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_VHD;
  memset(&storageType.VendorId, 0, 16);

  HANDLE virtualDisk;
  DWORD status = OpenVirtualDisk(
      &storageType, vhdPath,
      VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_DETACH,
      OPEN_VIRTUAL_DISK_FLAG_NONE, NULL, &virtualDisk);

  if (status != ERROR_SUCCESS) {
    fputc(STATUS_OPEN_FAILED, output);
    writeStatusCode(output, status);
    fclose(input);
    fclose(output);
    return (int)status;
  }

  const DWORD drivesBefore = GetLogicalDrives();
  status = AttachVirtualDisk(virtualDisk, NULL, ATTACH_VIRTUAL_DISK_FLAG_NONE,
                             0, NULL, NULL);

  if (status != ERROR_SUCCESS) {
    CloseHandle(virtualDisk);
    fputc(STATUS_ATTACH_FAILED, output);
    writeStatusCode(output, status);
    fclose(input);
    fclose(output);
    return (int)status;
  }

  const DWORD drivesAfter = GetLogicalDrives();
  const DWORD newDrives = drivesAfter & ~drivesBefore;

  unsigned long driveIndex;
  if (_BitScanReverse(&driveIndex, (unsigned long)newDrives) == 0) {
    CloseHandle(virtualDisk);
    fputc(STATUS_DRIVE_NOT_FOUND, output);
    fflush(output);
    fclose(input);
    fclose(output);
    return (int)ERROR_INVALID_DRIVE;
  }

  fputc(STATUS_SUCCESS, output);
  fputc('A' + (char)driveIndex, output);
  fflush(output);

  while (1) {
    const int c = fgetc(input);

    status = DetachVirtualDisk(virtualDisk, DETACH_VIRTUAL_DISK_FLAG_NONE, 0);
    if (status == ERROR_SUCCESS || c != 0)
      break;

    fputc(1, output);
    fflush(output);
  }

  CloseHandle(virtualDisk);
  fputc((status == ERROR_SUCCESS) ? 0 : 1, output);
  fflush(output);

  fclose(input);
  fclose(output);

  return (int)status;
}
