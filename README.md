
1. nrf9160의 tensorflow example 중에 gesture 기능을 구현한 예제이다
   가속도센서는 lis2dw12를 사용하였다
2. pc용 serial terminal은 teraterm을 사용하였다
   
3. 시리얼터미널 개발이 필요하다면 아래를 참고하라
https://github.com/davepruitt/BetterSerialMonitor

gesture_predictor.cpp
  최근 제스춰 3개의 확률 5개의 평균값 중에 최대값이 kDetectionThreshold 보다 크면 그 제스춰를 인식된 제스춰로 보여준다
  따라서 PredictGesture가 최소한 5번 이상은 수행되어야 한다
  이 후, 3개 제스춰 외의 제스춰가 누적되면 평균값이 리셋되는 효과가 일어나기 때문에 5개 이상의 정상 제스춰가 수행되어야 한다
  따라서 3개 제스춰 중에 하나가 꾸준이 수행되어야지 인식횟수가 많아지고 복합제스춰 또는 범위외 제스춰가 있으면 인식횟수가 급격히 떨어진다

  인식률을 높이려면
  1. 데이터를 많이 학습시키든지
  2. 모델 개량을 하든지
  3. thresholder값 0.8f를 낮추어서 0.6f, 0.7f로 바꾸든지
     또는 histroylength 5를 적절히 조절하여 3 정도로 바꾸든지

  

  
