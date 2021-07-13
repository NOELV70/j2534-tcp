#include <stdint.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "j2534.h"

/*
cd C:\Program Files (x86)\Softing\Diagnostic Tool Set 8\8.14\bin\
C:\Program Files (x86)\Softing\D-PDU API\11.26.072\VeCom\vecomw32fwj25proc.exe /s"j2534-tcp" /p9804 /l"C:\j2534-tcp\j2534-tcp.dll" /v2 /k0 /t"C:\ProgramData\Softing\D-PDU API\11.26.072\d-pduapi.ini"
*/

#define PASS_THRU_WRITE_MSGS_SLEEP_MS 0
#define PASS_THRU_READ_MSGS_SLEEP_MS 0
#define RECV_THREAD_SLEEP_MS 0
#define PORT 30000

static HANDLE recv_buffer_mutex = NULL;
static PASSTHRU_MSG recv_buffer[100000];
static int recv_buffer_size = 0;
static int client_fd = -1;
static int server_fd = -1;

void init_console() {
  AllocConsole();
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
}

void close_client() {
  if (client_fd != -1) {
    printf("closing client\n");
    shutdown(client_fd, SD_BOTH);
    closesocket(client_fd);
    client_fd = -1;
  }
}

void close_server() {
  if (server_fd != -1) {
    printf("closing server\n");
    shutdown(server_fd, SD_BOTH);
    closesocket(server_fd);
    server_fd = -1;
  }
}

void reset_buffer() {
  WaitForSingleObject(recv_buffer_mutex, INFINITE);
  recv_buffer_size = 0;
  ReleaseMutex(recv_buffer_mutex);
}

unsigned long GetTime() {
  FILETIME t;
  GetSystemTimeAsFileTime(&t);
  return t.dwLowDateTime;
}

void init_server() {
  WSADATA wsa;
  struct sockaddr_in server_address;
  // wsa startup
  WSAStartup(MAKEWORD(2,2), &wsa);
  // server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // server address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  // bind
  char yes = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
  // listen
  listen(server_fd, SOMAXCONN);
  printf("listening...\n");
}

DWORD WINAPI accept_thread(void* data) {
  for (;;) {
    // client address
    struct sockaddr_in client_address;
    int client_address_size;
    client_address_size = sizeof(struct sockaddr_in);
    memset(&client_address, 0, client_address_size);
    // accept client
    printf("waiting to accept...\n");
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_size);
    if (client_fd == -1) {
      printf("failed to accept\n");
      continue;
    }
    printf("accepted...\n");
    // set sockopt
    char yes = 1;
    setsockopt(client_fd, SOL_SOCKET, TCP_NODELAY, &yes, sizeof(yes));
    // reset buffer
    reset_buffer();
  }
  return 0;
}

DWORD WINAPI recv_thread(void* data) {
  char buffer[8192];
  for (;;) {
    // sleep
    usleep(RECV_THREAD_SLEEP_MS * 1000);
    // client not connected yet;
    if (client_fd == -1) {
      continue;
    }
    // receive length
    uint32_t networkMsgDataSize;
    if (recv(client_fd, (char*)&networkMsgDataSize, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t)) {
      printf("recv1 failed errno = %d\n", WSAGetLastError());
      continue;
    }
    uint32_t hostMsgDataSize = ntohl(networkMsgDataSize);
    // receive payload
    if (recv(client_fd, buffer, hostMsgDataSize, MSG_WAITALL) != hostMsgDataSize) {
      printf("recv2 failed errno = %d\n", WSAGetLastError());
      continue;
    }
    // skip ping frames
    if (buffer[0] == 0x00 &&
        buffer[1] == 0x00 &&
        buffer[2] == 0x00 &&
        buffer[3] == 0x00) {
      printf("got ping frame\n");
      continue;
    }
    // push to buffer
    WaitForSingleObject(recv_buffer_mutex, INFINITE);
    PASSTHRU_MSG *buffer_msg = &recv_buffer[recv_buffer_size];
    memset(buffer_msg, 0, sizeof(PASSTHRU_MSG));
    buffer_msg->ProtocolID = CAN;
    buffer_msg->Timestamp = GetTime();
    buffer_msg->DataSize = hostMsgDataSize;
    memcpy(buffer_msg->Data, buffer, hostMsgDataSize);
    // log
    printf("recv_thread: buffer_msg->DataSize = %04x\n", buffer_msg->DataSize);
    // increment
    recv_buffer_size += 1;
    // release
    ReleaseMutex(recv_buffer_mutex);
  }
}

void init_threads() {
  HANDLE accept_handle = CreateThread(NULL, 0, accept_thread, NULL, 0, NULL);
  if (!accept_handle) {
    printf("failed to create accept thread\n");
  }
  HANDLE recv_handle = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
  if (!recv_handle) {
    printf("failed to create accept thread\n");
  }
}

EXPORT PassThruOpen(const void *pName, uint32_t *pDeviceID) {
  printf("PassThruOpen\n");
  *pDeviceID = 1;
  return STATUS_NOERROR;
}

EXPORT PassThruConnect(uint32_t DeviceID, uint32_t ProtocolID,
                               uint32_t Flags, uint32_t Baudrate,
                               uint32_t *pChannelID) {
  printf("PassThruConnect DeviceID = %08x ProtocolID = %08x\n", DeviceID, ProtocolID);
  if (ProtocolID != CAN) {
    return ERR_INVALID_PROTOCOL_ID;
  }
  *pChannelID = 1;
  return STATUS_NOERROR;
}

