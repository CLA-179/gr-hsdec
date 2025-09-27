/*
 * Reed-Solomon encoder/decoder
 * Copyright 2004, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

#include <stdio.h>
#include <string.h>

#include "rs.h"

/* The guts of the Reed-Solomon encoder, meant to be #included
 * into a function body with the following typedefs, macros and variables supplied
 * according to the code parameters:

 * unsigned char - a typedef for the data symbol
 * unsigned char data[] - array of NN-NROOTS-PAD and type unsigned char to be encoded
 * unsigned char parity[] - an array of NROOTS and type unsigned char to be written with parity symbols
 * NROOTS - the number of roots in the RS code generator polynomial,
 *          which is the same as the number of parity symbols in a block.
            Integer variable or literal.
	    * 
 * NN - the total number of symbols in a RS block. Integer variable or literal.
 * PAD - the number of pad symbols in a block. Integer variable or literal.
 * ALPHA_TO - The address of an array of NN elements to convert Galois field
 *            elements in index (log) form to polynomial form. Read only.
 * INDEX_OF - The address of an array of NN elements to convert Galois field
 *            elements in polynomial form to index (log) form. Read only.
 * MODNN - a function to reduce its argument modulo NN. May be inline or a macro.
 * GENPOLY - an array of NROOTS+1 elements containing the generator polynomial in index form

 * The memset() and memmove() functions are used. The appropriate header
 * file declaring these functions (usually <string.h>) must be included by the calling
 * program.

 * Copyright 2004, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */


#undef A0
#define A0 (NN) /* Special reserved value encoding zero in index form */

unsigned char Taltab[] = {

    0x00, 0x7b, 0xaf, 0xd4, 0x99, 0xe2, 0x36, 0x4d, 0xfa, 0x81, 0x55, 0x2e, 0x63, 0x18,
    0xcc, 0xb7, 0x86, 0xfd, 0x29, 0x52, 0x1f, 0x64, 0xb0, 0xcb, 0x7c, 0x07, 0xd3, 0xa8,
    0xe5, 0x9e, 0x4a, 0x31, 0xec, 0x97, 0x43, 0x38, 0x75, 0x0e, 0xda, 0xa1, 0x16, 0x6d,
    0xb9, 0xc2, 0x8f, 0xf4, 0x20, 0x5b, 0x6a, 0x11, 0xc5, 0xbe, 0xf3, 0x88, 0x5c, 0x27,
    0x90, 0xeb, 0x3f, 0x44, 0x09, 0x72, 0xa6, 0xdd, 0xef, 0x94, 0x40, 0x3b, 0x76, 0x0d,
    0xd9, 0xa2, 0x15, 0x6e, 0xba, 0xc1, 0x8c, 0xf7, 0x23, 0x58, 0x69, 0x12, 0xc6, 0xbd,
    0xf0, 0x8b, 0x5f, 0x24, 0x93, 0xe8, 0x3c, 0x47, 0x0a, 0x71, 0xa5, 0xde, 0x03, 0x78,
    0xac, 0xd7, 0x9a, 0xe1, 0x35, 0x4e, 0xf9, 0x82, 0x56, 0x2d, 0x60, 0x1b, 0xcf, 0xb4,
    0x85, 0xfe, 0x2a, 0x51, 0x1c, 0x67, 0xb3, 0xc8, 0x7f, 0x04, 0xd0, 0xab, 0xe6, 0x9d,
    0x49, 0x32, 0x8d, 0xf6, 0x22, 0x59, 0x14, 0x6f, 0xbb, 0xc0, 0x77, 0x0c, 0xd8, 0xa3,
    0xee, 0x95, 0x41, 0x3a, 0x0b, 0x70, 0xa4, 0xdf, 0x92, 0xe9, 0x3d, 0x46, 0xf1, 0x8a,
    0x5e, 0x25, 0x68, 0x13, 0xc7, 0xbc, 0x61, 0x1a, 0xce, 0xb5, 0xf8, 0x83, 0x57, 0x2c,
    0x9b, 0xe0, 0x34, 0x4f, 0x02, 0x79, 0xad, 0xd6, 0xe7, 0x9c, 0x48, 0x33, 0x7e, 0x05,
    0xd1, 0xaa, 0x1d, 0x66, 0xb2, 0xc9, 0x84, 0xff, 0x2b, 0x50, 0x62, 0x19, 0xcd, 0xb6,
    0xfb, 0x80, 0x54, 0x2f, 0x98, 0xe3, 0x37, 0x4c, 0x01, 0x7a, 0xae, 0xd5, 0xe4, 0x9f,
    0x4b, 0x30, 0x7d, 0x06, 0xd2, 0xa9, 0x1e, 0x65, 0xb1, 0xca, 0x87, 0xfc, 0x28, 0x53,
    0x8e, 0xf5, 0x21, 0x5a, 0x17, 0x6c, 0xb8, 0xc3, 0x74, 0x0f, 0xdb, 0xa0, 0xed, 0x96,
    0x42, 0x39, 0x08, 0x73, 0xa7, 0xdc, 0x91, 0xea, 0x3e, 0x45, 0xf2, 0x89, 0x5d, 0x26,
    0x6b, 0x10, 0xc4, 0xbf,
};

