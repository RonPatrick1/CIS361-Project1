/*
 * decrypt.c
 *
 *  Created on: Jun 25, 2016
 *      Author: Ron Patrick
 *
 *  CIS 361 - Project 1
 *  Encrypt - Decrypt
 *
 *  The 'linux.words' file must be in the same
 *  directory as this executable
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

//I used an open source hash map for C header from
//MIT.  
#include "khash.h"
#include "enc_dec.h"

//Macro that initializes the hash map
KHASH_MAP_INIT_STR(words, char*)
#define WORD_MAX_LEN 10
#define WORD_MIN_LEN 4

void print(double a[26][2]) {
    for (int i = 0 ; i < 26 ; i++ ) {
    	printf( "%c:%lf", (char)a[i][0],a[i][1] );
        printf( "\n" );
    }
}

int cmp(void *pa, void *pb) {
    double (*a)[2] = pa;
    double (*b)[2] = pb;
    if ((*a)[1] > (*b)[1]) return -1;
    if ((*a)[1] < (*b)[1]) return +1;
    return 0;
}

void setupFreqArray(double inArray[26][2], bool empty) {
    for(int i=0;i<26;i++) {
    	inArray[i][0]=i+65;
    }

    //Setup typical letter frequency appearances in
    //the English language(Wikipedia)
    if(!empty) {
		inArray[0][1]=8.167;
		inArray[1][1]=1.492;
		inArray[2][1]=2.782;
		inArray[3][1]=4.253;
		inArray[4][1]=12.702;
		inArray[5][1]=2.228;
		inArray[6][1]=2.015;
		inArray[7][1]=6.094;
		inArray[8][1]=6.966;
		inArray[9][1]=0.153;
		inArray[10][1]=0.772;
		inArray[11][1]=4.025;
		inArray[12][1]=2.406;
		inArray[13][1]=6.749;
		inArray[14][1]=7.507;
		inArray[15][1]=1.929;
		inArray[16][1]=0.095;
		inArray[17][1]=5.987;
		inArray[18][1]=6.327;
		inArray[19][1]=9.056;
		inArray[20][1]=2.758;
		inArray[21][1]=0.978;
		inArray[22][1]=2.361;
		inArray[23][1]=0.150;
		inArray[24][1]=1.974;
		inArray[25][1]=0.074;
    } else {
    	for(int i=0;i<26;i++) {
    		inArray[i][1]=0.0;
    	}
    }
}

int main(int argc, char *argv[]) {

    double freqs[26][2];
    setupFreqArray(freqs,false);
    //qsort( freqs, 26, sizeof freqs[0], cmp );

	time_t start,end;
	time (&start);

	if(argc<2) {
		fprintf(stderr, "Need to supply a file name\n");
		return 1;
	}
	char *inFile = argv[1];
	if(strlen(inFile)>4) {
		char extCheck[5];
		strncpy(extCheck,&inFile[strlen(inFile)-4],5);
		if(strcmp(extCheck,".enc")!=0) {
			fprintf(stderr, "Need to supply file with '.enc' extension\n");
			return 1;
		}
	} else {
		fprintf(stderr, "Need to supply file with '.enc' extension\n");
		return 1;
	}

	char outFile[strlen(inFile)];
	strncpy(outFile,inFile,strlen(inFile)-4);
	outFile[strlen(inFile)-4]='\0';
	strcat(outFile,".dec");

	int amount=0;
	if(argc==3) {
		sscanf(argv[2],"%d",&amount);
	}

	//Open file, get the file size, seek back to
	// the beginning of the file, create the
	// appropriate sized char array and then
	// read the file.
	FILE *fp = fopen(inFile,"r");
	if (fp == NULL) {
		fprintf(stderr,"Error opening file %s\n", inFile);
		return 1;
	}
	fseek(fp, 0L, SEEK_END);
	unsigned long sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	//rewind(fp);

	char input[sz+1];
	//char *input;
	//input=(char*)calloc(sz+1,1);
	input[sz]='\0';
	unsigned long bytesLeft=sz;
	unsigned long pos=0;
	size_t bytes_read=fread(&input, 1, bytesLeft, fp);
	bytesLeft-=bytes_read;
	pos+=bytes_read;
	while(bytesLeft!=0) {
		printf("bLeft:%ld\tbytes_read:%ld",bytesLeft,bytes_read);
		bytes_read=fread(&input+pos, 1, bytesLeft, fp);
		bytesLeft-=bytes_read;
		pos+=bytes_read;
		if(bytes_read!=sz) {
			fprintf(stderr, "Could not read whole file\n");
			fclose(fp);
			return 1;
		}
	}

	fclose(fp);


	//Since I got a command-line argument specifying
	//the amount to shift, I use that and return from
	//execution.
	if(amount!=0) {
		enc_dec(input,sz,amount);

		FILE *fout = fopen(outFile,"w");
		if (fout == NULL) {
			fprintf(stderr,"Error opening file %s\n", outFile);
			return 1;
		}
		fwrite(&input,1,sz,fout);
		fclose(fout);
		return 0;
	}

	int hash_ret;
	khiter_t word_iter;
	khash_t(words) *word_hash=kh_init(words);
	char line[2048];
	FILE *lwords = fopen("linux.words","r");
	if (lwords == NULL) {
		fprintf(stderr,"Error opening file %s\n", "linux.words");
		return 1;
	}

	//get all the words from the 'linux.words' file and
	//put them into a hash map
	int count=0;
	while(!feof(lwords)) {
		if(fgets(line,2048,lwords)!=NULL) {
			while(line[strlen(line)-1]=='\n' || line[strlen(line)-1]=='\r') {
				line[strlen(line)-1]='\0';
			}
			hash_ret=0;
			char *newWord;
			newWord=(char*)malloc(strlen(line));
			strcpy(newWord,line);
			word_iter=kh_put(words,word_hash,newWord,&hash_ret);
			if(hash_ret) {
				kh_value(word_hash,word_iter)=newWord;
				count++;
				//printf("words:%d\r",count);
			}
		}
	}
	fclose(lwords);

	//I didn't get a command-line argument specifying
	//how many places to shift so now I take a guess.

	double score;
	double maxScore=0.0;
	int currentPick=0;
	//char input_cpy[sz+1];
	char *input_cpy;
	input_cpy=(char*)calloc(sz+1,1);
	char tempStr[WORD_MAX_LEN];
	int char_try=0;
	double lFreqs[26][2];
	double letterScore=0;
	double totalLetters=0;
	bool tryWord=true;
	//double progress=0.0;
	//double inc=100.0/(26*(WORD_MAX_LEN-WORD_MIN_LEN));

	//Go through each shift and determine which one
	//has the highest 'score' and use that one.
	//(Only the first quarter megabyte of text)
	unsigned long oldSZ=sz;
	if(sz>1048576/4) {
		sz=1048576/4;
	}
	for(int i=1;i<95;i++) {
		strcpy(input_cpy,input);
		score=0.0;
		enc_dec(input_cpy,sz,i);
		setupFreqArray(lFreqs,true);
		totalLetters=0.0;

		//See how many words it find with words between
		//	WORD_MIN_LEN(4) letters long and WORD_MAX_LEN(8) letters long
		for(int word_len=WORD_MIN_LEN;word_len<WORD_MAX_LEN;word_len++) {
			for(int j=0;j<sz-word_len;j++) {

				//It runs 7.6 times faster without
				//printing these two lines

				//printf("%.2f%% done\r",progress);
				//progress+=(inc/(sz-word_len));

				strncpy(tempStr,input_cpy+j,word_len);
				tempStr[word_len]='\0';
				tryWord=true;
				for(int k=0;k<word_len;k++) {
					char_try=tempStr[k];
					if(char_try>96 && char_try<123) {
						continue;
					}
					if(char_try>64 && char_try<91) {
						continue;
					}
					k=word_len;
					tryWord=false;
				}
				if(tryWord) {
					if (kh_exist(word_hash, kh_get(words,word_hash,tempStr))) {
						for(int k=0;k<word_len;k++) {
							char_try=tempStr[k];
							if(char_try>96 && char_try<123) {
								char_try-=97;
								lFreqs[char_try][1]+=1.0;
								totalLetters++;
								continue;
							}
							if(char_try>64 && char_try<91) {
								char_try-=65;
								lFreqs[char_try][1]+=1.0;
								totalLetters++;
							}
						}
						score+=0.1;
					}
				}
			}
		}

		letterScore=1.0;
		for(int j=0;j<26;j++) {
			lFreqs[j][1]/=totalLetters;
			lFreqs[j][1]*=100.0;
			letterScore+=fabs(freqs[j][1]-lFreqs[j][1]);
		}

		// I was using this code to compare results by
		// the order of letters appearance
		//qsort( lFreqs, 26, sizeof lFreqs[0], cmp );
		/*for(int j=0;j<26;j++) {
			if(lFreqs[j][0]!=freqs[j][0]) {
				letterScore++;
			}
		}*/

		//printf("i:%d\tscore:%lf\t",i,score);
		score/=letterScore;
		//printf("new_score:%lf\n",score);
		if(score>maxScore) {
			maxScore=score;
			currentPick=i;
		}
	}
	printf("I think it's shifted by %d\n",currentPick);

	enc_dec(input,oldSZ,currentPick);

	FILE *fout = fopen(outFile,"w");
	if (fout == NULL) {
		fprintf(stderr,"Error opening file %s\n", outFile);
		return 1;
	}
	fwrite(&input,1,oldSZ,fout);
	fclose(fout);

	time (&end);
	double dif = difftime (end,start);
	printf ("Elasped time is %.5lf seconds.\n", dif );

    return 0;
}
