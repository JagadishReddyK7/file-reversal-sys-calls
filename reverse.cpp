#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define FAILURE -1


int main(int argc, char* argv[]) {
    int flag = atoi(argv[1]);
    cout << flag << endl;
    off_t block_size = atoi(argv[2]);
    cout << block_size << endl;
    int fd_read, fd_write;
    off_t idx;
    struct stat st;
    char buf[block_size];
    ssize_t bytes_read, bytes_write;
    ssize_t total_bytes_written;

    fd_read = open("./read_file.txt", O_RDONLY);
    if (fd_read < 0) {
        perror("open read_file failed");
        exit(FAILURE);
    }

    fd_write = open("write_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_write < 0) {
        perror("open write_file failed");
        close(fd_read);
        exit(FAILURE);
    }

    if (fstat(fd_read, &st) != 0) {
        perror("fstat failed");
        close(fd_read);
        close(fd_write);
        exit(FAILURE);
    }

    if (st.st_size == 0) {
        close(fd_read);
        close(fd_write);
        return 0;
    }

    printf("size of the file is %lld\n", (long long)st.st_size);
    if(flag==0) {
        idx=0;
        for (; idx <= st.st_size; idx+=block_size) {
            if (lseek(fd_read, idx, SEEK_SET) == -1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            bytes_read = read(fd_read, buf, block_size);
            if (bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write = write(fd_write, buf, bytes_read);
                total_bytes_written+=bytes_write;
                cout << "\rProgress: " << total_bytes_written/st.st_size << "%" << endl;
                if (bytes_write < 0) {
                    perror("write failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            } else if (bytes_read < 0) {
                perror("read failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
        }
    }
    else if(flag==1) {
        off_t mod=st.st_size%block_size;
        idx=st.st_size-mod;
        for(; idx>=0;idx-=block_size) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            bytes_read = read(fd_read, buf, block_size);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                if(bytes_write<0) {
                    perror("write failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            } else if (bytes_read < 0) {
                perror("read failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
        }
    }
    else if(flag==2) {
        off_t start_idx=atoi(argv[3]);
        off_t end_idx=atoi(argv[4]);
        cout << start_idx << endl << end_idx << endl;
        off_t mod=start_idx%block_size;
        idx=start_idx-mod;
        for(; idx>=0;idx-=block_size) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            if(start_idx-idx<block_size) bytes_read = read(fd_read, buf, start_idx-idx);
            else bytes_read = read(fd_read, buf, block_size);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                if(bytes_write<0) {
                    perror("write failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            } else if (bytes_read < 0) {
                perror("read failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
        }
        idx=start_idx;
        for(;idx<=end_idx;idx+=block_size) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            if(end_idx-idx<block_size) bytes_read = read(fd_read, buf, end_idx-idx);
            else bytes_read = read(fd_read, buf, block_size);
            if(bytes_read>0) {
                bytes_write=write(fd_write, buf, bytes_read);
                if(bytes_write<0) {
                    perror("write failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            } else if (bytes_read < 0) {
                perror("read failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
        }
        mod=(st.st_size-end_idx)%block_size;
        idx=st.st_size-mod;
        for(;idx>=end_idx;idx-=block_size) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            bytes_read = read(fd_read, buf, block_size);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                if(bytes_write<0) {
                    perror("write failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            } else if (bytes_read < 0) {
                perror("read failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
        }
    }

    close(fd_read);
    close(fd_write);
    return 0;
}