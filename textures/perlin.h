#ifndef PERLIN_H
#define PERLIN_H

#include"rtweekend.h"

class perlin
{
public:
    perlin( ){
        ranvec = new Vec3f[point_count];
        for(int i=0;i<point_count;++i){
            ranvec[i] = random_v3f(-1,1).normalize();
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }
    ~perlin(){
        delete[] ranvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    //keypoint
    float noise(const point3& p) const{
        //[u,v,w] 三维晶格内的偏移向量
        auto u = p.x - floor(p.x);
        auto v = p.y - floor(p.y);
        auto w = p.z - floor(p.z);

        //Hermitian Smoothing
        // u = u*u*(3-2*u);
        // v = v*v*(3-2*v);
        // w = w*w*(3-2*w);

        //[i,j,k] 晶格的位置坐标，有一个对应该点的梯度向量（随机的
        auto i = static_cast<int>(floor(p.x)) ;
        auto j = static_cast<int>(floor(p.y)) ;
        auto k = static_cast<int>(floor(p.z)) ;
        Vec3f c[2][2][2]; //c[][][]为晶格各个定点上的位置坐标的梯度向量（待定

        for(int di=0;di<2;di++){
            for(int dj=0;dj<2;dj++){
                for (int dk = 0; dk < 2; dk++)
                {
                    //hash获取梯度向量
                    c[di][dj][dk]= ranvec[
                        perm_x[(i+di) & 255]^ //(i+di) & 255,其实是对256求余数的操作
                        perm_y[(j+dj) & 255]^
                        perm_z[(k+dk) & 255]
                    ];
                }                
            }
        }

        // return trilinear_interp(c, u, v, w);
        return perlin_interp(c,u,v,w);
    }

    //Turbulence湍流：is a sum of repeated calls to noise
    float turb(const point3& p, int depth=7) const{
        float accum = 0.f;
        auto temp_p = p; //频率
        auto weight = 1.f;//振幅

        for(int i=0;i<depth;i++){
            accum += weight*noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }


private:
    static const int point_count = 256;
    Vec3f* ranvec; // 构造函数中初始化了
    int* perm_x;    //同上
    int* perm_y;
    int* perm_z;
    
    static int* perlin_generate_perm(){
        auto p = new int[point_count];

        for(int i=0; i<perlin::point_count;i++){
            p[i] = i;
        }

        permute(p, point_count);

        return p;
    }

    static void permute(int* p, int n){
        for(int i=n-1; i>0; i--){
            //target与i进行交换
            int target = random_int(0,i);
            int tmp = p[i];
            p[i] = p[target];
            p[target]=tmp;
        }
    }

    static float trilinear_interp(float c[2][2][2], float u, float v, float w){
        float accum = 0.;
        for(int i=0;i<2;i++){
            for(int j=0;j<2;j++){
                for (int k = 0; k < 2; k++)
                {
                    accum+= (i*u + (1-i)*(1-u))*
                            (j*v + (1-j)*(1-v))*
                            (k*w + (1-k)*(1-w))*c[i][j][k];
                }                
            }
        }

        return accum;
    }

    static float perlin_interp(Vec3f c[2][2][2], float u, float v, float w){
        //Hermitian Smoothing，权重映射函数得到权重
        //uvw，是晶格内该点的偏移量
        //uu vv ww，是缓和曲线函数 映射之后的权重
        auto uu = u*u*(3-2*u);
        auto vv = v*v*(3-2*v);
        auto ww = w*w*(3-2*w);
        float accum = 0.;

        for(int i=0;i<2;i++){
            for(int j=0;j<2;j++){
                for (int k = 0; k < 2; k++)
                {
                    Vec3f weight_v(u-i,v-j,w-k);//偏移向量
                    accum+= (i*uu + (1-i)*(1-uu))*
                            (j*vv + (1-j)*(1-vv))*
                            (k*ww + (1-k)*(1-ww))*
                            (c[i][j][k] * weight_v); //偏移向量 点乘 梯度向量
                }                
            }
        }

        return accum;
    }



};

class noise_texture : public texture
{
public:
    noise_texture(){}
    noise_texture(float sc) : scale(sc) {}

    virtual color value(float u, float v, const point3& p) const override{
        //Perlin texture, shifted off integer values
        //perlin_interp函数可以返回负值。这些负值将被传递给我们的gamma函数的sqrt()函数，并被转换为nan。+1把perlin输出转换回0到1之间。
        // return color(1,1,1) * 0.5 * (1.f + noise.noise(scale * p)); //Perlin texture, shifted off integer values

        // return color(1,1,1) * noise.turb(scale * p); // turbulence

        return color(1,1,1) * 0.5 * (1.f + sin(scale*p.z + 10* noise.turb(p)));//Noise texture with marbled texture
    }
public:
    perlin noise;
    float scale; // for higher frequency
    
};


#endif