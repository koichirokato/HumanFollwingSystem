<h1>HumanFollwingSystem</h1> 

<h2>「追従中の人の軌跡を予測するRTC」</h2>
  
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

詳細なマニュアルは以下にあります．
https://github.com/koichirokato/HumanFollwingSystem/blob/master/user_manual.md


****
連絡先：
芝浦工業大学 機械機能工学科 知能機械システム研究室  
〒135-8548 東京都江東区豊洲3-7-5  
加藤　宏一朗 Koichiro Kato  
E-mail : ab16035@shibaura-it.ac.jp  