EXPORT PassThruReadMsgs(uint32_t ChannelID, PASSTHRU_MSG *pMsg,
                                uint32_t *pNumMsgs, uint32_t Timeout) {
  // printf("PassThruReadMsgs *pNumMsgs = %d Timeout = %d\n", *pNumMsgs, Timeout);
  usleep(PASS_THRU_READ_MSGS_SLEEP_MS * 1000);
  WaitForSingleObject(recv_buffer_mutex, INFINITE);
  uint32_t numMsgs = 0;
  // check recv buffer
  if (recv_buffer_size != 0) {
    for (int i = 0; i < recv_buffer_size; ++i) {
      PASSTHRU_MSG *msg = &pMsg[i];
      PASSTHRU_MSG *recv_msg = &recv_buffer[i];
      msg->ProtocolID = recv_msg->ProtocolID;
      msg->RxStatus = recv_msg->RxStatus;
      msg->Timestamp = recv_msg->Timestamp;
      msg->DataSize = recv_msg->DataSize;
      memcpy(msg->Data, recv_msg->Data, msg->DataSize);
      // log
      printf("PassThruReadMsgs: msg->DataSize = %04x\n", msg->DataSize);
      // increment
      numMsgs += 1;
    }
    // set buffer as depleted
    recv_buffer_size = 0;
  }
  ReleaseMutex(recv_buffer_mutex);
  *pNumMsgs = numMsgs;
  return numMsgs == 0 ? ERR_BUFFER_EMPTY : STATUS_NOERROR;
}

EXPORT PassThruWriteMsgs(uint32_t ChannelID, const PASSTHRU_MSG *pMsg,
                                 uint32_t *pNumMsgs, uint32_t Timeout) {
  // client not connected yet
  if (client_fd == -1) {
    *pNumMsgs = 0x00000000;
    return ERR_FAILED;
  }
  printf("PassThruWriteMsgs *pNumMsgs = %d\n", *pNumMsgs);
  uint32_t numMsgsSent = 0;
  for (int i = 0; i < *pNumMsgs; ++i) {
    const PASSTHRU_MSG *msg = &pMsg[i];
    // check for wrong protocol ID
    if (msg->ProtocolID != CAN) {
      *pNumMsgs = numMsgsSent;
      return ERR_MSG_PROTOCOL_ID;
    }
    uint32_t bigEndianMsgDataSize = htonl(msg->DataSize);
    // send length
    if (send(client_fd, (char*)&bigEndianMsgDataSize, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
      printf("send1 failed errno = %d\n", WSAGetLastError());
      *pNumMsgs = numMsgsSent;
      return ERR_FAILED;
    }
    // send payload
    if (send(client_fd, (char*)msg->Data, msg->DataSize, 0) != msg->DataSize) {
      printf("send2 failed errno = %d\n", WSAGetLastError());
      *pNumMsgs = numMsgsSent;
      return ERR_FAILED;
    }
    numMsgsSent += 1;
    // record loopback
    WaitForSingleObject(recv_buffer_mutex, INFINITE);
    PASSTHRU_MSG *loopback_msg = &recv_buffer[recv_buffer_size];
    memset(loopback_msg, 0, sizeof(PASSTHRU_MSG));
    loopback_msg->ProtocolID = msg->ProtocolID;
    loopback_msg->RxStatus = msg->RxStatus | TX_MSG_TYPE;
    loopback_msg->Timestamp = GetTime();
    loopback_msg->DataSize = msg->DataSize;
    memcpy(loopback_msg->Data, msg->Data, msg->DataSize);
    // log
    printf("PassThruWriteMsgs: loopback_msg->DataSize = %04x\n", msg->DataSize);
    // increment
    recv_buffer_size += 1;
    // release
    ReleaseMutex(recv_buffer_mutex);
    // sleep
    usleep(PASS_THRU_WRITE_MSGS_SLEEP_MS * 1000);
  }
  *pNumMsgs = numMsgsSent;
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
  // 3 READ_VBATT?
  // 2 CONFIG_SET?
  printf("PassThruIoctl IoctlID = %08x\n", IoctlID);
  if (IoctlID == READ_VBATT) {
    unsigned int *voltageLevel = (unsigned int*)pOutput;
    *voltageLevel = 12; // TODO: is this format for 12v?
  }
  return STATUS_NOERROR;
}

EXPORT PassThruDisconnect(uint32_t ChannelID) {
  printf("PassThruDisconnect\n");
  sleep(1);
  close_client();
  reset_buffer();
  return STATUS_NOERROR;
}

EXPORT PassThruClose(uint32_t DeviceID) {
  printf("PassThruClose\n");
  sleep(1);
  reset_buffer();
  close_client();
  close_server();
  /*shutdown(DeviceID, SD_BOTH);
  closesocket(DeviceID);
  WSACleanup();*/
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
      recv_buffer_mutex = CreateMutex(NULL, FALSE, NULL);
      init_console();
      init_server();
      init_threads();
      printf("DLL_PROCESS_ATTACH!\n");
      break;
    }
    case DLL_THREAD_ATTACH: {
      printf("DLL_THREAD_ATTACH\n");
      break;
    }
    case DLL_PROCESS_DETACH: {
      close_client();
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
