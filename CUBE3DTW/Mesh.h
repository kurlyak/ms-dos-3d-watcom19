//======================================================================================
//      Ed Kurlyak 2023 Perspective Correct Texturemapping
//      WATCOM 1.9 C++ Source Code
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_


#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>

#pragma pack(1)

typedef struct tagBITMAPFILEHEADER {
  unsigned short int   bfType;
  unsigned long    bfSize;
  unsigned short int   bfReserved1;
  unsigned short int   bfReserved2;
  unsigned long    bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
  unsigned long  biSize;
  unsigned long  biWidth;
  unsigned long  biHeight;
  unsigned short int   biPlanes;
  unsigned short int   biBitCount;
  unsigned long  biCompression;
  unsigned long  biSizeImage;
  unsigned long  biXPelsPerMeter;
  unsigned long  biYPelsPerMeter;
  unsigned long  biClrUsed;
  unsigned long  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD{
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char Reserverd;
}RGBQUAD;

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

struct matrix4x4
{
    union
    {
        float  m1[4][4];

        struct {
            float r1c1; float r1c2; float r1c3; float r1c4;
            float r2c1; float r2c2; float r2c3; float r2c4;
            float r3c1; float r3c2; float r3c3; float r3c4;
            float r4c1; float r4c2; float r4c3; float r4c4;
            };
    };

    matrix4x4 (){};
    ~matrix4x4 (){};
        
    matrix4x4 (float in_r1c1, float in_r1c2, float in_r1c3, float in_r1c4,
               float in_r2c1, float in_r2c2, float in_r2c3, float in_r2c4,
               float in_r3c1, float in_r3c2, float in_r3c3, float in_r3c4,
               float in_r4c1, float in_r4c2, float in_r4c3, float in_r4c4)
    {
        r1c1 = in_r1c1; r1c2 = in_r1c2; r1c3 = in_r1c3; r1c4 = in_r1c4;
        r2c1 = in_r2c1; r2c2 = in_r2c2; r2c3 = in_r2c3; r2c4 = in_r2c4;
        r3c1 = in_r3c1; r3c2 = in_r3c2; r3c3 = in_r3c3; r3c4 = in_r3c4;
        r4c1 = in_r4c1; r4c2 = in_r4c2; r4c3 = in_r4c3; r4c4 = in_r4c4;
    };

    matrix4x4 operator * (matrix4x4 mat2)
    {
        matrix4x4 mat;

        for (int row=0; row<4; row++)
        {
            for (int col=0; col<4; col++)
            {
                float sum = 0.0;

                for (int index=0; index<4; index++)
                {
                    sum+=(m1[row][index]*mat2.m1[index][col]);
                }
        
                mat.m1[row][col] = sum;
            }
        }

        return mat;
    }
};

struct tex_coord2
{
        int tu, tv;
};

struct vector3
{
    vector3() {};
    vector3(float xi, float yi, float zi) : x(xi), y(yi), z(zi) {};

    union
    {
        float m[3];
        struct {
            float x,y,z;
            };
    };

    vector3 operator - (vector3 v_in)
    {
        vector3 v_out;
                
        v_out.x = x - v_in.x;
        v_out.y = y - v_in.y;
        v_out.z = z - v_in.z;
                
        return v_out;
    }
        
    vector3 operator = (vector3 v_in)
    {
        x = v_in.x;
        y = v_in.y;
        z = v_in.z;

        return *this;
    }
};

struct plane
{
    float a,b,c,d;
};


class CMeshManager
{
public:
    CMeshManager();
    ~CMeshManager();

    void InitCube(unsigned char *back_buffer, int back_lpitch);
    void CalculateCube();
    void DrawCube ();

private:

    unsigned char *m_back_buffer;
    int m_back_lpitch;

    void Draw_Textured_Triangle(vector3 v1, tex_coord2 t1,
                                vector3 v2, tex_coord2 t2,
                                vector3 v3, tex_coord2 t3);
    void Draw_Textured_Poly(int y1, int y2);

    float dxdyl, dudyl, dvdyl, dzdyl, dxdyr, dudyr, dvdyr, dzdyr;
    float ul, ur, vl, vr, zl, zr, xl, xr;

    void Load_BMP();

    vector3 Mat4x4_Vect3_Mul(matrix4x4 mat1, vector3 vec1);
    float Vec3Dot(vector3 v1, vector3 v2);
    vector3 Vec3Cross(vector3 v1, vector3 v2);
    vector3 Vec3Normalize(vector3 v);
    vector3 Vec3Add(vector3 v1, vector3 v2);
    vector3 Vec3Scale(vector3 v, float val);

    unsigned char *pRes;

    /*
    int nTextureWidth;
    int nTextureHeight;
    */

    unsigned char nVertCount;
    unsigned char nTriangleCount;
    
    unsigned int nViewWidth;
    unsigned int nViewHeight;

    vector3 *vert_buff_transformed;
    vector3 *vert_buff;
    tex_coord2 *tex_coord;
    unsigned int *index_buff;
};

#endif