unsigned char Tal1tab[] = {
    0x00, 0xcc, 0xac, 0x60, 0x79, 0xb5, 0xd5, 0x19, 0xf0, 0x3c, 0x5c, 0x90, 0x89, 0x45,
    0x25, 0xe9, 0xfd, 0x31, 0x51, 0x9d, 0x84, 0x48, 0x28, 0xe4, 0x0d, 0xc1, 0xa1, 0x6d,
    0x74, 0xb8, 0xd8, 0x14, 0x2e, 0xe2, 0x82, 0x4e, 0x57, 0x9b, 0xfb, 0x37, 0xde, 0x12,
    0x72, 0xbe, 0xa7, 0x6b, 0x0b, 0xc7, 0xd3, 0x1f, 0x7f, 0xb3, 0xaa, 0x66, 0x06, 0xca,
    0x23, 0xef, 0x8f, 0x43, 0x5a, 0x96, 0xf6, 0x3a, 0x42, 0x8e, 0xee, 0x22, 0x3b, 0xf7,
    0x97, 0x5b, 0xb2, 0x7e, 0x1e, 0xd2, 0xcb, 0x07, 0x67, 0xab, 0xbf, 0x73, 0x13, 0xdf,
    0xc6, 0x0a, 0x6a, 0xa6, 0x4f, 0x83, 0xe3, 0x2f, 0x36, 0xfa, 0x9a, 0x56, 0x6c, 0xa0,
    0xc0, 0x0c, 0x15, 0xd9, 0xb9, 0x75, 0x9c, 0x50, 0x30, 0xfc, 0xe5, 0x29, 0x49, 0x85,
    0x91, 0x5d, 0x3d, 0xf1, 0xe8, 0x24, 0x44, 0x88, 0x61, 0xad, 0xcd, 0x01, 0x18, 0xd4,
    0xb4, 0x78, 0xc5, 0x09, 0x69, 0xa5, 0xbc, 0x70, 0x10, 0xdc, 0x35, 0xf9, 0x99, 0x55,
    0x4c, 0x80, 0xe0, 0x2c, 0x38, 0xf4, 0x94, 0x58, 0x41, 0x8d, 0xed, 0x21, 0xc8, 0x04,
    0x64, 0xa8, 0xb1, 0x7d, 0x1d, 0xd1, 0xeb, 0x27, 0x47, 0x8b, 0x92, 0x5e, 0x3e, 0xf2,
    0x1b, 0xd7, 0xb7, 0x7b, 0x62, 0xae, 0xce, 0x02, 0x16, 0xda, 0xba, 0x76, 0x6f, 0xa3,
    0xc3, 0x0f, 0xe6, 0x2a, 0x4a, 0x86, 0x9f, 0x53, 0x33, 0xff, 0x87, 0x4b, 0x2b, 0xe7,
    0xfe, 0x32, 0x52, 0x9e, 0x77, 0xbb, 0xdb, 0x17, 0x0e, 0xc2, 0xa2, 0x6e, 0x7a, 0xb6,
    0xd6, 0x1a, 0x03, 0xcf, 0xaf, 0x63, 0x8a, 0x46, 0x26, 0xea, 0xf3, 0x3f, 0x5f, 0x93,
    0xa9, 0x65, 0x05, 0xc9, 0xd0, 0x1c, 0x7c, 0xb0, 0x59, 0x95, 0xf5, 0x39, 0x20, 0xec,
    0x8c, 0x40, 0x54, 0x98, 0xf8, 0x34, 0x2d, 0xe1, 0x81, 0x4d, 0xa4, 0x68, 0x08, 0xc4,
    0xdd, 0x11, 0x71, 0xbd,
};

