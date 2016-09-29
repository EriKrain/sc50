#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
    // Unsure proper usage
    if (argc != 2){
        fprintf (stderr, "Usage: ./recover file'sname\n");
        return 1;
    }
    // Remember name of input file
    char *cardFile = argv[1];
    // Unsure we can open input file 
    FILE *card = fopen(cardFile, "r");
    if (card == NULL){
        fprintf (stderr, "Can not open %s\n", argv[1]);
        return 2;
    }
    
    unsigned char buffer[512]; // An array for storing bytes
    int imageNumber = 0; // A counter for recovered images
    char filename[8]; // An array for saving image's name
    bool fileOpened = false; // An indicator, which shows if we have opened for writing files
    FILE *img; // Temporary place for images
    
    while (fread(buffer, 512, 1, card) != 0){ // Condition for non-end of input file
        // Identify if readed block of bytes is start of JPEG image
        if (buffer[0] == 0xff && 
            buffer[1] == 0xd8 && 
            buffer[2] == 0xff && 
            ((buffer[3] & 0xf0) == 0xe0)){
            // If we have already opened file for writing - close it
            if (fileOpened){
                fclose(img);
            }
            
            sprintf(filename, "%03i.jpg", imageNumber); 
            imageNumber++;
            img = fopen(filename, "w");
            fileOpened = true; 
            fwrite(buffer, 512, 1, img); 
            }
        // If readed block of bytes isn't a start of JPEG image 
        // and we have opened file - continue to wright down in it    
        else{
            if (fileOpened){
                fwrite(buffer, 512, 1, img);
            }
        }
    }
    
    fclose(card);
    fclose(img);
    return 0;
}