#include "j2534.h"
#include <stdint.h>
#include <windows.h>

EXPORT int32_t PassThruOpen(const void *pName, uint32_t *pDeviceID) {
  return 0;
}

EXPORT int32_t PassThruClose(uint32_t DeviceID) { return 0; }

EXPORT int32_t PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID) {
  return 0;
}

EXPORT int32_t PassThruDisconnect(uint32_t ChannelID) { return 0; }

EXPORT int32_t PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout) {
  return 0;
}

EXPORT int32_t PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout) {
  return 0;
}

EXPORT int32_t PassThruStartPeriodicMsg(uint32_t ChannelID,
                                        const PASSTHRU_MSG *pMsg,
                                        uint32_t *pMsgID,
                                        uint32_t TimeInterval) {
  return 0;
}

EXPORT int32_t PassThruStopPeriodicMsg(uint32_t ChannelID, uint32_t MsgID) {
  return 0;
}

EXPORT int32_t PassThruStartMsgFilter(uint32_t ChannelID, uint32_t FilterType,
                                      const PASSTHRU_MSG *pMaskMsg,
                                      const PASSTHRU_MSG *pPatternMsg,
                                      const PASSTHRU_MSG *pFlowControlMsg,
                                      uint32_t *pMsgID) {
  return 0;
}

EXPORT int32_t PassThruStopMsgFilter(uint32_t ChannelID, uint32_t MsgID) {
  return 0;
}

EXPORT int32_t PassThruSetProgrammingVoltage(uint32_t DeviceID, uint32_t Pin,
                                             uint32_t Voltage) {
  return 0;
}

EXPORT int32_t PassThruReadVersion(uint32_t DeviceID, char *pApiVersion,
                                   char *pDllVersion, char *pFirmwareVersion) {
  return 0;
}

EXPORT int32_t PassThruGetLastError(char *pErrorDescription) { return 0; }

EXPORT int32_t PassThruIoctl(uint32_t ChannelID, uint32_t IoctlID,
                             const void *pInput, void *pOutput) {
  return 0;
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
