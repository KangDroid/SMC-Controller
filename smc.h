#ifndef __SMC_H__
#define __SMC_H__

/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull 
 * Portions Copyright (C) 2013 Michael Wilber
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <IOKit/IOKitLib.h>
#include <libkern/OSAtomic.h>

using namespace std;

#define CMD_TOOL
#define VERSION               "0.01"

#define OP_NONE               0
#define OP_LIST               1 
#define OP_READ               2
#define OP_READ_FAN           3
#define OP_WRITE              4
#define OP_READ_TEMPS         5

#define KERNEL_INDEX_SMC      2

#define SMC_CMD_READ_BYTES    5
#define SMC_CMD_WRITE_BYTES   6
#define SMC_CMD_READ_INDEX    8
#define SMC_CMD_READ_KEYINFO  9
#define SMC_CMD_READ_PLIMIT   11
#define SMC_CMD_READ_VERS     12

#define DATATYPE_FLT          "flt "
#define DATATYPE_FP1F         "fp1f"
#define DATATYPE_FP4C         "fp4c"
#define DATATYPE_FP5B         "fp5b"
#define DATATYPE_FP6A         "fp6a"
#define DATATYPE_FP79         "fp79"
#define DATATYPE_FP88         "fp88"
#define DATATYPE_FPA6         "fpa6"
#define DATATYPE_FPC4         "fpc4"
#define DATATYPE_FPE2         "fpe2"

#define DATATYPE_SP1E         "sp1e"
#define DATATYPE_SP3C         "sp3c"
#define DATATYPE_SP4B         "sp4b"
#define DATATYPE_SP5A         "sp5a"
#define DATATYPE_SP69         "sp69"
#define DATATYPE_SP78         "sp78"
#define DATATYPE_SP87         "sp87"
#define DATATYPE_SP96         "sp96"
#define DATATYPE_SPB4         "spb4"
#define DATATYPE_SPF0         "spf0"

#define DATATYPE_UINT8        "ui8 "
#define DATATYPE_UINT16       "ui16"
#define DATATYPE_UINT32       "ui32"

#define DATATYPE_SI8          "si8 "
#define DATATYPE_SI16         "si16"

#define DATATYPE_FLT          "flt "

#define DATATYPE_PWM          "{pwm"

#define KEY_INFO_CACHE_SIZE 100

typedef struct {
    char                  major;
    char                  minor;
    char                  build;
    char                  reserved[1]; 
    UInt16                release;
} SMCKeyData_vers_t;

typedef struct {
    UInt16                version;
    UInt16                length;
    UInt32                cpuPLimit;
    UInt32                gpuPLimit;
    UInt32                memPLimit;
} SMCKeyData_pLimitData_t;

typedef struct {
    UInt32                dataSize;
    UInt32                dataType;
    char                  dataAttributes;
} SMCKeyData_keyInfo_t;

typedef unsigned char              SMCBytes_t[32];

static UInt8 fannum[] = "0123456789ABCDEFGHIJ";

typedef struct {
  UInt32                  key; 
  SMCKeyData_vers_t       vers; 
  SMCKeyData_pLimitData_t pLimitData;
  SMCKeyData_keyInfo_t    keyInfo;
  char                    result;
  char                    status;
  char                    data8;
  UInt32                  data32;
  SMCBytes_t              bytes;
} SMCKeyData_t;

typedef char              UInt32Char_t[5];

typedef struct {
  UInt32Char_t            key;
  UInt32                  dataSize;
  UInt32Char_t            dataType;
  SMCBytes_t              bytes;
} SMCVal_t;

typedef struct {
    string temperature_watch;
    int found_idx;
} PredefTemp_t;

struct {
    UInt32 key;
    SMCKeyData_keyInfo_t keyInfo;
} g_keyInfoCache[KEY_INFO_CACHE_SIZE];

class SMC {
private:
    int g_keyInfoCacheCount;
    OSSpinLock g_keyInfoSpinLock;
    io_connect_t g_conn;
    PredefTemp_t temp_watch;
public:
    #pragma mark Init Functions

    SMC(string to_watch);
    SMC();
    void find_index_temp();

    ~SMC();

    #pragma mark C Helpers

    UInt32 _strtoul(char *str, int size, int base);
    void _ultostr(char *str, UInt32 val);
    float _strtof(unsigned char *str, int size, int e);
    void printFLT(SMCVal_t val);
    void printFP1F(SMCVal_t val);
    void printFP4C(SMCVal_t val);
    void printFP5B(SMCVal_t val);
    void printFP6A(SMCVal_t val);
    void printFP79(SMCVal_t val);
    void printFP88(SMCVal_t val);
    void printFPA6(SMCVal_t val);
    void printFPC4(SMCVal_t val);
    void printFPE2(SMCVal_t val);
    void printUInt(SMCVal_t val);
    void printSP1E(SMCVal_t val);
    void printSP3C(SMCVal_t val);
    void printSP4B(SMCVal_t val);
    void printSP5A(SMCVal_t val);
    void printSP69(SMCVal_t val);
    void printSP78(SMCVal_t val);
    void printSP87(SMCVal_t val);
    void printSP96(SMCVal_t val);
    void printSPB4(SMCVal_t val);
    void printSPF0(SMCVal_t val);
    void printSI8(SMCVal_t val);
    void printSI16(SMCVal_t val);
    void printPWM(SMCVal_t val);
    void printBytesHex(SMCVal_t val);
    void printVal(SMCVal_t val);

    #pragma mark Shared SMC functions
    kern_return_t SMCOpen(io_connect_t *conn);
    kern_return_t SMCClose(io_connect_t conn);
    kern_return_t SMCCall2(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure,io_connect_t conn);
    // Provides key info, using a cache to dramatically improve the energy impact of smcFanControl
    kern_return_t SMCGetKeyInfo(UInt32 key, SMCKeyData_keyInfo_t* keyInfo, io_connect_t conn);
    kern_return_t SMCReadKey2(UInt32Char_t key, SMCVal_t *val,io_connect_t conn);

    #pragma mark Command line only
    void smc_init();
    void smc_close();
    kern_return_t SMCCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure);
    kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t *val);
    kern_return_t SMCWriteKey2(SMCVal_t writeVal, io_connect_t conn);
    kern_return_t SMCWriteKey(SMCVal_t writeVal);
    UInt32 SMCReadIndexCount(void);
    kern_return_t SMCPrintAll(void);
    //Fix me with other types
    float getFloatFromVal(SMCVal_t val);
    kern_return_t SMCPrintFans(void);
    kern_return_t SMCPrintTemps(void);
    float SMCGetTemp();
    void usage(char* prog);
    kern_return_t SMCWriteSimple(UInt32Char_t key, char *wvalue, io_connect_t conn);
    void set_temp_watch(string what);
};

#endif // __SMC_H__