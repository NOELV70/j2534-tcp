#include <stdint.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "j2534.h"

EXPORT int32_t PassThruOpen(const void *pName, uint32_t *pDeviceID) {
  WSADATA wsa;
  int server_fd;
  struct sockaddr_in server_address;
  // wsa startup
  WSAStartup(MAKEWORD(2,2), &wsa);
  // server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // server address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(30000);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // bind
  bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
  // listen
  listen(server_fd, SOMAXCONN);
  // deviceId is really the serverFd
  *pDeviceID = server_fd;
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID) {
  struct sockaddr_in client_address;
  int client_address_size;
  // client address
  client_address_size = sizeof(struct sockaddr_in);
  memset(&client_address, 0, client_address_size);
  // get client
  int server_fd = DeviceID;
  int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_size);
  // channelId is really the clientFd
  *pChannelID = client_fd;
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout) {
  PASSTHRU_MSG msg = pMsg[0];
  msg.ProtocolID = ISO15765;
  msg.DataSize = recv(ChannelID, (char*)msg.Data, sizeof(msg.Data), 0);
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout) {
  for (int i = 0; i < *pNumMsgs; ++i) {
    PASSTHRU_MSG msg = pMsg[i];
    send(ChannelID, (char*)msg.Data, msg.DataSize, 0);
  }
  return STATUS_NOERROR;
}


EXPORT int32_t PassThruStartMsgFilter(uint32_t ChannelID, uint32_t FilterType,
                                      const PASSTHRU_MSG *pMaskMsg,
                                      const PASSTHRU_MSG *pPatternMsg,
                                      const PASSTHRU_MSG *pFlowControlMsg,
                                      uint32_t *pMsgID) {
  // TODO: keep state somewhere?
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruIoctl(uint32_t ChannelID, uint32_t IoctlID,
                             const void *pInput, void *pOutput) {
  // TODO: check IoctlID?
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruDisconnect(uint32_t ChannelID) {
  closesocket(ChannelID);
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruClose(uint32_t DeviceID) {
  // TODO: close socket?
  closesocket(DeviceID);
  WSACleanup();
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruStartPeriodicMsg(uint32_t ChannelID,
                                        const PASSTHRU_MSG *pMsg,
                                        uint32_t *pMsgID,
                                        uint32_t TimeInterval) {
  return ERR_NOT_SUPPORTED;
}

EXPORT int32_t PassThruStopPeriodicMsg(uint32_t ChannelID, uint32_t MsgID) {
  return ERR_NOT_SUPPORTED;
}

EXPORT int32_t PassThruStopMsgFilter(uint32_t ChannelID, uint32_t MsgID) {
  return ERR_NOT_SUPPORTED;
}

EXPORT int32_t PassThruSetProgrammingVoltage(uint32_t DeviceID, uint32_t Pin,
                                             uint32_t Voltage) {
  return ERR_NOT_SUPPORTED;
}

EXPORT int32_t PassThruReadVersion(uint32_t DeviceID, char *pApiVersion,
                                   char *pDllVersion, char *pFirmwareVersion) {
  return ERR_NOT_SUPPORTED;
}

EXPORT int32_t PassThruGetLastError(char *pErrorDescription) {
  return ERR_NOT_SUPPORTED;
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID lpvReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
      break;
    }
    case DLL_THREAD_ATTACH: {
      break;
    }
  }
  return TRUE;
}
