/*
 * encrypt.c
 *
 *  Created on: Jun 25, 2016
 *      Author: Ron Patrick
 *
 *  CIS 361 - Project 1
 *  Encrypt - Decrypt
 *
 */

#include <stdio.h>
#include <string.h>
#include "enc_dec.h"

int main(int argc, char *argv[]) {
	if(argc!=3) {
		fprintf(stderr, "Need to supply a file name and a shift amount\n");
		return 1;
	}
	char *inFile = argv[1];
	int amount=0;
	sscanf(argv[2],"%d",&amount);

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
	input[sz]='\0';
	size_t bytes_read=fread(&input, 1, sz, fp);
	if(bytes_read!=sz) {
		fprintf(stderr, "Could not read whole file\n");
		return 1;
	}
	fclose(fp);

	enc_dec(input,sz,amount);

	strcat(inFile,".enc");
	FILE *fout = fopen(inFile,"w");
	if (fout == NULL) {
		fprintf(stderr,"Error opening file %s\n", inFile);
		return 1;
	}
	fwrite(&input,1,sz,fout);
	fclose(fout);
	return 0;
}