void encode_rs_ccsds(unsigned char* data, unsigned char* parity, int pad)
{
    int i;
    unsigned char cdata[NN - NROOTS];

    /* Convert data from dual basis to conventional */
    for (i = 0; i < NN - NROOTS - pad; i++)
        cdata[i] = Tal1tab[data[i]];

    encode_rs(cdata, parity, pad);

    /* Convert parity from conventional to dual basis */
    for (i = 0; i < NROOTS; i++)
        parity[i] = Taltab[parity[i]];
}

void encode_rs(unsigned char *data, unsigned char *parity, int pad)
{
    int i, j;
    unsigned char feedback;

    memset(parity, 0, NROOTS * sizeof(unsigned char));

    for (i = 0; i < NN - NROOTS - PAD; i++) {
        feedback = INDEX_OF[data[i] ^ parity[0]];
        if (feedback != A0) {      /* feedback term is non-zero */
#ifdef UNNORMALIZED
            /* This line is unnecessary when GENPOLY[NROOTS] is unity, as it must
             * always be for the polynomials constructed by init_rs()
             */
            feedback = MODNN(NN - GENPOLY[NROOTS] + feedback);
#endif
            for (j = 1; j < NROOTS; j++)
                parity[j] ^= ALPHA_TO[MODNN(feedback + GENPOLY[NROOTS-j])];
        }
    
        /* Shift */
        memmove(&parity[0], &parity[1], sizeof(unsigned char) * (NROOTS-1));
        parity[NROOTS-1] = (feedback != A0) ? ALPHA_TO[MODNN(feedback + GENPOLY[0])] : 0;
    }
}

/* The guts of the Reed-Solomon decoder, meant to be #included
 * into a function body with the following typedefs, macros and variables supplied
 * according to the code parameters:

 * unsigned char - a typedef for the data symbol
 * unsigned char data[] - array of NN data and parity symbols to be corrected in place
 * retval - an integer lvalue into which the decoder's return code is written
 * NROOTS - the number of roots in the RS code generator polynomial,
 *          which is the same as the number of parity symbols in a block.
            Integer variable or literal.
 * NN - the total number of symbols in a RS block. Integer variable or literal.
 * PAD - the number of pad symbols in a block. Integer variable or literal.
 * ALPHA_TO - The address of an array of NN elements to convert Galois field
 *            elements in index (log) form to polynomial form. Read only.
 * INDEX_OF - The address of an array of NN elements to convert Galois field
 *            elements in polynomial form to index (log) form. Read only.
 * MODNN - a function to reduce its argument modulo NN. May be inline or a macro.
 * FCR - An integer literal or variable specifying the first consecutive root of the
 *       Reed-Solomon generator polynomial. Integer variable or literal.
 * PRIM - The primitive root of the generator poly. Integer variable or literal.
 * DEBUG - If set to 1 or more, do various internal consistency checking. Leave this
 *         undefined for production code

 * The memset(), memmove(), and memcpy() functions are used. The appropriate header
 * file declaring these functions (usually <string.h>) must be included by the calling
 * program.
 */

