/*
 * enc_dec.h
 *
 *  Created on: Jun 25, 2016
 *      Author: Ron Patrick
 *
 *  CIS 361 - Project 1
 *  Encrypt - Decrypt
 *
 */

#ifndef SRC_ENC_DEC_H_
#define SRC_ENC_DEC_H_

void enc_dec(char *arr, long len, int amount) {
	while(amount<0) {
		amount+=95;
	}
	amount=amount%95;
	for(int i=0;i<len;i++) {
		if(arr[i]>31 && arr[i]<127) {
			arr[i]=(((arr[i]-32)+amount)%95)+32;
		}
		/*if(arr[i]>96 && arr[i]<123) {
			arr[i]=(((arr[i]-97)+amount)%26)+97;
			continue;
		}
		if(arr[i]>64 && arr[i]<91) {
			arr[i]=(((arr[i]-65)+amount)%26)+65;
			continue;
		}*/
	}
}

#endif /* SRC_ENC_DEC_H_ */
