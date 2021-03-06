#include <stdio.h>
#include <string.h>
#include <xmodem.h>
#include "tests.h"

// These tests are not only used by the Arduino code but they are also used
// by non-arduino code that communicate over two serial ports on the host
// computer.  See the `extras` directory for more information.
//
// serial_write and serial_read must be given to us by the host environment
//
// The macros LOG and LOGLN are configured to be device-independent ways of
// logging info to the user.

void serial_write(int ch);
int serial_read(long ms);

#if defined(NO_ARDUINO)
#include <iostream>
using namespace std;

#define F(x) (x)
#define LOG(x) cout << (x)
#define LOGLN(x) \
  cout << (x); \
  cout << "\n"
#else
#include <Arduino.h>
#define LOG(x) Serial.print(x)
#define LOGLN(x) Serial.println(x)
#endif

const int BUFFER_SIZE = 5048;
static unsigned char buffer[BUFFER_SIZE];

struct alterRule
{
  unsigned char packetNumber;
  int position;
  int action;
  int value;
  bool isHandled;
};

const char ALTER_RULE_MAX = 5;
const char ALTER_RULE_NONE = 0;
const char ALTER_RULE_INSERT = 1;
const char ALTER_RULE_DELETE = 2;
const char ALTER_RULE_CHANGE = 3;

alterRule alterRules[ALTER_RULE_MAX];

bool isPassing;

void resetAlterRules()
{
  for (int i = 0; i < ALTER_RULE_MAX; i++)
  {
    alterRules[i].action = ALTER_RULE_NONE;
    alterRules[i].packetNumber = -1;
    alterRules[i].position = -1;
    alterRules[i].value = -1;
    alterRules[i].isHandled = false;
  }
}

unsigned char getBufferByte(size_t idx)
{
  static unsigned char txt[] = "The quick brown fox";
  return txt[idx % sizeof(txt)];
}

bool compareBuffer(unsigned char *buffer, size_t s)
{
  LOG(F("compareBuffer size: "));
  LOGLN(s);

  for (size_t i = 0; i < s; i++)
  {
    int actual = buffer[i];
    int expected = getBufferByte(i);

    if (actual != expected)
    {
      LOG(F("idx "));
      LOG(i);
      LOG(F("\tActual: "));
      LOG(actual);
      LOG(F("\tExpected: "));
      LOG(expected);
      LOGLN(F(""));

      return false;
    }
  }
  LOGLN(F("Compare buffer passed."));
  return true;
}

void fillBuffer(unsigned char *buffer, size_t s)
{
  LOG(F("fillBuffer size: "));
  LOGLN(s);

  for (size_t i = 0; i < s; i++)
  {
    buffer[i] = getBufferByte(i);
    // LOG((char)buffer[i]);
  }
}

static int currentPacketNumber = 0;
static int send_index = 0;

void tweak_write(int ch)
{
  char handled = 0;

  for (int i = 0; i < ALTER_RULE_MAX; i++)
  {
    alterRule r = alterRules[i];

    if (
      (r.position == send_index) &&
      (r.packetNumber == currentPacketNumber) &&
      (!r.isHandled))
    {

      switch (r.action)
      {
        case ALTER_RULE_DELETE:
          // Do nothing;
          handled = 1;
          LOG(F("****** DELETED BYTE at "));
          LOGLN(r.position);
          break;
        case ALTER_RULE_CHANGE:
          LOG(F("****** CHANGED BYTE at "));
          LOG(r.position);
          LOG(F(" "));
          LOGLN(r.value);

          (*serial_write)(r.value);
          handled = 1;
          break;
        case ALTER_RULE_INSERT:
          LOG(F("****** INSERTED BYTE at "));
          LOGLN(r.position);
          (*serial_write)(ch);
          (*serial_write)(r.value);
          handled = 1;
          break;
      }
      break;
    }
    r.isHandled = true;
  }
  if (!handled)
  {
    (*serial_write)(ch);
  }
  send_index++;
}

void update_packet(XModemPacketStatus status)
{
  LOG((int)status.packetNumber);
  LOG(F("\t"));

  switch (status.action)
  {
    case XMODEM_PACKET_ACTION::Receiving:
      LOG(F("Receiving"));
      break;
    case XMODEM_PACKET_ACTION::Timeout:
      LOG(F("Timeout"));
      break;
    case XMODEM_PACKET_ACTION::PacketNumberCorrupt:
      LOG(F("PacketNumberCorrupt"));
      break;
    case XMODEM_PACKET_ACTION::PacketNumberOutOfSequence:
      LOG(F("PacketNumberOutOfSequence"));
      break;
    case XMODEM_PACKET_ACTION::CrcMismatch:
      LOG(F("CrcMismatch"));
      break;
    case XMODEM_PACKET_ACTION::ChecksomeMismatch:
      LOG(F("ChecksomeMismatch"));
      break;
    case XMODEM_PACKET_ACTION::Accepted:
      LOG(F("Accepted"));
      break;
    case XMODEM_PACKET_ACTION::ValidDuplicate:
      LOG(F("ValidDuplicate"));
      break;
    case XMODEM_PACKET_ACTION::ReceiverACK:
      send_index = 0;
      currentPacketNumber = status.packetNumber + 1;
      LOG(F("ReceiverACK"));
      break;
    case XMODEM_PACKET_ACTION::ReceiverNAK:
      LOG(F("ReceiverNAK"));
      break;
    case XMODEM_PACKET_ACTION::ReceiverGarbage:
      LOG(F("ReceiverGarbage"));
      break;
    case XMODEM_PACKET_ACTION::Sync:
      LOG(F("Sync"));
      break;
    case XMODEM_PACKET_ACTION::SyncError:
      LOG(F("SyncError"));
      break;
    case XMODEM_PACKET_ACTION::Transmitting:
      LOG(F("Transmitting"));
      break;
    case XMODEM_PACKET_ACTION::WaitingForReceiver:
      LOG(F("WaitingForReceiver"));
      break;
    default:
      LOG(F("Unknown Action"));
      break;
  }
  LOGLN(F(""));
}

