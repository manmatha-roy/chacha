/*
chacha-ref.c version 20080118
D. J. Bernstein
Public domain.
*/

#include<stdio.h>
#include<stdint.h>
#include<unistd.h>
#include<time.h>
#include<stdlib.h>


#define ROTATE(v,c) (v >> c)
#define XOR(v,w) (v^w)
#define PLUS(v,w) (v+w)
#define PLUSONE(v) (v+1)

#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

void chacha_standard_routine(uint32_t in[16], uint32_t out[16])
{
  uint32_t x[16];
  int i;

  for (i = 0;i < 16;++i) x[i] = in[i];
  for (i = 8;i > 0;i -= 2) {
    QUARTERROUND( 0, 4, 8,12)
    QUARTERROUND( 1, 5, 9,13)
    QUARTERROUND( 2, 6,10,14)
    QUARTERROUND( 3, 7,11,15)
    QUARTERROUND( 0, 5,10,15)
    QUARTERROUND( 1, 6,11,12)
    QUARTERROUND( 2, 7, 8,13)
    QUARTERROUND( 3, 4, 9,14)
  }
  for (i = 0;i < 16;++i) out[i] = PLUS(x[i],in[i]);
  
}

void print_chacha_matrix(uint32_t words[16], char name[100])
{

	uint8_t i;

	printf("\n\t\t%s\n\n", name);

	for(i=0;i<16;i++)
	{
		
		if(i%4 == 0)
			printf("\n\t");
		printf("  %u", words[i]);
		
	}

	printf("\n\n");
}

uint32_t calc_bit_diff(uint32_t a[16], uint32_t b[16], uint32_t* bitset)
{

	uint32_t count;
	uint32_t xor,const_diff_count,iv_diff_count,key_diff_count,
				word_diff_count;
	const_diff_count=iv_diff_count=key_diff_count=0;

	uint32_t bitpos=0;
	uint32_t tmp=0;

	//for(count=256;count<384;count++)
	//	printf("%u %u \n", count+1, 
	//			bitset[count]);

	for(count=0;count<4;count++)
	{
		
		word_diff_count=xor=0;
		xor=XOR(a[count], b[count]);
		while(xor)
	  	{
	    		bitset[bitpos] += (xor & 1);
			bitpos++;
			word_diff_count += xor & 1;
	    		xor >>= 1;
	  	}
		bitpos =(32*(count+1));
		const_diff_count += word_diff_count;
	}
	for(count=4;count<12;count++)
	{
		
		word_diff_count=xor=0;
		xor=XOR(a[count], b[count]);
		while(xor)
	  	{
	    		bitset[bitpos] += (xor & 1);
			bitpos++;
			word_diff_count += xor & 1;
	    		xor >>= 1;
	  	}
		bitpos =(32*(count+1));
		key_diff_count += word_diff_count;		
	}
	for(count=12;count<16;count++)
	{
		
		word_diff_count=xor=0;
		
		xor=XOR(a[count], b[count]);
		while(xor)
	  	{
	    		bitset[bitpos] += (xor & 1);
			bitpos++;
			word_diff_count += xor & 1;
	    		xor >>= 1;
	  	}
		bitpos =(32*(count+1));
		iv_diff_count += word_diff_count;		
	}


	//for(count=256;count<384;count++)
	//	printf("%u %u \n", count+1, 
	//			bitset[count]);

	//printf("\tNo Toggle Bits: Const %d Key %d IV %d Total %d \n", 
	//		const_diff_count,key_diff_count,iv_diff_count, 
	//		(const_diff_count+iv_diff_count+key_diff_count));
	
	return (const_diff_count+iv_diff_count+key_diff_count);
}


void main()
{
	uint32_t input[16], output[16], test_input[16], test_output[16], 
				last_test_output[16];
	uint32_t test_count;
	uint32_t bit_diff_count;
	uint32_t bitflip[512];
	FILE* fp = fopen("bit_freq.dat", "w");
	srand(time(NULL)); 
	
	input[0]=test_input[0]=1558840823;
	input[1]=test_input[1]=2172759703;
	input[2]=test_input[2]=107528440;
	input[3]=test_input[3]=3469342470;
	input[12]=test_input[12]=1485973784;
	input[13]=test_input[13]=2137582508;
	input[14]=test_input[14]=519037319;
	input[15]=test_input[15]=4226299334;

	input[4]=3157501560;
	input[5]=934121906;
	input[6]=1955533394;
	input[7]=327522372;
	input[8]=3699050964;
	input[9]=1824524917;
	input[10]=745189549;
	input[11]=812780999;

	//print_chacha_matrix(input, "Original Matrix");
	chacha_standard_routine(input,output);
	//print_chacha_matrix(output, "Original Matrix After ChaCha");


	test_input[4]=rand();
	test_input[5]=rand();
	test_input[6]=rand();
	test_input[7]=rand();
	test_input[8]=rand();
	test_input[9]=rand();
	test_input[10]=rand();
	test_input[11]=rand();

	
	uint32_t master_count;
	
	for(master_count=0;master_count<100;master_count++)
	{
		
		for(test_count=0;test_count<512;test_count++)
		{
			bitflip[test_count]=0;
		}
		
		for(test_count=0;test_count<10000;test_count++)
		{	
			
		
	
			if(test_count > 0)
			{	
				test_input[4]=test_output[4];
				test_input[5]=test_output[5];
				test_input[6]=test_output[6];
				test_input[7]=test_output[7];
				test_input[8]=test_output[8];
				test_input[9]=test_output[9];
				test_input[10]=test_output[10];
				test_input[11]=test_output[11];
			}
/*
			test_input[4]=rand();
			test_input[5]=rand();
			test_input[6]=rand();
			test_input[7]=rand();
			test_input[8]=rand();
			test_input[9]=rand();
			test_input[10]=rand();
			test_input[11]=rand();

*/
		

			chacha_standard_routine(test_input,test_output);
			calc_bit_diff(output, test_output, bitflip);
		}


		

		uint8_t keyword_start_pos = 4;
		uint32_t word=0;
		uint32_t similar=0;
		uint32_t tmp;

		for(test_count=128;test_count<385;test_count++)
		{

			if((tmp=(bitflip[test_count]>5000?(bitflip[test_count]-5000):(5000-bitflip[test_count]))) > 100)
			{
				similar++;
			}
			else
			{
				word +=1;
			}
			word <<= 1 ;
			if((test_count > 128) && (tmp=(test_count % 32) == 0))
			{

				test_output[keyword_start_pos] =rand() ^ word;
				word=0;
				keyword_start_pos++;
			}
		}
		printf("Similarity %u\n", similar);

	}
	for(test_count=0;test_count<400;test_count++)
		fprintf(fp, "%u %u \n", test_count+1, 
				(bitflip[test_count]>5000?(bitflip[test_count]-5000):(5000-bitflip[test_count])));
		
}


/*

	test_input[4]=rand();
	test_input[5]=rand();
	test_input[6]=rand();
	test_input[7]=rand();
	test_input[8]=rand();
	test_input[9]=rand();
	test_input[10]=rand();
	test_input[11]=rand();

*/
































