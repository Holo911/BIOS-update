#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ROMS 20       
#define MAX_FILENAME 256

// Checks if the filename is safe
int is_safe_filename(const char *filename) {
    for (int i = 0; filename[i] != '\0'; i++) {
        char c = filename[i];
        if (!isalnum(c) && c != '.' && c != '-' && c != '_') {
            return 0; 
        }
    }
    return 1; 
}

int main() {
    DIR *d;
    struct dirent *dir;
    
    char rom_files[MAX_ROMS][MAX_FILENAME]; 
    int rom_count = 0;
    char selected_rom[MAX_FILENAME] = "";

    printf("Starting Smart BIOS Update Process...\n");

    // 1. Open the folder and collect safe .rom files
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            int len = strlen(dir->d_name);
            
            if (len >= 4 && strcasecmp(dir->d_name + len - 4, ".rom") == 0) {
                if (is_safe_filename(dir->d_name)) {
                    if (rom_count < MAX_ROMS) {
                        
                        // strncpy prevents Buffer Overflows
                        strncpy(rom_files[rom_count], dir->d_name, MAX_FILENAME - 1);
                        // Guarantee the string ends safely, just in case it was too long
                        rom_files[rom_count][MAX_FILENAME - 1] = '\0'; 
                        
                        rom_count++;
                    }
                } else {
                    printf("WARNING: Ignored dangerously named file: %s\n", dir->d_name);
                }
            }
        }
        closedir(d);
    }

    // 2. Decide what to do based on how many files we found
    if (rom_count == 0) {
        printf("\nERROR: No valid .rom or .ROM file found in this folder!\n");
        printf("Please place the BIOS file in the same folder as this program.\n");
        printf("\nPress Enter to exit...");
        getchar();
        return 1;
    } 
    else if (rom_count == 1) {
        strncpy(selected_rom, rom_files[0], MAX_FILENAME - 1);
        selected_rom[MAX_FILENAME - 1] = '\0';
        printf("Found exactly 1 BIOS file: %s\n", selected_rom);
    } 
    else {
        printf("\nWARNING: Multiple ROM files detected!\n");
        printf("Please choose which file to use:\n\n");
        
        for (int i = 0; i < rom_count; i++) {
            printf("  [%d] %s\n", i + 1, rom_files[i]);
        }
        printf("  [0] Cancel and Exit\n");

        int choice = -1;
        while (choice < 0 || choice > rom_count) {
            printf("\nEnter your choice (0-%d): ", rom_count);
            
            if (scanf("%d", &choice) == 1) {
                if (choice == 0) {
                    printf("\nOperation canceled by user.\n");
                    return 0; 
                }
                if (choice >= 1 && choice <= rom_count) {
                    break; 
                }
            } else {
                printf("Invalid input. Please type a number.\n");
            }
            // Clear the invisible input buffer
            while (getchar() != '\n'); 
        }
        
        strncpy(selected_rom, rom_files[choice - 1], MAX_FILENAME - 1);
        selected_rom[MAX_FILENAME - 1] = '\0';
        printf("\nYou selected: %s\n", selected_rom);
        
        while (getchar() != '\n'); 
    }

    // 3. Execute the command securely
    printf("Please do not turn off your computer!\n\n");

    // SECURITY FIX: Using fork() and execvp() instead of system()
    
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        printf("\nCritical Error: Failed to initialize the execution process.\n");
    } 
    else if (pid == 0) {
        // --- CHILD PROCESS ---
        char *args[] = {"sudo", "./afulnx_64", selected_rom, "/b", "/p", "/n", "/x", NULL};
        
        execvp(args[0], args);
        
        printf("\nError: Could not execute the flashing tool. Is ./afulnx_64 in this folder?\n");
        exit(1);
    } 
    else {
        // --- PARENT PROCESS ---
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("\nBIOS update command completed successfully.\n");
        } else {
            printf("\nError: The BIOS update failed, was canceled, or the password was wrong.\n");
        }
    }

    printf("\nPress Enter to exit...");
    getchar(); 

    return 0;
}
