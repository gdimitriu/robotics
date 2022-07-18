#ifndef __RFID_H__
#define __RFID_H__

  void initRFID();
  bool isCardPresent();
  void readRFID(char *inData);

#endif
