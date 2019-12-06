@echo off

echo +++ get computer name +++
FOR /F %%A IN ('hostname.exe') DO SET NAME=%%A
echo %NAME%

timeout /T 3

echo +++ start naming service +++
cd %RTM_ROOT%/bin
start rtm-naming.bat

timeout /T 1

echo +++ Connecting port +++
call rtcon /localhost/%NAME%.host_cxt/URG0.rtc:RangeData                       /localhost/%NAME%.host_cxt/object_tacking_concierge0.rtc:RangeData
call rtcon /localhost/%NAME%.host_cxt/object_tacking_concierge0.rtc:black_ship /localhost/%NAME%.host_cxt/Concierge_Type3_verOLD0.rtc:Velocity2D
call rtcon /localhost/%NAME%.host_cxt/Concierge_Type3_verOLD0.rtc:RobotPose2D  /localhost/%NAME%.host_cxt/object_tacking_concierge0.rtc:RobotPose2D

call rtcon /localhost/%NAME%.host_cxt/Kinect0.rtc:user_info                    /localhost/%NAME%.host_cxt/object_tacking_concierge0.rtc:user_info

timeout /T 3