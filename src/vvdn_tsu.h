#ifndef VVDN_PTPD
#define VVDN_PTPD

#include "ptpd.h"

#define ToD_REG 0x80060000
#define ToD_VER 0
#define ToD_S_LO_OFF 0x4
#define ToD_S_HI_OFF 0x8
#define ToD_EN 0xC

#define DELAY_REQ_TX_TS_ADDR 0x80020000
#define SYNC_RX_TS_ADDR 0x80030000

#define PTP_DELAY_ADJ 0x80060018

#define VERSION 0x0
#define SEC_LO 0x4
#define SEC_HI 0x8
#define NSEC 0xc
#define TAG 0x10


#define PAGE_SIZE 4096

#define MODE_TX 0
#define MODE_RX 1


#define VVDN
#define RUNTIME_DEBUG
#endif
