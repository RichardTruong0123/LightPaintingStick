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

void sdCardType()
{
    uint8_t cardType = SD.cardType();

    Serial.printf("Card type value: %d\n", cardType);

    switch (cardType)
    {
        case CARD_NONE:
            Serial.println("No SD card");
            break;

        case CARD_MMC:
            Serial.println("MMC card");
            break;

        case CARD_SD:
            Serial.println("SDSC card");
            break;

        case CARD_SDHC:
            Serial.println("SDHC card");
            break;

        default:
            Serial.println("Unknown card type");
            break;
    }

    Serial.printf("Card size: %llu\n",
                (unsigned long long)SD.cardSize());

    Serial.printf("Filesystem total: %llu\n",
                (unsigned long long)SD.totalBytes());

    Serial.printf("Filesystem used: %llu\n",
                (unsigned long long)SD.usedBytes());
}