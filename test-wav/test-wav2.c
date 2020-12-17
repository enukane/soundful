#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>

struct wav_header {
    uint8_t chunk_id[4];
    uint32_t chunk_size;
    uint8_t form_type[4];
    uint8_t sub_chunk_fmt_id[4];
    uint32_t sub_chunk_fmt_size;
    #define LPCM (1)
    uint16_t format_tag;
    #define MONO (1)
    #define STEREO (2)
    uint16_t channels;
    #define SAMPLE48KHZ (48*1000)
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    #define BITSPERSAMPLE (16)
    uint16_t bits_per_sample;
    uint8_t sub_chunk_data_id[4];
    uint32_t sub_chunk_data_size;
} __attribute__((packed));

struct wav_header wav_template = {
    "RIFF",
    0,
    "WAVE",
    "fmt ",
    16,
    LPCM,
    MONO,
    SAMPLE48KHZ,
    SAMPLE48KHZ * MONO * BITSPERSAMPLE/8,
    MONO*BITSPERSAMPLE/8,
    BITSPERSAMPLE,
    "data",
    0,
};

double
chirp(int x, int n, double sampleing_freq, double start_freq, double end_freq, double amp)
{
    double k = (end_freq - start_freq) / (double) n;
    return amp * sin(2.0 * M_PI / start_freq * (start_freq * (double)x + k / 2.0 * (double)x * (double)x));
}

double
sinwave(int x, int n, double sampling_freq, double base_freq, double amp)
{
    return amp * sin(2.0 * M_PI * base_freq * x / sampling_freq);
}

void
create_and_write_data(FILE *fp, int n, double base_freq, double amp)
{
    int i;
    size_t ret;

    for (i = 0; i < n; i++) {
        if (i != 0 && i % 48000 == 0) {
            //base_freq *= 1.5;
            base_freq += 1000;
        }
        int16_t s = (int16_t) sinwave(i, n, (double)SAMPLE48KHZ, base_freq, amp);
        //printf("%d: %d\n", i, s);
        ret = fwrite(&s, 1, sizeof(s), fp);
        if (ret != sizeof(s)) {
            perror("error fwrite");
            return;
        }
    }
}

void
usage()
{
    printf("usage: test-wav [base] [amp] [n]");
}

int
main(int argc, char *argv[])
{
    int base_freq, amp, n;
    size_t ret;
    FILE *fp;

    if (argc != 4) {
        usage();
        return 1;
    }

    base_freq = atoi(argv[1]);
    amp = atoi(argv[2]);
    n = atoi(argv[3]);

    printf("base=%d amp=%d n=%d\n", base_freq, amp, n);

    fp = fopen("test2.wav", "wb");
    if (fp == NULL) {
        perror("error fopen");
        return 1;
    }

    wav_template.sub_chunk_data_size = n * sizeof(int16_t);
    printf("chunk data size : %d\n", wav_template.sub_chunk_data_size);
    wav_template.chunk_size = 36 + wav_template.sub_chunk_data_size;

    ret = fwrite(&wav_template, 1, sizeof(wav_template), fp);
    if (ret != sizeof(wav_template)) {
        printf("error fwrite header");
        return -1;
    }

    create_and_write_data(fp, n, (double)base_freq, (double)amp);

    fclose(fp);
    printf("DONE\n");
    return 0;
}