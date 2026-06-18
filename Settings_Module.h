#ifndef Settings_Module_h
#define Settings_Module_h

#include "EEPROM.h"

#define STRBUF_LEN 256
#define IPADDR_LEN 16
#define EEPROM_SIZE 1000
#define EEPROM_MAGIC 0x53494731UL
#define EEPROM_MAGIC_ADDRESS 0
#define SETTINGS_TABLE_ADDRESS sizeof(uint32_t)

#define MAX_NUM_ENTRIES 256

class Settings_Module {
    public:
        Settings_Module(int num_handles);
        ~Settings_Module();
        bool IsInitialized();
        void Reset();
        void StoreString(int handle, String value);
        void StoreULong(int handle, unsigned long value);
        void StoreIp(int handle, IPAddress value);
        void Commit();
        void LoadString(int handle, String* dest);
        void LoadULong(int handle, unsigned long* dest);
        void LoadIp(int handle, IPAddress* value);
    private:
        int addresses[MAX_NUM_ENTRIES];
        int current_address = SETTINGS_TABLE_ADDRESS;
        int num_handles = 0;
        bool initialized = true;
        bool hasAddress(int handle, int length);
        void ensureAddress(int handle, int length);
        void registerAddress(int handle, int length);
};

#endif
