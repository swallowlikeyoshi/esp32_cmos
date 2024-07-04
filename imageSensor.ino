#include "interrupt.h"
#include "Pixel.h"

Pixel P = Pixel(ENV::RESET, ENV::LEVEL);

int resetTime = 100;
int expTime = 10;

void setup()
{
    Serial.begin(115200);

    pinMode(ENV::RESET, OUTPUT);
    pinMode(ENV::BTN, INPUT_PULLUP);
    pinMode(ENV::LEVEL, INPUT);

    analogSetWidth(ENV::ANALOG_WIDTH);

    attachInterrupt(digitalPinToInterrupt(ENV::BTN), handleInterrupt, FALLING);
}

void loop()
{
    if (INTERRUPT_FLAG)
    {
        float level = P.getExposedLevel(resetTime, expTime);
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
            float level = P.getExposedLevel(resetTime, expTime);
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
        else if (input.startsWith("t "))
        {
            input.remove(0, 2);
            int firstSpace = input.indexOf(' ');
            int secondSpace = input.lastIndexOf(' ');
            if (firstSpace == -1 || secondSpace == -1 || firstSpace == secondSpace)
            {
                Serial.println("Invalid command format");
                return;
            }

            String resetTimeStr = input.substring(0, firstSpace);
            String expTimeStr = input.substring(firstSpace + 1, secondSpace);
            String stepStr = input.substring(secondSpace + 1);

            int reset = resetTimeStr.toInt();
            int exp = expTimeStr.toInt();
            int step = stepStr.toInt();

            Serial.printf("reset: %d, exp: %d, step: %d\n", reset, exp, step);

            float *array = new float[step];
            // chargingGraph(P, array, reset, exp, step);

            memset(array, 0, sizeof(float) * step); // 배열 초기화
            int duration = exp / step;

            P.resetCap(reset);
            for (int i = 0; i < step; i++)
            {
                array[i] = P.convertLevel(P.readLevel());
                delay(duration);
            }

            for (int i = 0; i < step; i++)
            {
                Serial.printf("%f ", array[i]);
                Serial.println();
            }
            delete[] array; // 동적 배열 메모리 해제
        }
        else
        {
            Serial.println("Invalid command");
        }
    }
}