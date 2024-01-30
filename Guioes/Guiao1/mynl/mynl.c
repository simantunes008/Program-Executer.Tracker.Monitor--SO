#include "mynl.h"

int readch(int fd, char * buf) {
    return read(fd, buf, 1);
}

ssize_t readln(int fd, char * line, size_t size) {
    int next_pos = 0;
    int read_bytes = 0;
    while (next_pos < size && readch(fd, line + next_pos) > 0) {
        read_bytes++;
        if (line[next_pos] == '\n') break;
        next_pos++;
    }
    return read_bytes;
}

int mynl() {
    char line[1024], buf[24];
    int read_bytes, prefix_bytes, line_number = 1;
    while ((read_bytes = readln(STDIN_FILENO, line, sizeof(line))) > 0) {
        if (read_bytes == 1) write(STDOUT_FILENO,"\n",1);
        else {
            prefix_bytes = sprintf(buf, "    %d  ", line_number++);
            write(STDOUT_FILENO, buf, prefix_bytes);
            write(STDOUT_FILENO, line, read_bytes);
        }
        memset(line, 0, sizeof(line));
    }
    return 0;
}

/* Funções da aula */
int newreadln(int fd, char * buf, int size) {
    int buf_pos = 0;
    while (buf_pos < size && read(fd, buf + buf_pos, 1) > 0) {
        if (buf[buf_pos] == '\n') return buf_pos + 1;
        buf_pos++;
    }
    return buf_pos;
}

int newmynl() {
    int bytes_read = 0;
    char line[20];
    char output_buf[20];
    int line_counter = 1;
    while ((bytes_read = newreadln(STDIN_FILENO, line, 20)) > 0) {
        int output_bytes = snprintf(output_buf, 20, "%d: ", line_counter);
        write(STDOUT_FILENO, output_buf, output_bytes);
        write(STDOUT_FILENO, line, bytes_read);
        line_counter++;
    }
    return 0;
}

/* ./mynl < ex.txt para utilizar em ficheiros */
int main(int argc, char const * argv[]) {
    return mynl();
}
