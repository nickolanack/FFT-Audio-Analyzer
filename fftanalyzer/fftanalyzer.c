/**
 * This command line tool uses the open source fftw libraries http://www.fftw.org/ https://github.com/FFTW/fftw3
 * and it is assumed that the prerequiste libraries are installed and available
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


	FILE *fp;
	long skipto=44600;
	fp=fopen("goforward.wav", "r");
	if(fp){

		int N =512;
		fseek(fp, 44600, SEEK_CUR);

		int16_t buf[N];
		size_t len;
		size_t bytes=sizeof(int16_t);
		double *in;
		in = (double*) malloc(sizeof(double) * N);
		memset(in, 0, sizeof (double) * N);


		if(!feof(fp)){

			len = fread(buf, bytes, N, fp);
			int i;
			for(i=0;i<len;i++){

				in[i]=(double)buf[i];

			}

			if(len<N){

				//memset(in, 0, sizeof (double) * N);
			}

			fft(in, len);
		}



		return 0;

	}else{
		perror("failed to open stream");
		return 1;
	}



}

/**
 * @param FILE stream, file handle or stdin
 * @param int N number of samples to use for fft transform
 */
int fft(double *in, int length){


	fftw_complex* out;

	fftw_plan p;
	// ...

	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * length);

	memset(out, 0, sizeof (fftw_complex) * length);
	p = fftw_plan_dft_r2c_1d(length, in, out, FFTW_MEASURE);
	// ...

	fftw_execute(p); /* repeat as needed */

	double re, im, mag, pha;

	long samples=length/2+1; //

	double samplerate=16000.0;
	double maxfreq=samplerate/2.0;
	double res=samplerate/length; // this is the frequency resolution in hertz for each frequency sample

	double freq;

	printf("// fast fourier transform output:\n"
			"{\n"
			"//sample rate - samples per second\n"
			"\"rate\":16000.0,\n"
			"//array of fft samples, representing frequecy magnitudes\n"
			"\"samples\":[\n");
	int i;
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
					"\t{\"m\":%0.3f, \"f\":\"0.0\"},\n"
					"", mag, pha);
		}else{
			freq=(i/(double)(samples-1))*maxfreq;
			printf("\t{\"m\":%0.3f, \"p\":%lf, \"f\":%0.1f}", mag, pha, freq-(res/2.0));
			if(i<samples-1){
				printf(",");
			}
			printf("\n");
		}

	}


	printf("\t],\n"
			"\"count\":%d,\n"
			"\"resolution\":%0.3f,\n"
			"\"max_freq\":%0.3f,\n"
			"\"time_len_sec\":%0.2f\n"
			"}\n", samples, res, maxfreq, length/samplerate);



	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(out);
}
