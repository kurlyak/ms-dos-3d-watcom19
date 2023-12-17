//======================================================================================
//      Ed Kurlyak 2020 Perspective Texturemapping
//======================================================================================

#include "Mesh.h"

void CMeshManager::Load_BMP()
{
    FILE *fp;

    //texture size 256x256 pixels, 8 bit color depth
    fp = fopen("texture.bmp","rb");
    if(fp==NULL) printf("Error Open File");

    BITMAPFILEHEADER bfh;

    fread(&bfh, sizeof(bfh), 1, fp);

    BITMAPINFOHEADER bih;

    fread(&bih, sizeof(bih), 1, fp);

    RGBQUAD rgbPal[256];
    fread(&rgbPal, 256 * 4,1,fp);

    unsigned char *nStartData;
    nStartData = new unsigned char [bih.biWidth*bih.biHeight];

    fread(nStartData,bih.biWidth*bih.biHeight,1,fp);

    fclose(fp);

    pRes = nStartData;

    //register color address VGA
    #define PALETTE_MASK            0x3c6
    #define PALETTE_REGISTER_RD     0x3c7    //register VGA for reading
    #define PALETTE_REGISTER_WR     0x3c8    //register VGA for writing
    #define PALETTE_DATA            0x3c9    //color data 3 byte following

    //one after another r,g,b
    for(int i = 0; i<256; i++)
    {
        //register color has 6 bit - 64
        //picture has color 8 bit - 256
        //convert i.e. 256/64 = 4

        unsigned char r = rgbPal[i].rgbRed/4;
        unsigned char g = rgbPal[i].rgbGreen/4;
        unsigned char b = rgbPal[i].rgbBlue/4;

        outp(PALETTE_MASK, 0xff);
        outp(PALETTE_REGISTER_WR, i);
        outp(PALETTE_DATA, r);
        outp(PALETTE_DATA, g);
        outp(PALETTE_DATA, b);
    }   
}

CMeshManager::CMeshManager()
{

}

CMeshManager::~CMeshManager()
{ 
        delete [] vert_buff;
        delete [] vert_buff_transformed;
        delete [] tex_coord;
        delete [] index_buff;
};

vector3 CMeshManager::Mat4x4_Vect3_Mul(matrix4x4 mat1,vector3 vec1)
{
        vector3 temp;

        for ( int j = 0; j < 3; j++)
        {
                float sum = 0.0f;
                int i;
                for ( i = 0; i < 3; i++)
                {
                        sum += vec1.m[i] * mat1.m1[i][j];
                }
                
                sum += mat1.m1[i][j];
                temp.m[j] = sum;
        }

        return temp;
}

