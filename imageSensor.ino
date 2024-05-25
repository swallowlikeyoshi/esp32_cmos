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
        Serial.print("Resetting... resetTime: ");
        Serial.println(resetTime);
        delay(resetTime);
        digitalWrite(resetPin, LOW);
        Serial.println("Reset complete.");
    }

    float readLevel(int resetTime, int expTime)
    {
        this->resetCap(resetTime);
        Serial.print("Exposing... expTime: ");
        Serial.println(expTime);
        delay(expTime);
        Serial.println("Expose complete.");
        int level = analogRead(levelPin);
        Serial.println("Signal level recorded.");
        return ((4096 - level) / 4096.0) * 3.3;
    }
};

void
setup()
{
    Serial.begin(115200);
    pinMode(RESET, OUTPUT);
    pinMode(BTN, INPUT_PULLUP);
    pinMode(COLUMN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN), handleInterrupt, FALLING);
    Serial.println("initialized!");
}

void loop()
{
    if (interruptTriggered)
    {
        Pixel P = Pixel(RESET, COLUMN);
        float level = P.readLevel(3000, 5000);
        Serial.println(level);

        interruptTriggered = false; // 인터럽트 처리 완료 표시
        delay(100);
        attachInterrupt(digitalPinToInterrupt(BTN), handleInterrupt, FALLING); // 인터럽트 다시 활성화
    }
}