// #include <driver/adc.h>

namespace ENV {
    const int VOLT_MAX = 3.3;
    const int ANALOG_WIDTH = 11;

    const int RESET = 32;
    const int LEVEL = 34;
    const int BTN = 18;
}

volatile bool INTERRUPT_FLAG = false; // 인터럽트 발생 여부를 저장할 변수

void IRAM_ATTR handleInterrupt()
{
    INTERRUPT_FLAG = true;                      // 인터럽트가 발생했음을 기록
    detachInterrupt(digitalPinToInterrupt(ENV::BTN)); // 인터럽트 비활성화
}

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

    float readLevel(int resetTime, int expTime)
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

        // 오버샘플링 코드
        int level = 0; // A0에서 읽은 값을 저장할 변수

        // A0 핀을 5번 읽어서 합산
        for (byte n = 0; n < 5; n++) {
            level += analogRead(levelPin); // A0 핀에서 값 읽기
            delay(2); // 2밀리초 지연
        }

        // A0에서 읽은 값의 평균 계산
        level = level / 5;

        return ((float)(pow(2, ENV::ANALOG_WIDTH) - level) / pow(2, ENV::ANALOG_WIDTH));
    }
};

Pixel P = Pixel(ENV::RESET, ENV::LEVEL);

int resetTime = 100, expTime = 10;

void setup()
{
    analogSetWidth(ENV::ANALOG_WIDTH);
    Serial.begin(115200);
    pinMode(ENV::RESET, OUTPUT);
    pinMode(ENV::BTN, INPUT_PULLUP);
    pinMode(ENV::LEVEL, INPUT);
    attachInterrupt(digitalPinToInterrupt(ENV::BTN), handleInterrupt, FALLING);
}

void loop()
{
    if (INTERRUPT_FLAG)
    {
        float level = P.readLevel(resetTime, expTime);
        Serial.println(level);

        INTERRUPT_FLAG = false; // 인터럽트 처리 완료 표시
        delay(100);
        attachInterrupt(digitalPinToInterrupt(ENV::BTN), handleInterrupt, FALLING); // 인터럽트 다시 활성화
    }

    if (Serial.available())
    {
        String input = Serial.readStringUntil('\n');
        input.trim(); // 입력 문자열의 앞뒤 공백 제거

        if (input.startsWith("r"))
        {
            Serial.println(resetTime + expTime);
            float level = P.readLevel(resetTime, expTime);
            Serial.println(level);
        }
        else if (input.startsWith("c "))
        {
            input.remove(0, 2);
            int firstSpace = input.indexOf(' ');
            if (firstSpace == -1)
            {
                Serial.println("Invalid command format");
                return;
            }

            String resetTimeStr = input.substring(0, firstSpace);
            String expTimeStr = input.substring(firstSpace + 1);

            // 문자열을 숫자로 변환
            resetTime = resetTimeStr.toInt();
            expTime = expTimeStr.toInt();
            Serial.printf("Changed! resetTime: %d, expTime: %d\n", resetTime, expTime);
        }
        else if (input.startsWith("p")) 
        {
            Serial.printf("Printing configs... resetTime: %d, expTime: %d\n", resetTime, expTime);
        }
        else 
        {
            Serial.println("Invalid command");
        }
    }
}