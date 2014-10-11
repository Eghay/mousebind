/* Binds mouse4 to Ctrl and mouse5 to Shift. */

#define EVENT_SIZE 72
char* mouse_name = "Xornet gaming mouse";
char sample_down_m4[EVENT_SIZE] = 
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x04\x00\x04\x00\x04\x00\x09\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x01\x00\x13\x01\x01\x00\x00\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00";
char sample_up_m4[EVENT_SIZE] = 
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x04\x00\x04\x00\x04\x00\x09\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x01\x00\x13\x01\x00\x00\x00\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00";
char sample_down_m5[EVENT_SIZE] = 
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x04\x00\x04\x00\x05\x00\x09\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x01\x00\x13\x01\x01\x00\x00\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x63\x80\x05\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00";
char sample_up_m5[EVENT_SIZE] = 
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x04\x00\x04\x00\x05\x00\x09\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x01\x00\x13\x01\x00\x00\x00\x00"
    "\x57\xC6\x2A\x54\x00\x00\x00\x00"
    "\x99\x66\x08\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00";
char* xdotool_argv_down_m4[] = {"xdotool", "keydown", "ctrl", 0};
char* xdotool_argv_up_m4[] = {"xdotool", "keyup", "ctrl", 0};
char* xdotool_argv_down_m5[] = {"xdotool", "keydown", "shift", 0};
char* xdotool_argv_up_m5[] = {"xdotool", "keyup", "shift", 0};

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int scan_devices(char* filename);
static int is_event_device(const struct dirent *dir);

/* main() */
int main(int argc, char** argv) {

    char filename[256] = "none", buffer[EVENT_SIZE];
    int fd = -1, i;

    scan_devices(filename);

    printf("Reading %s\n", filename);

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Error: open()\n");
        return -1;
    }

    while (read(fd, buffer, EVENT_SIZE)) {
    
        // Mouse 4.
        if (!memcmp(buffer + 16, sample_down_m4 + 16, 8)) {
            // State down/up.
            if (!memcmp(buffer + 43, sample_down_m4 + 43, 5)) {
                xdotool_main(3, xdotool_argv_down_m4);
            }
            else if (!memcmp(buffer + 43, sample_up_m4 + 43, 5)) {
                xdotool_main(3, xdotool_argv_up_m4);
            }
        } 
        // Mouse 5.
        else if (!memcmp(buffer + 16, sample_down_m5 + 16, 8)) {
            // State down/up.
            if (!memcmp(buffer + 43, sample_down_m5 + 43, 5)) {
                xdotool_main(3, xdotool_argv_down_m5);
            }
            else if (!memcmp(buffer + 43, sample_up_m5 + 43, 5)) {
                xdotool_main(3, xdotool_argv_up_m5);
            }
        }

        // Debug: Print data.
        //for (i = 0; i < EVENT_SIZE; i++) {
        //    printf("%02x", (unsigned char) buffer[i]);
        //}
        //printf("\n");
    }

    close(fd);
    return 0;
}


/* scan_devices() */
int scan_devices(char* filename) {

    struct dirent **namelist;
    int i, ndev, devnum, fd = -1;
    char fname[64], name[256] = "???";

    ndev = scandir("/dev/input", &namelist, is_event_device, alphasort);
    if (ndev <= 0) {
        return -1;
    }

    for (i = 0; i < ndev; i++) {

        snprintf(fname, sizeof(fname), "%s/%s", "/dev/input", 
            namelist[i]->d_name);

        if ((fd = open(fname, O_RDONLY)) < 0) {
            free(namelist[i]);
            continue;
        }
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);

        if (!strcmp(mouse_name, name)) {
            strcpy(filename, fname);
        }

        close(fd);
        free(namelist[i]);
    }

    return 0;
}


/* is_event_device() */
static int is_event_device(const struct dirent *dir) {
    return strncmp("event", dir->d_name, 5) == 0;
}

