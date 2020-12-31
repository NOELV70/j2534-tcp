#include <stdint.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "j2534.h"

void init_console() {
  AllocConsole();
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
  printf("DLL loaded!\n");
}

EXPORT PassThruOpen(const void *pName, uint32_t *pDeviceID) {
  printf("PassThruOpen\n");
  WSADATA wsa;
  int server_fd;
  // wsa startup
  WSAStartup(MAKEWORD(2,2), &wsa);
  // server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // deviceId is really the serverFd
  *pDeviceID = server_fd;
  return STATUS_NOERROR;
}

EXPORT PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID) {
  printf("PassThruConnect\n");
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;
  int client_address_size;
  int server_fd;
  // deviceId is really serverFd
  server_fd = DeviceID;
  // server address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(30000);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // bind
  char socket_option_value = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_value, sizeof(socket_option_value));
  bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
  // listen
  listen(server_fd, SOMAXCONN);
  printf("listening...\n");
  // client address
  client_address_size = sizeof(struct sockaddr_in);
  memset(&client_address, 0, client_address_size);
  // get client
  int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_size);
  printf("accepted...\n");
  // channelId is really the clientFd
  *pChannelID = client_fd;
  return STATUS_NOERROR;
}

EXPORT PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout) {
  //printf("PassThruReadMsgs *pNumMsgs = %08x Timeout = %08x\n", *pNumMsgs, Timeout);
  for (;;) {
    unsigned long bytes_available = 0;
    ioctlsocket(ChannelID, FIONREAD, &bytes_available);
    // no messages available
    if (bytes_available < 12) {
      *pNumMsgs = 0;
      return STATUS_NOERROR;
    }
    // receive a message
    PASSTHRU_MSG msg = pMsg[0];
    msg.ProtocolID = ISO15765;
    msg.DataSize = recv(ChannelID, (char*)msg.Data, 12, 0);
    // skip ping frames
    if (msg.Data[0] == 0x00 &&
        msg.Data[1] == 0x00 &&
        msg.Data[2] == 0x00 &&
        msg.Data[3] == 0x00) {
      continue;
    }
    for (int i = 0; i < msg.DataSize; ++i) {
      printf("%02x", msg.Data[i]);
    }
    printf("\n");
    printf("recved...\n");
    *pNumMsgs = 0x00000001;
    return STATUS_NOERROR;
  }
}

EXPORT PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout) {
  printf("PassThruWriteMsgs *pNumMsgs = %08x Timeout = %08x\n", *pNumMsgs, Timeout);
  for (int i = 0; i < *pNumMsgs; ++i) {
    PASSTHRU_MSG msg = pMsg[i];
    send(ChannelID, (char*)msg.Data, msg.DataSize, 0);
    for (int x = 0; x < msg.DataSize; ++x) {
      printf("%02x", msg.Data[x]);
    }
    printf("\n");
    printf("wrote...\n");
  }
  return STATUS_NOERROR;
}

EXPORT PassThruStartMsgFilter(uint32_t ChannelID, uint32_t FilterType,
                                      const PASSTHRU_MSG *pMaskMsg,
                                      const PASSTHRU_MSG *pPatternMsg,
                                      const PASSTHRU_MSG *pFlowControlMsg,
                                      uint32_t *pMsgID) {
  // TODO: keep state somewhere?
  printf("PassThruStartMsgFilter\n");
  *pMsgID = 0x00000001;
  return STATUS_NOERROR;
}

EXPORT PassThruStopMsgFilter(uint32_t ChannelID, uint32_t MsgID) {
  // TODO: keep state somewhere?
  printf("PassThruStopMsgFilter\n");
  return STATUS_NOERROR;
}

EXPORT PassThruIoctl(uint32_t ChannelID, uint32_t IoctlID,
                             const void *pInput, void *pOutput) {
  // TODO: check IoctlID?
  // 3?
  // 2?
  printf("PassThruIoctl IoctlID = %08x\n", IoctlID);
  return STATUS_NOERROR;
}

EXPORT PassThruDisconnect(uint32_t ChannelID) {
  printf("PassThruDisconnect\n");
  shutdown(ChannelID, SD_BOTH);
  closesocket(ChannelID);
  return STATUS_NOERROR;
}

EXPORT PassThruClose(uint32_t DeviceID) {
  printf("PassThruClose\n");
  shutdown(DeviceID, SD_BOTH);
  closesocket(DeviceID);
  WSACleanup();
  return STATUS_NOERROR;
}

EXPORT PassThruStartPeriodicMsg(uint32_t ChannelID,
                                        const PASSTHRU_MSG *pMsg,
                                        uint32_t *pMsgID,
                                        uint32_t TimeInterval) {
  printf("PassThruStartPeriodicMsg\n");
  return ERR_NOT_SUPPORTED;
}

EXPORT PassThruStopPeriodicMsg(uint32_t ChannelID, uint32_t MsgID) {
  printf("PassThruStopPeriodicMsg\n");
  return ERR_NOT_SUPPORTED;
}

EXPORT PassThruSetProgrammingVoltage(uint32_t DeviceID, uint32_t Pin,
                                             uint32_t Voltage) {
  printf("PassThruSetProgrammingVoltage\n");
  return ERR_NOT_SUPPORTED;
}

EXPORT PassThruReadVersion(uint32_t DeviceID, char *pApiVersion,
                                   char *pDllVersion, char *pFirmwareVersion) {
  printf("PassThruReadVersion\n");
  return ERR_NOT_SUPPORTED;
}

EXPORT PassThruGetLastError(char *pErrorDescription) {
  printf("PassThruGetLastError\n");
  return ERR_NOT_SUPPORTED;
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID lpvReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
      init_console();
      break;
    }
    case DLL_THREAD_ATTACH: {
      printf("DLL_THREAD_ATTACH\n");
      break;
    }
    case DLL_PROCESS_DETACH: {
      printf("DLL_PROCESS_DETACH\n");
      break;
    }
    case DLL_THREAD_DETACH: {
      printf("DLL_THREAD_DETACH\n");
      break;
    }
  }
  return TRUE;
}
