// This file is a "Hello, world!" in C++ language by GCC for wandbox.
#include <iostream>
#include <cstdlib>

int main()
{
    float f = 0;
    uint8_t *p= (uint8_t *)&f;
    
    *(p+3) = 2;
    float mi=0, ma=0;
    for(int k = 0; k <= 255; k++)
    {
        for(int j = 0; j <= 255; j++)
        {
            for(int l = 0; l <= 255; l++)
            {                
                *(p+1) = l;
                if(f<mi)
                    mi=f;
                if(f>ma)
                    ma=f;
            }
            *(p+2) = j;
        }
        *(p+0) = k;
    }
          
    std::cout << "Minimo " << mi << "  Massimo " << ma;
}
