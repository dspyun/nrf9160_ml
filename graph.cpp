
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define MAX_LEN 200
static char cordi_upper[10][200];
static char cordi_lower[10][200];
static int weight;
static int length;

void init_cordi(int param,int len) {
    weight = param;
    length = len;
    // 공백으로 채운다
    for(int i =0;i<10;i++) {
        memset(&cordi_upper[i][0],0x20,MAX_LEN);
        memset(&cordi_lower[i][0],0x20,MAX_LEN);
    }
}

void fill_x_cordi(float *data) {

    int i, j;
    int result;
    char ch='x';

    for(i =0;i<length;i++) {
        result = (int)*(data+i)/weight;
        //printk("%d ", result);
        if(result >= 0) {
            if(result >= 9) result = 9;
            switch(result) {
                case 0: cordi_upper[0][i]=ch; break;
                case 1: cordi_upper[1][i]=ch; break;
                case 2: cordi_upper[2][i]=ch; break;
                case 3: cordi_upper[3][i]=ch; break;
                case 4: cordi_upper[4][i]=ch; break;
                case 5: cordi_upper[5][i]=ch; break;
                case 6: cordi_upper[6][i]=ch; break;
                case 7: cordi_upper[7][i]=ch; break;
                case 8: cordi_upper[8][i]=ch; break;
                case 9: cordi_upper[9][i]=ch; break;
            }
        } else {
            if(result <= -8) result = -8;
            switch(result) {
                case -1: cordi_lower[0][i]=ch; break;
                case -2: cordi_lower[1][i]=ch; break;
                case -3: cordi_lower[2][i]=ch; break;
                case -4: cordi_lower[3][i]=ch; break;
                case -5: cordi_lower[4][i]=ch; break;
                case -6: cordi_lower[5][i]=ch; break;
                case -7: cordi_lower[6][i]=ch; break;
                case -8: cordi_lower[7][i]=ch; break;
            }
        }
    }
}

void fill_o_cordi(float *data) {

    int i, j;
    int result;
    for(i =0;i<length;i++) {
        result = (int)*(data+i)/weight;
        //printk("%d ", result);
        if(result >= 9) result = 9;
        if(result <= 0) result = 0;
        switch(result) {
            case 0: cordi_upper[0][i]='o'; break;
            case 1: cordi_upper[1][i]='o'; break;
            case 2: cordi_upper[2][i]='o'; break;
            case 3: cordi_upper[3][i]='o'; break;
            case 4: cordi_upper[4][i]='o'; break;
            case 5: cordi_upper[5][i]='o'; break;
            case 6: cordi_upper[6][i]='o'; break;
            case 7: cordi_upper[7][i]='o'; break;
            case 8: cordi_upper[8][i]='o'; break;
            case 9: cordi_upper[9][i]='o'; break;
        }
    }
}

void print_cordi(void) {

    int i, j;
    for( i=9;i>=0;i--) {
        for(j=0;j<length;j++) {
            printk("%c",cordi_upper[i][j]);
        }
        printk("\n");
    }
    for( i=0;i<=9;i++) {
        for(j=0;j<length;j++) {
            printk("%c",cordi_lower[i][j]);
        }
        printk("\n");
    }
    printk("\n");
}