void CMeshManager::InitCube(unsigned char *back_buffer, int back_lpitch)
{

    m_back_buffer = back_buffer;
    m_back_lpitch = back_lpitch;

    pRes = NULL;

    Load_BMP();

    nViewWidth = 640;
    nViewHeight = 480;

    nVertCount = 24;
    nTriangleCount = 12;

    vector3 vert_buff_temp[24] = {vector3(-5.000000,-5.000000,-5.000000),
        vector3(-5.000000,-5.000000,5.000000),
        vector3(5.000000,-5.000000,5.000000),
        vector3(5.000000,-5.000000,-5.000000),
        vector3(-5.000000,5.000000,-5.000000),
        vector3(5.000000,5.000000,-5.000000),
        vector3(5.000000,5.000000,5.000000),
        vector3(-5.000000,5.000000,5.000000),
        vector3(-5.000000,-5.000000,-5.000000),
        vector3(5.000000,-5.000000,-5.000000),
        vector3(5.000000,5.000000,-5.000000),
        vector3(-5.000000,5.000000,-5.000000),
        vector3(5.000000,-5.000000,-5.000000),
        vector3(5.000000,-5.000000,5.000000),
        vector3(5.000000,5.000000,5.000000),
        vector3(5.000000,5.000000,-5.000000),
        vector3(5.000000,-5.000000,5.000000),
        vector3(-5.000000,-5.000000,5.000000),
        vector3(-5.000000,5.000000,5.000000),
        vector3(5.000000,5.000000,5.000000),
        vector3(-5.000000,-5.000000,5.000000),
        vector3(-5.000000,-5.000000,-5.000000),
        vector3(-5.000000,5.000000,-5.000000),
        vector3(-5.000000,5.000000,5.000000) };

    //texture has size 256x256 pixels
    tex_coord2 tex_coord_temp[24] = {255,255,
        255,0,
        0,0,
        0,255,
        0,255,
        255,255,
        255,0,
        0,0,
        0,255,
        255,255,
        255,0,
        0,0,
        0,255,
        255,255,
        255,0,
        0,0,
        0,255,
        255,255,
        255,0,
        0,0,
        0,255,
        255,255,
        255,0,
        0,0};

    unsigned int index_buff_temp[36] = {0,2,1,
        2,0,3,
        4,6,5,
        6,4,7,
        8,10,9,
        10,8,11,
        12,14,13,
        14,12,15,
        16,18,17,
        18,16,19,
        20,22,21,
        22,20,23};

    vert_buff = NULL;
    vert_buff = new vector3[24];

    vert_buff_transformed = NULL;
    vert_buff_transformed = new vector3[24];

    tex_coord = NULL;
    tex_coord = new tex_coord2[24];

    index_buff = NULL;
    index_buff = new unsigned int [36];

    memcpy(vert_buff, vert_buff_temp, 24 * sizeof(vector3));
    memcpy(tex_coord, tex_coord_temp, 24 * sizeof(tex_coord2));
    memcpy(index_buff, index_buff_temp, 36 * sizeof(unsigned int));
}

