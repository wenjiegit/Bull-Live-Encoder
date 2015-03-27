Bull-Live-Encoder
=================
Bull Live Encoder is a tool for pushing rtmp to server,<br/>
which supports H264 + AAC or H264 + MP3 and supports ingest from camera or desktop or some windows or other.<br/>
similar tools : obs or flash media live encoder.<br/>
<br/>
Blog: BLE(http://wenjiegit.github.io/Bull-Live-Encoder/)<br/>
<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_001.png)<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_002.png)<br/>
![preview](https://github.com/wenjiegit/Bull-Live-Encoder/blob/master/trunk/images/ble_003.png)<br/>
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
5.  ingest from video file using VLC SDK.<br/>
6.  support all of sources combination.<br/>
7.  support skin change.<br/>
8.  start and stop quickly.<br/>
9.  support B frame with libx264.<br/>
10. support CBR and VBR mode.
11. support multi-thread encoding.
12. support skin change, with color or image.
13. support MP3 encode with 11K 22K  44K sample rate.
14. support record to flv file.
15. support ingest from RTSP with libvlc.
16. support ingest from RTMP with libvlc.
17. support ingest from RTP with libvlc.
18. support ingest from HLS with libvlc.
19. support paint rgba to rgb24.
20. support gif movie.

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
	with libmp3lame.<br/>
	with libvlc.<br/>

2. Linux with GUI<br/>
    coming soon...<br/>

3. Mac<br/>
    coming soon...<br/>

plan
=================
1.  support multi-scene, and change scene quickly.
2.  support ingest from process, eg, from game on windows platform.
3.  support push htpp + flv stream to Bull-Live-Server ,which support this stream pushing.
4.	support play rtmp stream on windows platform using flash tech.

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
[0.9.5]&nbsp;&nbsp;2015-02-03&nbsp;&nbsp;&nbsp;support mp3 encode.<br/>
[0.9.5]&nbsp;&nbsp;2015-02-03&nbsp;&nbsp;&nbsp;using a perfect method to build timestamp to make vlc play well.<br/>
[0.9.5]&nbsp;&nbsp;2015-02-03&nbsp;&nbsp;&nbsp;fix fps show problems in vlc.<br/>
[0.9.6]&nbsp;&nbsp;2015-02-08&nbsp;&nbsp;&nbsp;ensure the audio pkt to be not empty.<br/>
[0.9.6]&nbsp;&nbsp;2015-02-08&nbsp;&nbsp;&nbsp;add record UI.<br/>
[0.9.8]&nbsp;&nbsp;2015-02-09&nbsp;&nbsp;&nbsp;support flv record now.<br/>
[0.9.9]&nbsp;&nbsp;2015-02-11&nbsp;&nbsp;&nbsp;support specfy B frame count in settings file .<br/>
[0.9.9]&nbsp;&nbsp;2015-02-11&nbsp;&nbsp;&nbsp;support ingest from RTSP  with opencv.<br/>
[0.9.9]&nbsp;&nbsp;2015-02-11&nbsp;&nbsp;&nbsp;support ingest from RTP  with opencv.<br/>
[0.9.9]&nbsp;&nbsp;2015-02-11&nbsp;&nbsp;&nbsp;support ingest from RTMP  with opencv.<br/>
[0.9.9]&nbsp;&nbsp;2015-02-11&nbsp;&nbsp;&nbsp;support ingest from HLS  with opencv.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support ingest from RTSP  with libvlc.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support ingest from RTP  with libvlc.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support ingest from RTMP  with libvlc.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support ingest from HLS  with libvlc.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support ingest from GIF movie.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;support combine rgba to rgb24.<br/>
[0.1.0]&nbsp;&nbsp;2015-03-28&nbsp;&nbsp;&nbsp;add player with libvlc.<br/>