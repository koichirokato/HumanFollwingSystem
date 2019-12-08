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

概要は以下にあります．  
https://github.com/koichirokato/HumanFollwingSystem/blob/master/HumanFollowingSystemPPT.pptx  

詳細なマニュアルは以下にあります．  
https://github.com/koichirokato/HumanFollwingSystem/blob/master/user_manual.md  

RTCのソースコードは以下にあります．  
https://github.com/koichirokato/HumanFollwingSystem/tree/master/workspace  

変更履歴は以下にあります．  
https://github.com/koichirokato/HumanFollwingSystem/releases  

****
連絡先：
芝浦工業大学 機械機能工学科 知能機械システム研究室  
〒135-8548 東京都江東区豊洲3-7-5  
加藤　宏一朗 Koichiro Kato  
E-mail : ab16035@shibaura-it.ac.jp  

東京女子大学 数理科学科 加藤研究室  
〒167-8585 東京都杉並区善福寺2-6-1  
赤羽根 里奈 Rina Akabane  
E-mail : d19m201@cis.twcu.ac.jp  