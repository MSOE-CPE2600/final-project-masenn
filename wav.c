/**
 * @file wav.c
 * @brief wave file for final lab 2024
 * 
 * Course: CPE2600
 * Section: 011
 * Assignment: Final
 * Name: Matthew Senn
 */

#include "wav.h"


float midi_to_freq(float num) {
    return 440 * pow(2,(num-69)/12);
}

void write_wave_header(FILE *file, WaveParameters* params) {
    // Calculate sizes
    int sample_freq = params->sample_freq;
    int sample_length = params->sample_length;
    int sample_channels = params->sample_channels;
    int num_samples = sample_freq*sample_length;
    int bit_depth = params->bit_depth;
    int byte_rate = sample_freq * sample_channels * (bit_depth / 8);
    int block_align = sample_channels * (bit_depth / 8);
    int data_chunk_size = num_samples * sample_channels * (bit_depth / 8);
    int riff_chunk_size = 4 + (8 + 16) + (8 + data_chunk_size); // "WAVE" + fmt chunk + data chunk

    // Write the RIFF chunk descriptor
    fwrite("RIFF", 1, 4, file);                        // Chunk ID
    fwrite(&riff_chunk_size, 4, 1, file);              // Chunk Size
    fwrite("WAVE", 1, 4, file);                        // Format

    // Write the fmt subchunk
    fwrite("fmt ", 1, 4, file);                        // Subchunk ID
    int fmt_subchunk_size = 16;                        // Subchunk Size (PCM format)
    fwrite(&fmt_subchunk_size, 4, 1, file);
    uint16_t audio_format = 1;                         // Audio Format (1 = PCM)
    fwrite(&audio_format, 2, 1, file);
    fwrite(&sample_channels, 2, 1, file);                 // Number of Channels
    fwrite(&sample_freq, 4, 1, file);                  // Sample Rate
    fwrite(&byte_rate, 4, 1, file);                    // Byte Rate
    fwrite(&block_align, 2, 1, file);                  // Block Align
    fwrite(&bit_depth, 2, 1, file);              // Bits Per Sample

    // Write the data subchunk
    fwrite("data", 1, 4, file);                        // Subchunk ID
    fwrite(&data_chunk_size, 4, 1, file);              // Subchunk Size
}

void generate_sample(FILE* fp, WaveParameters* params){
    int sample_freq = params->sample_freq;
    int sample_channels = params->sample_channels;     
    int sample_length = params->sample_length;          
    float freq_base = params->freq_base;                
    float amp_depth = params->amp_depth;                
    float freq_depth = params->freq_depth;              
    float rot_freq = params->rot_freq;                  
    uint16_t max_val = params->MAX_VAL;

    for(int i = 0; i < sample_freq*sample_length; i++){
        for(int channel = 0; channel < sample_channels; channel++){
            float t = (double)i/(double)(sample_freq);
            float val = sin(2 * M_PI * freq_base * t);   
            val *= (1 + amp_depth)*sin(2*M_PI*rot_freq * t);      
            val *= VOLUME;  
            val *= max_val;
            int16_t point = (int16_t)val;
            fwrite((const void*)&point,sizeof(point),1,fp);
        }
    }
    fflush(fp);
}

void generate_package(WaveParameters* params){
   // creating directories 
   create_dir("audio/");
    static char save_path[256];
    strcpy(save_path,"");
    printf("Starting with save path %s: \n",save_path);
    printf("name: %s\n", params->name);
   strcat(save_path,"audio/");
   strcat(save_path,params->name);
//    strcat(save_path,"/");
   create_dir(save_path);

    char* data[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    char fname[256];

   for(int note = 0; note < 128; note++){
        strcpy(fname,"");
        sprintf(fname, "%03d-%s.wav", note,data[note%12]);
        params->freq_base = midi_to_freq(note);
        // printf("freq: %f",midi_to_freq(note));
        //initilizing file
        FILE* fp;
        char file_path[256];
        strcpy(file_path,"");

      strcat(file_path,save_path);
      strcat(file_path,"/");
      strcat(file_path,fname);

    //   printf("Writing to file: %s\n",file_path);
      //opening file
      fp = fopen(file_path,"w");
      if(fp == NULL){
         printf("Error Opening file!\n");
         return;
      }
      int num_bytes = params->sample_freq * params->sample_channels * params->sample_length;
      write_wave_header(fp,params);
      generate_sample(fp,params);
      printf("Generating %s\n",file_path);
      fflush(fp);
      fclose(fp);
   }
}

void create_dir(const char* path) {
    printf("Attempting to create:%s\n",path);
    if (mkdir(path, 0700) == 0) {
        printf("Directory created successfully: %s\n", path);
    } else {
        // Check if the error is because the directory already exists
        if (errno == EEXIST) {
            printf("Directory already exists: %s\n", path);
        } else {
            perror("Error creating directory");
        }
    }
}

