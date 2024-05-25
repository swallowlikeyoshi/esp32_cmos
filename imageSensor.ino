const int RESET = 32;
const int COLUMN = 34;
const int BTN = 18;

volatile bool interruptTriggered = false; // 인터럽트 발생 여부를 저장할 변수

// 인터럽트 서비스 루틴 함수
void IRAM_ATTR handleInterrupt()
{
    interruptTriggered = true;                   // 인터럽트가 발생했음을 기록
    detachInterrupt(digitalPinToInterrupt(BTN)); // 인터럽트 비활성화
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
        if (interruptTriggered)
        {
            Serial.print("Resetting... resetTime: ");
            Serial.println(resetTime);
        }
        delay(resetTime);
        digitalWrite(resetPin, LOW);
        if (interruptTriggered)
        {
            Serial.println("Reset complete.");
        }
    }

    float readLevel(int resetTime, int expTime)
    {
        this->resetCap(resetTime);
        if (interruptTriggered)
        {
            Serial.print("Exposing... expTime: ");
            Serial.println(expTime);
        }
        delay(expTime);
        if (interruptTriggered)
        {
            Serial.println("Expose complete.");
        }
        int level = analogRead(levelPin);
        return ((4096 - level) / 4096.0) * 3.3;
    }
};

void setup()
{
    Serial.begin(115200);
    pinMode(RESET, OUTPUT);
    pinMode(BTN, INPUT_PULLUP);
    pinMode(COLUMN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN), handleInterrupt, FALLING);
}


Pixel P = Pixel(RESET, COLUMN);
int resetTime = 3000, expTime = 5000;

void loop()
{
    if (interruptTriggered)
    {
        float level = P.readLevel(resetTime, expTime);
        Serial.println(level);

        interruptTriggered = false; // 인터럽트 처리 완료 표시
        delay(100);
        attachInterrupt(digitalPinToInterrupt(BTN), handleInterrupt, FALLING); // 인터럽트 다시 활성화
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