int decode_rs(unsigned char *data, int *eras_pos, int no_eras, int pad){
	int retval;

	if(pad < 0 || pad > 222){
		return -1;
	}

	int deg_lambda, el, deg_omega;
	int i, j, r,k;
	unsigned char u,q,tmp,num1,num2,den,discr_r;
	unsigned char lambda[NROOTS+1], s[NROOTS];	/* Err+Eras Locator poly
						 * and syndrome poly */
	unsigned char b[NROOTS+1], t[NROOTS+1], omega[NROOTS+1];
	unsigned char root[NROOTS], reg[NROOTS+1], loc[NROOTS];
	int syn_error, count;

	/* form the syndromes; i.e., evaluate data(x) at roots of g(x) */
	for(i=0;i<NROOTS;i++)
		s[i] = data[0];

	for(j=1;j<NN-PAD;j++){
		for(i=0;i<NROOTS;i++){
			if(s[i] == 0){
				s[i] = data[j];
			} else {
				s[i] = data[j] ^ ALPHA_TO[MODNN(INDEX_OF[s[i]] + (FCR+i)*PRIM)];
			}
		}
	}

	/* Convert syndromes to index form, checking for nonzero condition */
	syn_error = 0;
	for(i=0;i<NROOTS;i++){
		syn_error |= s[i];
		s[i] = INDEX_OF[s[i]];
	}

	if (!syn_error) {
		/* if syndrome is zero, data[] is a codeword and there are no
		 * errors to correct. So return data[] unmodified
		 */
		count = 0;
		goto finish;
	}
	memset(&lambda[1],0,NROOTS*sizeof(lambda[0]));
	lambda[0] = 1;

	if (no_eras > 0) {
		/* Init lambda to be the erasure locator polynomial */
		lambda[1] = ALPHA_TO[MODNN(PRIM*(NN-1-eras_pos[0]))];
		for (i = 1; i < no_eras; i++) {
			u = MODNN(PRIM*(NN-1-eras_pos[i]));
			for (j = i+1; j > 0; j--) {
				tmp = INDEX_OF[lambda[j - 1]];
				if(tmp != A0)
					lambda[j] ^= ALPHA_TO[MODNN(u + tmp)];
			}
		}

#if DEBUG >= 1
		/* Test code that verifies the erasure locator polynomial just constructed
		   Needed only for decoder debugging. */

		/* find roots of the erasure location polynomial */
		for(i=1;i<=no_eras;i++)
			reg[i] = INDEX_OF[lambda[i]];

		count = 0;
		for (i = 1,k=IPRIM-1; i <= NN; i++,k = MODNN(k+IPRIM)) {
			q = 1;
			for (j = 1; j <= no_eras; j++)
				if (reg[j] != A0) {
					reg[j] = MODNN(reg[j] + j);
					q ^= ALPHA_TO[reg[j]];
				}
			if (q != 0)
				continue;
			/* store root and error location number indices */
			root[count] = i;
			loc[count] = k;
			count++;
		}
		if (count != no_eras) {
			printf("count = %d no_eras = %d\n lambda(x) is WRONG\n",count,no_eras);
			count = -1;
			goto finish;
		}
#if DEBUG >= 2
		printf("\n Erasure positions as determined by roots of Eras Loc Poly:\n");
		for (i = 0; i < count; i++)
			printf("%d ", loc[i]);
		printf("\n");
#endif
#endif
	}
	for(i=0;i<NROOTS+1;i++)
		b[i] = INDEX_OF[lambda[i]];

	/*
	 * Begin Berlekamp-Massey algorithm to determine error+erasure
	 * locator polynomial
	 */
	r = no_eras;
	el = no_eras;
	while (++r <= NROOTS) {	/* r is the step number */
		/* Compute discrepancy at the r-th step in poly-form */
		discr_r = 0;
		for (i = 0; i < r; i++){
			if ((lambda[i] != 0) && (s[r-i-1] != A0)) {
				discr_r ^= ALPHA_TO[MODNN(INDEX_OF[lambda[i]] + s[r-i-1])];
			}
		}
		discr_r = INDEX_OF[discr_r];	/* Index form */
		if (discr_r == A0) {
			/* 2 lines below: B(x) <-- x*B(x) */
			memmove(&b[1],b,NROOTS*sizeof(b[0]));
			b[0] = A0;
		} else {
			/* 7 lines below: T(x) <-- lambda(x) - discr_r*x*b(x) */
			t[0] = lambda[0];
			for (i = 0 ; i < NROOTS; i++) {
				if(b[i] != A0)
					t[i+1] = lambda[i+1] ^ ALPHA_TO[MODNN(discr_r + b[i])];
				else
					t[i+1] = lambda[i+1];
			}
			if (2 * el <= r + no_eras - 1) {
				el = r + no_eras - el;
				/*
				 * 2 lines below: B(x) <-- inv(discr_r) *
				 * lambda(x)
				 */
				for (i = 0; i <= NROOTS; i++)
					b[i] = (lambda[i] == 0) ? A0 : MODNN(INDEX_OF[lambda[i]] - discr_r + NN);
			} else {
				/* 2 lines below: B(x) <-- x*B(x) */
				memmove(&b[1],b,NROOTS*sizeof(b[0]));
				b[0] = A0;
			}
			memcpy(lambda,t,(NROOTS+1)*sizeof(t[0]));
		}
	}

	/* Convert lambda to index form and compute deg(lambda(x)) */
	deg_lambda = 0;
	for(i=0;i<NROOTS+1;i++){
		lambda[i] = INDEX_OF[lambda[i]];
		if(lambda[i] != A0)
			deg_lambda = i;
	}
	/* Find roots of the error+erasure locator polynomial by Chien search */
	memcpy(&reg[1],&lambda[1],NROOTS*sizeof(reg[0]));
	count = 0;		/* Number of roots of lambda(x) */
	for (i = 1,k=IPRIM-1; i <= NN; i++,k = MODNN(k+IPRIM)) {
		q = 1; /* lambda[0] is always 0 */
		for (j = deg_lambda; j > 0; j--){
			if (reg[j] != A0) {
				reg[j] = MODNN(reg[j] + j);
				q ^= ALPHA_TO[reg[j]];
			}
		}
		if (q != 0)
			continue; /* Not a root */
		/* store root (index-form) and error location number */
#if DEBUG>=2
		printf("count %d root %d loc %d\n",count,i,k);
#endif
		root[count] = i;
		loc[count] = k;
		/* If we've already found max possible roots,
		 * abort the search to save time
		 */
		if(++count == deg_lambda)
			break;
	}
	if (deg_lambda != count) {
		/*
		 * deg(lambda) unequal to number of roots => uncorrectable
		 * error detected
		 */
		count = -1;
		goto finish;
	}
	/*
	 * Compute err+eras evaluator poly omega(x) = s(x)*lambda(x) (modulo
	 * x**NROOTS). in index form. Also find deg(omega).
	 */
	deg_omega = deg_lambda-1;
	for (i = 0; i <= deg_omega;i++){
		tmp = 0;
		for(j=i;j >= 0; j--){
			if ((s[i - j] != A0) && (lambda[j] != A0))
				tmp ^= ALPHA_TO[MODNN(s[i - j] + lambda[j])];
		}
		omega[i] = INDEX_OF[tmp];
	}

	/*
	 * Compute error values in poly-form. num1 = omega(inv(X(l))), num2 =
	 * inv(X(l))**(FCR-1) and den = lambda_pr(inv(X(l))) all in poly-form
	 */
	for (j = count-1; j >=0; j--) {
		num1 = 0;
		for (i = deg_omega; i >= 0; i--) {
			if (omega[i] != A0)
				num1  ^= ALPHA_TO[MODNN(omega[i] + i * root[j])];
		}
		num2 = ALPHA_TO[MODNN(root[j] * (FCR - 1) + NN)];
		den = 0;

		/* lambda[i+1] for i even is the formal derivative lambda_pr of lambda[i] */
		for (i = MIN(deg_lambda,NROOTS-1) & ~1; i >= 0; i -=2) {
			if(lambda[i+1] != A0)
				den ^= ALPHA_TO[MODNN(lambda[i+1] + i * root[j])];
		}
#if DEBUG >= 1
		if (den == 0) {
			printf("\n ERROR: denominator = 0\n");
			count = -1;
			goto finish;
		}
#endif
		/* Apply error to data */
		if (num1 != 0 && loc[j] >= PAD) {
			data[loc[j]-PAD] ^= ALPHA_TO[MODNN(INDEX_OF[num1] + INDEX_OF[num2] + NN - INDEX_OF[den])];
		}
	}
finish:
	if(eras_pos != NULL){
		for(i=0;i<count;i++)
			eras_pos[i] = loc[i];
	}
	retval = count;
	return retval;
}
