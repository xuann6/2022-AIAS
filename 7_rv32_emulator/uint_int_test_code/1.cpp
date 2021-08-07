#include <stdlib.h>
#include <stdio.h>

int main(){

    int16_t   fVar =  0xffff;
    int32_t     iVar;

    iVar = *(int32_t*)&fVar;    // iVar 值為整數 1

    //printf("iVar = %d.\n",*&iVar);
    printf("fVar = %d\n",fVar);
    printf("fVar address = %p\n",&fVar);
    printf("iVar = %d\n",iVar);
    printf("iVar address = %p\n",&iVar);
    return 0;
}
//case 1
//fVar = 0x2121     = binary 0000000000000000,0010000100100001
//iVar = 0x7ffc2121 = binary 0111111111111100,0010000100100001

//case 2
//fVar = 0x00fc     = binary 0000000000000000,0000000011111100
//iVar = 0x7fff00fc = binary 0111111111111111,0000000011111100

//case 3
//fVar = 0xffff     = binary 0000000000000000,1111111111111111
//iVar = 0x7fff00fc = binary 0111111111111111,0000000011111100
