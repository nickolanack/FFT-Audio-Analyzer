/**
 *
 *
 * compile: gcc -o fourier fftanalyzer.c -lfftw3 -lm
 *
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>


#include <string.h>
#include <math.h>

int main(char argc, char *argv[]){

	double *in;
	fftw_complex* out;

	int N=512;


	fftw_plan p;
	// ...
	in = (double*) malloc(sizeof(double) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

	memset(in, 0, sizeof (double) * N);
	memset(out, 0, sizeof (fftw_complex) * N);

	p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
	// ...
 
	FILE *fp;
	long skipto=44600;
	fp=fopen("goforward.raw", "r");
        if(fp){
		
		int16_t buf[N];
		fseek(fp, 44600,SEEK_SET);
		size_t bytes=sizeof(int16_t);
		if(!feof(fp)){
	
			size_t len = fread(buf, bytes, N, fp);
			int i;
			for(i=0;i<len;i++){

				in[i]=(double)buf[i];

			}



			fftw_execute(p); /* repeat as needed */
		  	
			double re, im, mag, pha;

			long samples=N/2+1; //

			double samplerate=16000.0;
			double maxfreq=samplerate/2.0;
			double res=samplerate/N; // this is the frequency resolution in hertz for each frequency sample

			double freq;

			printf("// fast fourier transform output:\n"
					"{\n"
					"//sample rate - samples per second\n"
					"\"rate\":16000.0,\n"
					"//array of fft samples, representing frequecy magnitudes\n"
					"\"samples\":[\n");

			for(i=0;i<samples;i++){

				re = out[i][0]; //real component
				im = out[i][1]; //imaginary component
				mag = sqrt((re * re) + (im * im)); //magnitude

				//calculate phase
				if(re!=0){
					pha= atan(im/re);
					if(re<0){
						if(im>=0){
							pha+=M_PI;
						}else{
							pha-=M_PI;
						}
					}
				}else{
					if(im>0){
						pha=M_PI_2;
					}else if(im<0){
						pha=-M_PI_2;
					}else{
						pha=0;
					}
				}

				if(i==0){
					//this is the DC component
					printf("\t// the first sample is the dc component\n"
							"\t{\"m\":%0.3f, \"f\":\"DC\"},\n"
							"", mag, pha);
				}else{
					freq=(i/(double)(samples-1))*maxfreq;
					printf("\t{\"m\":%0.3f, \"p\":%lf, \"f_min\":%0.1f, \"f_max\":%0.1f}", mag, pha, freq-res, freq);
					if(i<samples-1){
						printf(",");
					}
					printf("\n");
				}

			}


			printf("\t],\n"
					"\"samples\":%d,\n"
					"\"resolution\":%0.3f,\n"
					"\"max_freq\":%0.3f,\n"
					"\"time_len_sec\":%0.2f\n"
					"}\n", samples, res, maxfreq, N/samplerate);

		}
	}
        fftw_destroy_plan(p);
        fftw_free(in); fftw_free(out);





}
