#include <sys/types.h>
#include <unistd.h> // * chamadas ao sistema: defs e decls essenciais 
#include <fcntl.h> // * O_RDONLY, O_WRONLY, O_CREAT, O_* 

int main(int argc, char ** argv) {
    int bytes_read = 0;
    char buffer[64];

    if (argc == 1) {
        while ((bytes_read = read(STDIN_FILENO, buffer, 64)) > 0)
        {
            write(STDOUT_FILENO, buffer, bytes_read);
        }
    } else {
        for (int i = 1; i < argc; i++) {
            int from_fd = open(argv[1], O_RDONLY);
            if (from_fd < 0) return 1;

            while ((bytes_read = read(from_fd, buffer, 64)) > 0)
            {
                write(STDOUT_FILENO, buffer, bytes_read);
            }
        }
    }
    
    return 0;
}
