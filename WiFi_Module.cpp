#include "WiFi_Module.h"

static bool isUnspecifiedIp(IPAddress ip) {
    return ip == IPAddress(0, 0, 0, 0);
}

bool WiFi_Module::TryConnect(String ssid, String password, IPAddress local_ip, IPAddress gateway, IPAddress subnet) {
    bool wifi_connected = false;

    bool wifi_configured = ssid.length() > 0;
    if (wifi_configured) {
        WiFi.persistent(false);
        WiFi.disconnect(true);  //disconnect from wifi to set new wifi connection
        WiFi.mode(WIFI_STA); //init wifi mode
        WiFi.setSleep(false);

        // Set static IP, gateway and subnet only when requested. 0.0.0.0 keeps DHCP enabled.
        if (!isUnspecifiedIp(local_ip)) {
            Serial.println("Configuring static IP...");
            if (!WiFi.config(local_ip, gateway, subnet)) {
                Serial.println("STA Failed to configure");
            }
        } else {
            Serial.println("Using DHCP network configuration...");
        }

        // Connect using WPA personal
        WiFi.begin(ssid.c_str(), password.c_str());

        int ctr = 10;
        while (WiFi.status() != WL_CONNECTED && ctr-- > 0) {
            delay(500);
        }

        wifi_connected = ctr >= 0;
    }

    return wifi_connected;
}

IPAddress WiFi_Module::SetupAccessPoint()
{
    // Run device as access point with SSID "SigGen"
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);

    IPAddress ap_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);
    WiFi.softAPConfig(ap_ip, gateway, subnet);

    if (!WiFi.softAP("SigGen")) {
        Serial.println("Failed to start access point.");
        return IPAddress(0, 0, 0, 0);
    }

    return WiFi.softAPIP();
}
