#include "interrupt.h"

class Pixel
{
private:
    int pixelId;
    int resetPin;
    int levelPin;

public:
    Pixel(int resetPin, int levelPin) : resetPin(resetPin), levelPin(levelPin) {}

    Pixel(int pixelId, int resetPin, int levelPin) : pixelId(pixelId), resetPin(resetPin), levelPin(levelPin) {}

    void resetCap(int resetTime)
    {
        digitalWrite(resetPin, HIGH);
        if (INTERRUPT_FLAG)
        {
            Serial.print("Resetting... resetTime: ");
            Serial.println(resetTime);
        }
        delay(resetTime);
        digitalWrite(resetPin, LOW);
        if (INTERRUPT_FLAG)
        {
            Serial.println("Reset complete.");
        }
    }

    float readLevel()
    {
        // 오버샘플링 코드
        float level = 0; // A0에서 읽은 값을 저장할 변수
        // A0 핀을 5번 읽어서 합산
        for (byte n = 0; n < 5; n++)
        {
            level += analogRead(levelPin); // A0 핀에서 값 읽기
            // delay(2); // 2밀리초 지연
        }
        // A0에서 읽은 값의 평균 계산
        level = level / 5;
        return level;
    }

    float convertLevel(float level)
    {
        return ((float)(pow(2, ENV::ANALOG_WIDTH) - level) / pow(2, ENV::ANALOG_WIDTH));
    }

    float getExposedLevel(int resetTime, int expTime)
    {
        this->resetCap(resetTime);

        if (INTERRUPT_FLAG)
        {
            Serial.print("Exposing... expTime: ");
            Serial.println(expTime);
        }
        delay(expTime);
        if (INTERRUPT_FLAG)
        {
            Serial.println("Expose complete.");
        }

        float level = readLevel();

        return convertLevel(level);
    }
};