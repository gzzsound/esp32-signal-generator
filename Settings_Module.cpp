#include "Settings_Module.h"

static String readStringBounded(int address, int maxLength) {
    String value = "";
    if (address < 0 || address >= EEPROM_SIZE) {
        return value;
    }

    for (int i = 0; i < maxLength && address + i < EEPROM_SIZE; i++) {
        char c = (char)EEPROM.read(address + i);
        if (c == '\0' || c == (char)0xff) {
            break;
        }
        value += c;
    }

    return value;
}

Settings_Module::Settings_Module(int num_handles) {
    this->num_handles = num_handles;

    for (int i = 0; i < MAX_NUM_ENTRIES; i++) {
        addresses[i] = -1;
    }

    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("Failed to initialise EEPROM");
    }

    initialized = EEPROM.readULong(EEPROM_MAGIC_ADDRESS) == EEPROM_MAGIC;
    int first_data_address = SETTINGS_TABLE_ADDRESS + num_handles * (int)sizeof(int32_t);

    for (int i = 0; i < num_handles; i++)
    {
        addresses[i] = EEPROM.readInt(current_address);
        current_address += sizeof(int32_t);

        if (addresses[i] < first_data_address || addresses[i] >= EEPROM_SIZE) {
            addresses[i] = -1;
            initialized = false;
        }
    }
}

bool Settings_Module::IsInitialized() {
    return initialized;
}

void Settings_Module::Reset() {
    current_address = SETTINGS_TABLE_ADDRESS + num_handles * (int)sizeof(int32_t);
    initialized = false;

    for (int i = 0; i < num_handles; i++) {
        addresses[i] = -1;
        EEPROM.writeInt(SETTINGS_TABLE_ADDRESS + i * sizeof(int32_t), addresses[i]);
    }
}

bool Settings_Module::hasAddress(int handle, int length) {
    return handle >= 0
        && handle < num_handles
        && addresses[handle] >= SETTINGS_TABLE_ADDRESS + num_handles * (int)sizeof(int32_t)
        && addresses[handle] + length <= EEPROM_SIZE;
}

void Settings_Module::ensureAddress(int handle, int length) {
    if (!hasAddress(handle, length)) {
        registerAddress(handle, length);
    }
}

void Settings_Module::registerAddress(int handle, int length) {
    addresses[handle] = current_address;
    current_address += length;
    EEPROM.writeInt(SETTINGS_TABLE_ADDRESS + handle * sizeof(int32_t), addresses[handle]);
}

void Settings_Module::StoreString(int handle, String value) {
    ensureAddress(handle, STRBUF_LEN);
    EEPROM.writeString(addresses[handle], value);
}

void Settings_Module::StoreULong(int handle, unsigned long value) {
    ensureAddress(handle, sizeof(unsigned long));
    EEPROM.writeULong(addresses[handle], value);
}

void Settings_Module::StoreIp(int handle, IPAddress value) {
    ensureAddress(handle, IPADDR_LEN);
    EEPROM.writeString(addresses[handle], value.toString());
}

void Settings_Module::Commit() {
    EEPROM.writeULong(EEPROM_MAGIC_ADDRESS, EEPROM_MAGIC);
    EEPROM.commit();
}

void Settings_Module::LoadString(int handle, String* dest) {
    *dest = readStringBounded(addresses[handle], STRBUF_LEN);
}

void Settings_Module::LoadULong(int handle, unsigned long* dest) {
    if (addresses[handle] < 0 || addresses[handle] + (int)sizeof(unsigned long) > EEPROM_SIZE) {
        *dest = 0;
        return;
    }
    *dest = EEPROM.readULong(addresses[handle]);
}

void Settings_Module::LoadIp(int handle, IPAddress* value) {
    String buf = readStringBounded(addresses[handle], IPADDR_LEN);
    value->fromString(buf);
}
