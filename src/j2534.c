#include <stdint.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "j2534.h"

static int server_fd = -1;
static int client_fd = -1;

EXPORT int32_t PassThruOpen(const void *pName, uint32_t *pDeviceID) {
  WSADATA wsa;
  struct sockaddr_in addr;
  WSAStartup(MAKEWORD(2,2), &wsa);
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(30000);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
  listen(server_fd, SOMAXCONN);
  *pDeviceID = 0x00000001;
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID) {
  // TODO: socket connect


  *pChannelID = 0x00000001;
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout) {
  // TODO: read from socket
  PASSTHRU_MSG msg = pMsg[0];
  msg.ProtocolID = ISO15765;
  msg.DataSize = 6;
  // arbitration ID
  msg.Data[0] = 0x00;
  msg.Data[1] = 0x00;
  msg.Data[2] = 0x07;
  msg.Data[3] = 0xE8;
  // PDU
  msg.Data[4] = 0x7E;
  msg.Data[5] = 0x00;
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout) {
  for (int i = 0; i < *pNumMsgs; ++i) {
    PASSTHRU_MSG msg = pMsg[i];
    // TODO: write to socket
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
  // TODO: disconnect socket
  return STATUS_NOERROR;
}

EXPORT int32_t PassThruClose(uint32_t DeviceID) {
  // TODO: close socket?
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
