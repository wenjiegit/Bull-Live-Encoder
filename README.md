Bull-Live-Encoder
=================
Bull Live Encoder is a tool for pushing rtmp to server,<br/>
which supports H264 + AAC and supports ingest from camer or desktop or some windows.<br/>
<br/>
Blog: BLE(http://wenjiegit.github.io/Bull-Live-Encoder/)<br/>
<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_001.jpg)<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_002.jpg)<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_003.jpg)<br/>
<br/>

donate
=================
if you want, you could donate me to help ble better. ^_^<br/>
site: http://wenjiegit.github.io/ble_release/donate.html<br/>

Features
=================
1.  push video(H.264) and audio(AAC) to rtmp server.<br/>
2.  ingest from camera.<br/>
3.  ingest from grab desktop.<br/>
4.  ingest from picture.<br/>
5.  ingest from video file using ffmpeg SDK.<br/>
6.  support all of sources combination.<br/>
7.  support skin change.<br/>
8.  start and stop quickly.<br/>
9.  support B frame with libx264.<br/>
10. support CBR and VBR mode.
11. support multi-thread encoding.
12. support skin change, with color or image.

BLE Download
=================
download: BLE(http://wenjiegit.github.io/ble_release/download.html)

Build Env
=================
1. windows<br/>
    Qt 5.3.1 with mingw 4.82 or above.<br/>
    with librtmp latest.<br/>
    with opencv 2.48 or above.<br/>
    with libx264.<br/>
    with libfaac.<br/>
    with RtAudio.<br/>

2. Linux with GUI<br/>
    coming soon...<br/>

3. Mac<br/>
    coming soon...<br/>

plan
=================
1.  support multi-scene, and change scene quickly.
2.  support ingest from process, eg, from game on windows platform.
3.  support push htpp + flv stream to Bull-Live-Server ,which support this stream pushing.\
4.  support record to flv file.
5.	support play rtmp stream on windows platform using flash tech.

history
=================
[0.1]&nbsp;&nbsp;2014-07-05&nbsp;&nbsp;&nbsp;init project.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support camera source.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support window grab source.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support picture source.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support picture composition.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support H264 encoding use libx264.<br/>
[0.1]&nbsp;&nbsp;2014-08-04&nbsp;&nbsp;&nbsp;support transfer rtmp to media server.<br/>
[0.1]&nbsp;&nbsp;2014-08-05&nbsp;&nbsp;&nbsp;add script on windows.<br/>
[0.1]&nbsp;&nbsp;2014-08-15&nbsp;&nbsp;&nbsp;support AAC Encode<br/>
[0.1]&nbsp;&nbsp;2014-08-17&nbsp;&nbsp;&nbsp;support file source(without sound supported)<br/>
[0.1]&nbsp;&nbsp;2014-10-07&nbsp;&nbsp;&nbsp;support type B frame.<br/>
[0.1]&nbsp;&nbsp;2014-10-12&nbsp;&nbsp;&nbsp;make libx264 thread count and b frame to option.<br/>
[0.1]&nbsp;&nbsp;2014-10-12&nbsp;&nbsp;&nbsp;make start and stop action more quick.<br/>
[0.1]&nbsp;&nbsp;2014-10-12&nbsp;&nbsp;&nbsp;support to choose encode quality.<br/>
[0.1]&nbsp;&nbsp;2014-10-12&nbsp;&nbsp;&nbsp;support color composition with win7(or above) aero.<br/>
[0.1]&nbsp;&nbsp;2014-12-25&nbsp;&nbsp;&nbsp;fix timestamp from qint64 to double.<br/>