class XM_ShouldSucceed : public XMTestBase
{
  public:
    static void MasterAction(alterRule *alterRules)
    {
      Xmodem xmodem(serial_read, tweak_write);

      LOGLN(F("Sending..."));
      fillBuffer(buffer, BUFFER_SIZE);
      auto ret = xmodem.transmitFullBuffer(buffer, BUFFER_SIZE, update_packet);
      LOG(F("Transmit result: "));
      LOGLN(ret == XMODEM_TRANSFER_STATUS::SUCCESS);
      if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
      {
        isPassing = false;
      }
    };

    static void SlaveAction()
    {
      Xmodem xmodem(serial_read, serial_write);

      LOGLN(F("Receiving..."));
      auto ret = xmodem.receiveFullBuffer(buffer, BUFFER_SIZE, update_packet);
      LOG(F("Receive result: "));
      LOGLN(ret == XMODEM_TRANSFER_STATUS::SUCCESS);
      if (ret != XMODEM_TRANSFER_STATUS::SUCCESS)
      {
        isPassing = false;
      }

      if (!compareBuffer(buffer, BUFFER_SIZE))
      {
        LOGLN(F("Buffer compare failed"));
        isPassing = false;
      }
    }
};

void testNoGlitches(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-           NO GLITCHES        -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testMissingDataByte(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-     testMissingDataByte      -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();
  alterRules[0].packetNumber = 4;
  alterRules[0].action = ALTER_RULE_DELETE;
  alterRules[0].position = 17;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testExtraDataByte(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-      testExtraDataByte       -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();
  alterRules[0].action = ALTER_RULE_INSERT;
  alterRules[0].packetNumber = 5;
  alterRules[0].position = 17;
  alterRules[0].value = 0xFF;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testChangedByteInPayload(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-   testChangedByteInPayload   -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();
  alterRules[0].action = ALTER_RULE_CHANGE;
  alterRules[0].packetNumber = 1;
  alterRules[0].position = 17;
  alterRules[0].value = 0xFF;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testCorruptPacketNumber(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-    testCorruptPacketNumber   -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();

  alterRules[0].action = ALTER_RULE_CHANGE;
  alterRules[0].packetNumber = 16;
  alterRules[0].position = 1;
  alterRules[0].value = 0xA0;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testCorruptPacketNumberCheck(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("- testCorruptPacketNumberCheck  -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();

  alterRules[0].action = ALTER_RULE_CHANGE;
  alterRules[0].packetNumber = 15;
  alterRules[0].position = 2;
  alterRules[0].value = 0xA0;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testPacketSequenceError(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-   testPacketSequenceError    -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();

  alterRules[0].action = ALTER_RULE_CHANGE;
  alterRules[0].packetNumber = 16;
  alterRules[0].position = 1;
  alterRules[0].value = 0xA0;

  alterRules[1].action = ALTER_RULE_CHANGE;
  alterRules[1].packetNumber = 16;
  alterRules[1].position = 2;
  alterRules[1].value = ~(0xA0);

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testCorreptSohError(bool isMaster)
{
  LOGLN(F("--------------------------------"));
  LOGLN(F("-     testCorreptSohError      -"));
  LOGLN(F("--------------------------------"));

  resetAlterRules();

  alterRules[0].action = ALTER_RULE_CHANGE;
  alterRules[0].packetNumber = 16;
  alterRules[0].position = 0;
  alterRules[0].value = 0xA0;

  if (isMaster)
  {
    XM_ShouldSucceed::MasterAction(alterRules);
  }
  else
  {
    XM_ShouldSucceed::SlaveAction();
  }
}

void testAll(bool isMaster)
{
  isPassing = true;
  if (isPassing)
    testNoGlitches(isMaster);

  if (isPassing)
    testMissingDataByte(isMaster);

  if (isPassing)
    testExtraDataByte(isMaster);

  if (isPassing)
    testChangedByteInPayload(isMaster);

  if (isPassing)
    testCorruptPacketNumber(isMaster);

  if (isPassing)
    testCorruptPacketNumberCheck(isMaster);

  if (isPassing)
    testPacketSequenceError(isMaster);

  if (isPassing)
    testCorreptSohError(isMaster);

  if (isPassing)
  {
    LOGLN(F("--------------------------------"));
    LOGLN(F("-********** Success ********** -"));
    LOGLN(F("--------------------------------"));
  }
  else
  {
    LOGLN(F("--------------------------------"));
    LOGLN(F("- ======= Tests Failed ======= -"));
    LOGLN(F("--------------------------------"));
  }
}
