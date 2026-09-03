
// =====================================================
// CONNECT TO WIFI
// =====================================================

bool connectToWiFi()
{
    // Make sure we have credentials
    if (wifiSSID[0] == '\0')
    {
        Serial.println("WiFi SSID is empty");
        return false;
    }

    Serial.print("Connecting to WiFi: ");
    Serial.println(wifiSSID);

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        wifiSSID,
        wifiPassword);

    const unsigned long timeout = 15000;
    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startTime >= timeout)
        {
            Serial.println();
            Serial.println("WiFi connection timeout");

            WiFi.disconnect();

            return false;
        }

        Serial.print(".");

        delay(250);
    }

    Serial.println();
    Serial.println("WiFi connected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}


// =====================================================
// WIFI STATUS
// =====================================================

bool isWiFiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}