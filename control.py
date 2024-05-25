import serial
import time

# 시리얼 포트 설정
py_serial = serial.Serial(
    port='COM6',
    baudrate=115200,
    timeout=1  # 1초 대기
)

# 시리얼 포트를 초기화하고 플러시
py_serial.flushInput()
py_serial.flushOutput()

def execute(command):
    py_serial.write('i\n'.encode())
    time.sleep(0.1)
    if py_serial.in_waiting:
        py_serial.read(py_serial.in_waiting)

    # 사용자 입력 받기
    py_serial.write((command + '\n').encode())
    time.sleep(0.1)

    # 응답 읽기
    if (command == 'r'):
        if py_serial.readable():
            delayTime = py_serial.readline()
            time.sleep(float(delayTime) / 1000)
            res = py_serial.readline()
            return res.decode().strip()
    else:
        if py_serial.in_waiting:
            res = py_serial.readline()
            return res.decode().strip()

if __name__ == "__main__":
    try:
        while True:
            # 초기화 명령어 'i' 전송 및 응답 플러시
            py_serial.write('i\n'.encode())
            time.sleep(0.1)
            if py_serial.in_waiting:
                py_serial.read(py_serial.in_waiting)

            # 사용자 입력 받기
            command = input('cmd: ')
            py_serial.write((command + '\n').encode())
            time.sleep(0.1)

            # 응답 읽기
            if (command == 'r'):
                if py_serial.readable():
                    delayTime = py_serial.readline()
                    time.sleep(float(delayTime) / 1000)
                    res = py_serial.readline()
                    print(res.decode().strip())
            else:
                if py_serial.in_waiting:
                    res = py_serial.readline()
                    print(res.decode().strip())

    except KeyboardInterrupt:
        print("종료")

    finally:
        # 시리얼 포트 닫기
        py_serial.close()
