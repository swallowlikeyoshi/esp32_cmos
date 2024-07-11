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

        #ifdef env1

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

        #endif

        #ifdef env2

        if (INTERRUPT_FLAG) {
            Serial.print("Exposing... expTime: ");
            Serial.println(expTime);
        }
        
        // C1, C2 초기화 -> 0V
        digitalWrite(ENV::S1, HIGH);
        digitalWrite(ENV::S2, HIGH);
        digitalWrite(ENV::PC, HIGH);

        delay(resetTime);

        // S2 -> kTC noise level 저장됨.
        digitalWrite(ENV::S2, LOW);

        // 필요 없을 듯
        // delay(expTime / 4);

        // 노출 시작
        digitalWrite(ENV::TG, HIGH);

        delay(expTime);

        // 노출 종료
        digitalWrite(ENV::TG, LOW);

        // 필요 없을 듯
        // delay(expTime / 4);

        // S1 -> signal + kTC noise level 저장됨.
        digitalWrite(ENV::S1, LOW);

        digitalWrite(ENV::PC, LOW);

        // reset level 읽기
        digitalWrite(ENV::S2, HIGH);
        
        float reset_level = readLevel();

        digitalWrite(ENV::S2, LOW);
        
        // signal + reset level 읽기
        digitalWrite(ENV::S1, HIGH);

        float signal_level = readLevel();

        digitalWrite(ENV::S1, LOW);

        #endif

        return convertLevel(signal_level) - convertLevel(reset_level);
    }
};