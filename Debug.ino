void psram()
{
    Serial.begin(115200);

    if (psramFound())
    {
        Serial.println("PSRAM FOUND");

        Serial.printf("Total: %u MB\n",
                      ESP.getPsramSize() / 1024 / 1024);
    }
    else
    {
        Serial.println("NO PSRAM");
    }
}