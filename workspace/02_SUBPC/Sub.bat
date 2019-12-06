@echo off

echo +++ get MAIN PC IP AND NAME +++
for /F "delims== tokens=1,2" %%i in (Config.ini) do (
  set %%i=%%j
)
echo %MAINIP%
echo %MAINNAME%

timeout /T 1

echo +++ get SUB PC name +++
FOR /F %%A IN ('hostname.exe') DO SET SUBNAME=%%A
echo %SUBNAME%

timeout /T 1

echo +++ start naming service +++
cd %RTM_ROOT%/bin
start rtm-naming.bat

timeout /T 1

echo +++ start TrajectoryPrediction +++
cd C:/workspace/TrajectoryPrediction
start python TrajectoryPrediction.py

timeout /T 12

echo +++ Connecting port +++
call rtcon /%MAINIP%/%MAINNAME%.host_cxt/object_tacking_concierge0.rtc:HumanPoint           /localhost/%SUBNAME%.host_cxt/TrajectoryPrediction0.rtc:HumanPoint
call rtcon /%MAINIP%/%MAINNAME%.host_cxt/object_tacking_concierge0.rtc:PredictionHumanPoint /localhost/%SUBNAME%.host_cxt/TrajectoryPrediction0.rtc:PredictionHumanPoint

timeout /T 5

echo +++ Activate +++
call rtact /%MAINIP%/%MAINNAME%.host_cxt/object_tacking_concierge0.rtc
call rtact /%MAINIP%/%MAINNAME%.host_cxt/URG0.rtc
call rtact /%MAINIP%/%MAINNAME%.host_cxt/Kinect0.rtc
call rtact /%MAINIP%/%MAINNAME%.host_cxt/Concierge_Type3_verOLD0.rtc
call rtact /localhost/%SUBNAME%.host_cxt/TrajectoryPrediction0.rtc

timeout /T 3