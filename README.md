<h1>HumanFollwingSystem</h1>

***  

<h2>「追従中の人の軌跡を予測するRTC」</h2>

***  

本システムは次の5つのRTCから構成されています．  

- URG RTC
  - URG-04LXのセンサデータを取得するRTC
- Kinect RTC
  - Xtion Pro Live のセンサデータを取得するRTC
- object_tracking_concierge RTC
  - センサデータから人の位置を計算し台車に速度指令を送るRTC
- Concierge_Type3_verOLD RTC
  - 速度指令を受け取り，移動台車を動かすRTC
- TrajectoryPrediction RTC
  - 今回開発した追従対象者の軌跡を予測するRTC

