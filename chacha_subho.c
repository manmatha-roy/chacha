#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define ul unsigned int
#define rotateleft(x, n) (((x) << (n)) ^ ((x) >> (32-n)))
#define rotateright(x, n) (((x) >> (n)) ^ ((x) << (32-n)))
#define myrand32 ((ul) (4294967296.0*drand48()))

#define LOOP (1024*1024*1024ULL)

void initializeR(ul *x){
        int i;
        for (i = 0; i < 16; i++)
                x[i] = myrand32;

        x[0] = 0x61707865;
        x[1] = 0x3320646e;
        x[2] = 0x79622d32;
        x[3] = 0x6b206574;
}

void copystate(ul *x1, ul *x){
        int i;
        for (i = 0; i < 16; i++)
                x1[i] = x[i];
}

void print(ul *x){
        int i;
        for (i = 0; i < 16; i++){
                printf("%8x ", x[i]);
                if (i > 0 && i%4 == 3)
                        printf("\n");
        }
        printf("\n");
}

void qr(ul *x0, ul *x1, ul *x2, ul *x3){
        ul p,q,r,s;

        p = *x0;
        q = *x1;
        r = *x2;
        s = *x3;

        p += q; s ^= p; s = rotateleft(s,16);
        r += s; q ^= r; q = rotateleft(q,12);
        p += q; s ^= p; s = rotateleft(s,8);
        r += s; q ^= r; q = rotateleft(q,7);

        *x0 = p;
        *x1 = q;
        *x2 = r;
        *x3 = s;
}

void rounds(ul *x){

        qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
        qr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
        qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
        qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
        qr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
        qr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
        qr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
        qr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}

void roundo(ul *x){
        qr(&(x[0]), &(x[4]), &(x[8]), &(x[12]));
        qr(&(x[1]), &(x[5]), &(x[9]), &(x[13]));
        qr(&(x[2]), &(x[6]), &(x[10]), &(x[14]));
        qr(&(x[3]), &(x[7]), &(x[11]), &(x[15]));
}
void rounde(ul *x){
        qr(&(x[0]), &(x[5]), &(x[10]), &(x[15]));
        qr(&(x[1]), &(x[6]), &(x[11]), &(x[12]));
        qr(&(x[2]), &(x[7]), &(x[8]), &(x[13]));
        qr(&(x[3]), &(x[4]), &(x[9]), &(x[14]));
}

int main(int argc, char **argv){
	ul x[16], x1[16], pattern, patx, outpt, outpt1;
	int i, j, k, word1, bit1, word2, bit2, i1; 
	unsigned long long loop = 0;
	double cnt[16][32], val3 = 0.0, tval = 0.0, max, min, val4 = 0.0, val5 = 0.0, val34 = 0.0, val35 = 0.0;
	FILE *fp;

	int ww[] = {1, 11, 12, 12};
	int bb[] = {0, 0, 8, 0};

	int ww1[] = {12,0,0,8,4,15,11,15,3,1,5,5,9,13,12,0,0,8,4,8,4};
	int bb1[] = {24,16,0,0,7,0,0,8,0,0,7,19,12,0,0,24,8,8,15,7,14};

	srand48(time(NULL));

	word1 = 13; bit1 = 13;
	word2 = 11; bit2 = 0;

	while (loop < LOOP){
		tval += 1.0; loop++;
	
		initializeR(x);	copystate(x1, x);
		patx = 0x1; patx = patx << bit1;
		x1[word1] = x[word1] ^ patx;

		for (i = 0; i < 1; i++){ rounds(x); rounds(x1); }
		roundo(x); roundo(x1); // after 3 rounds

		outpt = 0x0;	
		patx = 0x1; patx = patx << bit2; outpt = outpt ^ (((x[word2] ^ x1[word2]) & patx) >> bit2);

		if (outpt == 0x0) val3 += 1.0; //round 3 bias
		outpt1 = outpt;

		rounde(x); rounde(x1); //4 rounds

		outpt = 0x0;	
		for (i1 = 0; i1 < 4; i1++){
			patx = 0x1; patx = patx << bb[i1]; outpt = outpt ^ (((x[ww[i1]] ^ x1[ww[i1]]) & patx) >> bb[i1]);
		}

		if (outpt == 0x0) val4 += 1.0; //4 round bias
		if (outpt == outpt1) val34 += 1.0;

		roundo(x); roundo(x1); // after 5 rounds

		outpt = 0x0;	
		for (i1 = 0; i1 < 21; i1++){
			patx = 0x1; patx = patx << bb1[i1]; outpt = outpt ^ (((x[ww1[i1]] ^ x1[ww1[i1]]) & patx) >> bb1[i1]);
		}

		if (outpt == 0x0) val5 += 1.0; //5 round bias

		if (outpt == outpt1) val35 += 1.0;

		if (loop > 0 && loop%(1024*1024*16) == 0)
			printf("%llu x 2^24, Prob: 3 round %lf, 4 round: %lf, round 3 = round 4: %lf, 5 round: %lf, round 3 = round 5: %lf\n", loop/(1024*1024*16), val3/tval, val4/tval, val34/tval, val5/tval, val35/tval);
	}
}
