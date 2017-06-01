# scratch_project

伺服器編輯器
https://www.visualstudio.com/zh-hant/downloads/?rr=https%3A%2F%2Fwww.google.com.tw%2F

Arduino編輯器
https://www.arduino.cc/en/Main/Donate
燒錄wifi模組D1 mini需要的lib:

https://drive.google.com/file/d/0B3rnGrzvGiDkcUFLVFZub25rNVk/view?usp=sharing
(解壓縮後放進Arduino目錄下)

S4A軟體
http://s4a.cat/


程式碼更新請至: 
Server程式碼
scratch_project/TCP_ECHO_SERVER/TCP_ECHO_SERVER.cpp

S4A韌體，用Arduino編輯器燒錄到板子
scratch_project/scratch_server/scratch_server.ino

小朋友的wifi程式碼
scratch_project/tcp_client/tcp_client.ino

連接Arduino板子的wifi程式碼
scratch_project/WiFiSerial.ino


步驟:
1.打開熱點
2.裝置上電(wifi module),熱點軟體會顯示目前連接上的ip
3.打開server.exe,看裝置是否都有連到server
4.確認連上後,Arduino uno接電腦,開S4A
5.點S4A的綠旗,按下按鈕後會正常運作

目前待解問題:
1. 採4號裝置,scratch會收到1號
2. 十秒後歸零後的第一次觸發都會失效(ID亂跳)
