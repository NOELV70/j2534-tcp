#ifndef J2534_H
#define J2534_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define EXPORT __stdcall __declspec(dllexport)

typedef struct _SCONFIG {
  uint32_t Parameter;
  uint32_t Value;
} SCONFIG;

typedef struct _SCONFIG_LIST {
  uint32_t NumOfParams;
  SCONFIG *ConfigPtr;
} SCONFIG_LIST;

typedef struct _PASSTHRU_MSG {
  uint32_t ProtocolID;
  uint32_t RxStatus;
  uint32_t TxFlags;
  uint32_t Timestamp;
  uint32_t DataSize;
  uint32_t ExtraDataIndex;
  unsigned char Data[4128];
} PASSTHRU_MSG;

EXPORT int32_t PassThruOpen(const void *pName, uint32_t *pDeviceID);
EXPORT int32_t PassThruClose(uint32_t DeviceID);
EXPORT int32_t PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID);
EXPORT int32_t PassThruDisconnect(uint32_t ChannelID);
EXPORT int32_t PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout);
EXPORT int32_t PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout);
EXPORT int32_t PassThruStartPeriodicMsg(uint32_t ChannelID,
                                        const PASSTHRU_MSG *pMsg,
                                        uint32_t *pMsgID,
                                        uint32_t TimeInterval);
EXPORT int32_t PassThruStopPeriodicMsg(uint32_t ChannelID, uint32_t MsgID);
EXPORT int32_t PassThruStartMsgFilter(uint32_t ChannelID, uint32_t FilterType,
                                      const PASSTHRU_MSG *pMaskMsg,
                                      const PASSTHRU_MSG *pPatternMsg,
                                      const PASSTHRU_MSG *pFlowControlMsg,
                                      uint32_t *pMsgID);
EXPORT int32_t PassThruStopMsgFilter(uint32_t ChannelID, uint32_t MsgID);
EXPORT int32_t PassThruSetProgrammingVoltage(uint32_t DeviceID, uint32_t Pin,
                                             uint32_t Voltage);
EXPORT int32_t PassThruReadVersion(uint32_t DeviceID, char *pApiVersion,
                                   char *pDllVersion, char *pFirmwareVersion);
EXPORT int32_t PassThruGetLastError(char *pErrorDescription);
EXPORT int32_t PassThruIoctl(uint32_t ChannelID, uint32_t IoctlID,
                             const void *pInput, void *pOutput);
#endif
