#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char ** argv) {
    int bytes_read = 0;
    char buffer[64];

    int from_fd = open(argv[1], O_RDONLY);
    if (from_fd < 0) return 1;

    int to_fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (to_fd < 0) return 1;

    while ((bytes_read = read(from_fd, buffer, 64))) {
        write(to_fd, buffer, bytes_read);
    }
    
    close(from_fd);
    close(to_fd);

    return 0;
}
