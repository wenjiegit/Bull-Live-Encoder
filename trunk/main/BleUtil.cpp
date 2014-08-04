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

#include "BleUtil.hpp"

#include <QObject>

#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))

#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))

//void ConvertRGB2YUV(unsigned char *RGB, unsigned char *YUV, int WIDTH , int HEIGHT)
//{
//    unsigned int i,x,y,j;
//    unsigned char *Y = NULL;
//    unsigned char *U = NULL;
//    unsigned char *V = NULL;

//    Y = YUV;
//    U = YUV + WIDTH*HEIGHT;
//    V = U + ((WIDTH*HEIGHT)>>2);

//    for(y=0; y < HEIGHT; y++)
//        for(x=0; x < WIDTH; x++)
//        {
//            j = y*WIDTH + x;
//            i = j*3;
//            Y[j] = (unsigned char)(DY(RGB[i], RGB[i+1], RGB[i+2]));

//            if(x%2 == 1 && y%2 == 1)
//            {
//                j = (WIDTH>>1) * (y>>1) + (x>>1);
//                //上面i仍有效
//                U[j] = (unsigned char)
//                        ((DU(RGB[i  ], RGB[i+1], RGB[i+2]) +
//                        DU(RGB[i-3], RGB[i-2], RGB[i-1]) +
//                        DU(RGB[i  -WIDTH*3], RGB[i+1-WIDTH*3], RGB[i+2-WIDTH*3]) +
//                        DU(RGB[i-3-WIDTH*3], RGB[i-2-WIDTH*3], RGB[i-1-WIDTH*3]))/4);

//                V[j] = (unsigned char)
//                        ((DV(RGB[i  ], RGB[i+1], RGB[i+2]) +
//                        DV(RGB[i-3], RGB[i-2], RGB[i-1]) +
//                        DV(RGB[i  -WIDTH*3], RGB[i+1-WIDTH*3], RGB[i+2-WIDTH*3]) +
//                        DV(RGB[i-3-WIDTH*3], RGB[i-2-WIDTH*3], RGB[i-1-WIDTH*3]))/4);
//            }
//        }
//}

// Conversion from RGB to YUV420

int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];

int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];

int RGB2YUV_VG[256], RGB2YUV_VB[256];

// Conversion from YUV420 to RGB24

static long int crv_tab[256];

static long int cbu_tab[256];

static long int cgu_tab[256];

static long int cgv_tab[256];

static long int tab_76309[256];

static unsigned char clp[1024]; //for clip in CCIR601

//

// Table used for RGB to YUV420 conversion

//

void InitLookupTable()
{

    int i;

    for (i = 0; i < 256; i++) RGB2YUV_YR[i] = (float)65.481 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_YG[i] = (float)128.553 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_YB[i] = (float)24.966 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_UR[i] = (float)37.797 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_UG[i] = (float)74.203 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_VG[i] = (float)93.786 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_VB[i] = (float)18.214 * (i<<8);

    for (i = 0; i < 256; i++) RGB2YUV_UBVR[i] = (float)112 * (i<<8);

}

//

// Convert from RGB24 to YUV420

//

int ConvertRGB2YUV(unsigned char *bmp,unsigned char *yuv, int w,int h)

{

    static bool init = false;
    if (!init) {
        InitLookupTable();
        init = true;
    }
    unsigned char *u,*v,*y,*uu,*vv;

    unsigned char *pu1,*pu2,*pu3,*pu4;

    unsigned char *pv1,*pv2,*pv3,*pv4;

    unsigned char *r,*g,*b;

    int i,j;

    uu=new unsigned char[w*h];

    vv=new unsigned char[w*h];

    if(uu==NULL || vv==NULL)

        return 0;

    y=yuv;

    u=uu;

    v=vv;

    // Get r,g,b pointers from bmp image data....

    r=bmp;

    g=bmp+1;

    b=bmp+2;

    //Get YUV values for rgb values...

    for(i=0;i<h;i++)

    {

        for(j=0;j<w;j++)

        {

            *y++=( RGB2YUV_YR[*r] +RGB2YUV_YG[*g]+RGB2YUV_YB[*b]+1048576)>>16;

            *u++=(-RGB2YUV_UR[*r] -RGB2YUV_UG[*g]+RGB2YUV_UBVR[*b]+8388608)>>16;

            *v++=( RGB2YUV_UBVR[*r]-RGB2YUV_VG[*g]-RGB2YUV_VB[*b]+8388608)>>16;

            r+=3;

            g+=3;

            b+=3;

        }

    }

    // Now sample the U & V to obtain YUV 4:2:0 format

    // Sampling mechanism...

    /* @ -> Y

# -> U or V

@ @ @ @

# #

@ @ @ @

@ @ @ @

# #

@ @ @ @

*/

    // Get the right pointers...

    u=yuv+w*h;

    v=u+(w*h)/4;

    // For U

    pu1=uu;

    pu2=pu1+1;

    pu3=pu1+w;

    pu4=pu3+1;

    // For V

    pv1=vv;

    pv2=pv1+1;

    pv3=pv1+w;

    pv4=pv3+1;

    // Do sampling....

    for(i=0;i<h;i+=2)

    {

        for(j=0;j<w;j+=2)

        {

            *u++=(*pu1+*pu2+*pu3+*pu4)>>2;

            *v++=(*pv1+*pv2+*pv3+*pv4)>>2;

            pu1+=2;

            pu2+=2;

            pu3+=2;

            pu4+=2;

            pv1+=2;

            pv2+=2;

            pv3+=2;

            pv4+=2;

        }

        pu1+=w;

        pu2+=w;

        pu3+=w;

        pu4+=w;

        pv1+=w;

        pv2+=w;

        pv3+=w;

        pv4+=w;

    }

    delete uu;

    delete vv;

    return 1;

}