void CMeshManager::CalculateCube()
{
    static float angle = 0.0f;
    
    matrix4x4 mWorldRotX(
        1, 0, 0, 0,
        0, cos(angle), sin(angle), 0,
        0,-sin(angle),  cos(angle), 0,
        0, 0, 0, 1);

    matrix4x4 mWorldRotY(
        cos(angle), 0, -sin(angle), 0,
        0, 1, 0, 0,
        sin(angle), 0, cos(angle), 0,
        0, 0, 0, 1);

    matrix4x4 mWorldRotZ(
        cos(angle), sin(angle), 0, 0,
        -sin(angle), cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    angle+=PI/45.0f;
    if(angle>PI2*2.0f)
        angle = 0.0f;

    matrix4x4 mWorld(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 30, 1);

    matrix4x4 mView(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    float fFov =(float) 3.14/3; // FOV 90 degree
    float fAspect = (float) nViewWidth/nViewHeight;
    float fZFar = 100.0f;
    float fZNear = (float)0.1f;

    float    h, w, Q;

    w = (float)(1/tan(fFov*0.5f))/fAspect;  
    h = (float)1/tan(fFov*0.5f);   
    Q = fZFar/(fZFar - fZNear);

    /*
    matrix4x4 mProj(
        w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*fZNear, 0);
    */

    matrix4x4 mProj(
        w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

     
    float alpha = 0.5f * (float)nViewWidth;
    float beta  = 0.5f * (float)nViewHeight;
        
    matrix4x4 mScreen(
        alpha, 0, 0, 0, 
        0, -beta, 0, 0, 
        0, 0, 1, 0,
        alpha, beta, 0, 1);

    for (unsigned int i = 0; i < nVertCount; i++)
    {
        vector3 v;
        v = Mat4x4_Vect3_Mul(mWorldRotX, vert_buff[i]);
        v = Mat4x4_Vect3_Mul(mWorldRotY, v);
        v = Mat4x4_Vect3_Mul(mWorldRotZ, v);
        v = Mat4x4_Vect3_Mul(mWorld, v);
        v = Mat4x4_Vect3_Mul(mView, v);
        v = Mat4x4_Vect3_Mul(mProj, v);

        v.x = v.x / v.z;
        v.y = v.y / v.z;

        v = Mat4x4_Vect3_Mul(mScreen, v);

        vert_buff_transformed[i] = v;
    }
}

void CMeshManager::DrawCube ()
{
    for (unsigned int i = 0; i < nTriangleCount; i++)
    {
        vector3 v1 = vert_buff_transformed[index_buff[i * 3 + 0]];
        vector3 v2 = vert_buff_transformed[index_buff[i * 3 + 1]];
        vector3 v3 = vert_buff_transformed[index_buff[i * 3 + 2]];

        tex_coord2 t1 = tex_coord[index_buff[i * 3 + 0]];
        tex_coord2 t2 = tex_coord[index_buff[i * 3 + 1]];
        tex_coord2 t3 = tex_coord[index_buff[i * 3 + 2]];
                
        float s = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);

        if(s <= 0 )
            continue;

        Draw_Textured_Triangle(v1, t1, v2, t2, v3, t3);
    }  
}

void CMeshManager::Draw_Textured_Triangle(vector3 v1, tex_coord2 t1,
                                    vector3 v2, tex_coord2 t2,
                                    vector3 v3, tex_coord2 t3)
{
    int side;
    float x1, x2, x3;
    float y1, y2, y3;
    float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
    float tempf;
    int y1i, y2i, y3i;
    float dy;
    float dyl, dyr;

    x1 = v1.x + 0.5f;
    y1 = v1.y + 0.5f;
    x2 = v2.x + 0.5f;
    y2 = v2.y + 0.5f;
    x3 = v3.x + 0.5f;
    y3 = v3.y + 0.5f;

    iz1 = 1.0f / v1.z;
    iz2 = 1.0f / v2.z;
    iz3 = 1.0f / v3.z;
        
    uiz1 = t1.tu * iz1;
    viz1 = t1.tv * iz1;
    uiz2 = t2.tu * iz2;
    viz2 = t2.tv * iz2;
    uiz3 = t3.tu * iz3;
    viz3 = t3.tv * iz3;

    #define swapfloat(x, y) tempf = x; x = y; y = tempf;

    if (y1 > y2)
    {
        swapfloat(x1, x2);
        swapfloat(y1, y2);
        swapfloat(iz1, iz2);
        swapfloat(uiz1, uiz2);
        swapfloat(viz1, viz2);
    }
    if (y1 > y3)
    {
        swapfloat(x1, x3);
        swapfloat(y1, y3);
        swapfloat(iz1, iz3);
        swapfloat(uiz1, uiz3);
        swapfloat(viz1, viz3);
    }
    if (y2 > y3)
    {
        swapfloat(x2, x3);
        swapfloat(y2, y3);
        swapfloat(iz2, iz3);
        swapfloat(uiz2, uiz3);
        swapfloat(viz2, viz3);
    }
        
    #undef swapfloat

    y1i = (int) floor (y1);
    y2i = (int) floor (y2);
    y3i = (int) floor (y3);

    if ((y1i == y2i && y1i == y3i)
        || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
            return;

    // Determine which side of the poly the longer edge is on

    if(y2 > y1 && y3 > y2)
    {
        float dxdy1, dxdy2;
        dxdy1 = (x2 - x1) / (y2 - y1);
        dxdy2 = (x3 - x1) / (y3 - y1);
        side = dxdy2 > dxdy1;
    }

    if (y1 == y2)
        side = x1 > x2;
    if (y2 == y3)
        side = x3 > x2;


    if (!side)      // Longer edge is on the left side
    {
        dyl = y3 - y1;
        dxdyl = (x3 - x1) / dyl;
        dudyl = (uiz3 - uiz1) / dyl;
        dvdyl = (viz3 - viz1) / dyl;
        dzdyl = (iz3 - iz1) / dyl;
        
        dy = 1 - (y1 - y1i);
        xl = x1 + dy * dxdyl;
        ul = uiz1 + dy * dudyl;
        vl = viz1 + dy * dvdyl;
        zl = iz1 + dy * dzdyl;

        if (y1 < y2)    // Draw upper segment if possibly visible
        {
            dyr = y2 - y1;
            dxdyr = (x2 - x1) / dyr;
            dudyr = (uiz2 - uiz1) / dyr;
            dvdyr = (viz2 - viz1) / dyr;
            dzdyr = (iz2 - iz1) / dyr;

            xr = x1 + dy * dxdyr;
            ur = uiz1 + dy * dudyr;
            vr = viz1 + dy * dvdyr;
            zr = iz1 + dy * dzdyr;

            Draw_Textured_Poly(y1i, y2i);

        }

        if (y2 < y3)    // Draw lower segment if possibly visible
        {
            dyr = y3 - y2;
            dxdyr = (x3 - x2) / dyr;
            dudyr = (uiz3 - uiz2) / dyr;
            dvdyr = (viz3 - viz2) / dyr;
            dzdyr = (iz3 - iz2) / dyr;

            xr = x2 + (1 - (y2 - y2i)) * dxdyr;
            ur = uiz2 + (1 - (y2 - y2i)) * dudyr;
            vr = viz2 + (1 - (y2 - y2i)) * dvdyr;
            zr = iz2 + (1 - (y2 - y2i)) * dzdyr;

            Draw_Textured_Poly(y2i, y3i);
        }
                
    }
    else // Longer edge is on the right side
    {
        dyr = y3 - y1;
        dxdyr = (x3 - x1) / dyr;
        dudyr = (uiz3 - uiz1) / dyr;
        dvdyr = (viz3 - viz1) / dyr;
        dzdyr = (iz3 - iz1) / dyr;

        dy = 1 - (y1 - y1i);
        xr = x1 + dy * dxdyr;
        ur = uiz1 + dy * dudyr;
        vr = viz1 + dy * dvdyr;
        zr = iz1 + dy * dzdyr;

        if (y1 < y2)    // Draw upper segment if possibly visible
        {
            dyl = y2 - y1;
            dxdyl = (x2 - x1) / dyl;
            dudyl = (uiz2 - uiz1) / dyl;
            dvdyl = (viz2 - viz1) / dyl;
            dzdyl = (iz2 - iz1) / dyl;

            xl = x1 + dy * dxdyl;
            ul = uiz1 + dy * dudyl;
            vl = viz1 + dy * dvdyl;
            zl = iz1 + dy * dzdyl;

            Draw_Textured_Poly(y1i, y2i);
        
        }
        if (y2 < y3)    // Draw lower segment if possibly visible
        {
            dyl = y3 - y2;
            dxdyl = (x3 - x2) / dyl;
            dudyl = (uiz3 - uiz2) / dyl;
            dvdyl = (viz3 - viz2) / dyl;
            dzdyl = (iz3 - iz2) / dyl;
            
            dy = 1 - (y2 - y2i);
            xl = x2 + dy * dxdyl;
            ul = uiz2 + dy * dudyl;
            vl = viz2 + dy * dvdyl;
            zl = iz2 + dy * dzdyl;

            Draw_Textured_Poly(y2i, y3i);
        
        }
    }
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
    int x1, x2;
    int dx;
    float ui, vi, zi;
    float du, dv, dz;

    unsigned char *screen_ptr  = NULL;
    screen_ptr = m_back_buffer + (y1 * m_back_lpitch);
                        
    for (int yi = y1; yi<y2; yi++)
    {
        x1 = (int) xl;
        x2   = (int) xr;

        ui = ul;
        vi = vl;
        zi = zl;

        if ((dx = (x2 - x1))>0)
        {
            du = (ur - ul)/dx;
            dv = (vr - vl)/dx;
            dz = (zr - zl)/dx;
        }
        else
        {
            du = (ur - ul);
            dv = (vr - vl);
            dz = (zr - zl);
        }

        for (int xi=x1; xi<x2; xi++)
        {
            float z = 1.0f/zi;
            float u = ui * z;
            float v = vi * z;

            int t = (int)u  + (((int)v) << 8);

            screen_ptr[xi] = pRes[t];

            ui+=du;
            vi+=dv;
            zi+=dz;
        }

        xl+=dxdyl;
        ul+=dudyl;
        vl+=dvdyl;
        zl+=dzdyl;

        xr+=dxdyr;
        ur+=dudyr;
        vr+=dvdyr;
        zr+=dzdyr;

        screen_ptr+= m_back_lpitch;
    }
}


