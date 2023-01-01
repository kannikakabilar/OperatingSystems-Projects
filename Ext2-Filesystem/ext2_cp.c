/*
This program takes three command line arguments. The first is the name of an ext2 formatted virtual disk. 
The second is the path to a file on the native operating system, and the third is an absolute path on the ext2 
formatted disk. The program should work like cp, copying the file on a native file system onto the specified 
location on the disk. If the specified file or target location does not exist, then the program returns
the appropriate error (ENOENT). Other important information in the inodes are set (e.g., i_dtime).
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ext2_utils.h"

unsigned char *disk = NULL;

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <image file name> <file on native OS> <path on ext2 image>\n", argv[0]);
        exit(1);
    }
    
    char *disk_image = argv[1];
    char *source_path = argv[2];
    char *destination_path = argv[3];
    
    disk = read_disk_image(disk_image);
    
    FILE *source_file = open_file(source_path);
    
    char *source_filename = get_file_name(source_path);
    
    struct ext2_dir_entry *copied_file = cp_create_target_file(destination_path, source_filename); // Make the copied file.
    
    struct ext2_inode *inode = get_inode_table() + INDEX(copied_file->inode);
    
    copy_sourcefile_data(inode, source_file); // Transfer the data from the source file into the copied file.
    
    fclose(source_file);
    
    return 0;
    
}
