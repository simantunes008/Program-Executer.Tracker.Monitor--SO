#include <sys/types.h>
#include <unistd.h> // * chamadas ao sistema: defs e decls essenciais 
#include <fcntl.h> // * O_RDONLY, O_WRONLY, O_CREAT, O_* 

int main(int argc, char ** argv) {
    int from_fd = open(argv[1], O_RDONLY);
    if (from_fd < 0) 
    {
        return 1;
    }

    int to_fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);

    int bytes_read = 0;
    char buffer[64];

    while ((bytes_read = read(from_fd, buffer, 64)))
    {
        write(to_fd, buffer, bytes_read);
    }
    
    close(from_fd);
    close(to_fd);

    return 0;
}
