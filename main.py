from control import execute
import numpy as np
import matplotlib.pyplot as plt

brightness_level = int(input("밝기 단계: "))
test_case = int(input("테스트 수: "))
resetTime = input("리셋 시간: ")
expTime = input("노출 시간: ")

data = np.zeros((brightness_level, test_case))
execute(f'c {resetTime} {expTime}')

for i in range(0, brightness_level):
    print(f"밝기 단계 {i+1}. 준비 완료 후 엔터")
    input()
    for j in range(0, test_case):
        level = execute('r')
        print(f"{j+1}번째 시행, 값: {level}")
        data[i][j] = level; 
    print(f"밝기 단계 {i+1} 종료.")


# 각 행의 평균을 구합니다.
averages = np.mean(data, axis=1)

# 막대 그래프를 그리기 위한 함수
def plot_1d_array(data):
    # x 좌표를 준비합니다 (밝기 수준).
    x = np.arange(1, len(data) + 1)
    
    # 막대 그래프를 그립니다.
    plt.bar(x, data, color='b', align='center')
    
    # 그래프의 제목과 축 레이블을 추가합니다.
    plt.title('Average Values Bar Graph')
    plt.xlabel('Brightness Level')
    plt.ylabel('Average Value')
    
    # 그래프를 표시합니다.
    plt.show()

# 함수를 호출하여 그래프를 그립니다.
plot_1d_array(averages)
