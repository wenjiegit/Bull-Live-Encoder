/*
The MIT License (MIT)

Copyright (c) wenjie.zhao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef BLEERRNO_H
#define BLEERRNO_H

#define BLE_SUCESS                              0
#define BLE_FAILED                              -1

// rtmp errors
#define BLE_RTMPSEND_ERROR                      100
#define BLE_RTMPCONNECT_ERROR                   101

// RtAudio errors
#define BLE_AUDIO_DEVICE_OPEN_ERROR             201
#define BLE_AUDIO_INIT_ERROR                    202

// BLE-VLC player error
#define BLE_VLC_MEDIA_MRL_EMPTY_ERROR           301
#define BLE_VLC_MEDIA_TYPE_ERROR                302
#define BLE_VLC_INTERNAL_ERROR                  303
#define BLE_VLC_MEDIA_OPEN_ERROR                304
#define BLE_VLC_SAMPLE_NOT_ENOUGH               305

#endif // BLEERRNO_H
