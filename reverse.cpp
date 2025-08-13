#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
using namespace std;

#define FAILURE 1
#define CHUNK_SIZE 10


int main(int argc, char* argv[]) {
    const char* input_file = argv[1];
    cout << input_file << endl; 
    int flag = atoi(argv[2]);
    cout << flag << endl;
    off_t block_size = atoi(argv[3]);
    cout << block_size << endl;
    cout << CHUNK_SIZE << endl;
    int fd_read, fd_write;
    off_t idx;
    struct stat status;
    off_t buf_size=CHUNK_SIZE;
    if(CHUNK_SIZE<block_size) buf_size=block_size;
    char buf[buf_size];
    ssize_t bytes_read, bytes_write;
    ssize_t total_bytes_written=0;

    fd_read = open(input_file, O_RDONLY);
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

    if (fstat(fd_read, &status) != 0) {
        perror("fstat failed");
        close(fd_read);
        close(fd_write);
        exit(FAILURE);
    }

    if (status.st_size == 0) {
        close(fd_read);
        close(fd_write);
        return 0;
    }
    cout << "Size of the file is: " << status.st_size << endl;

    if(flag==0) {
        if(CHUNK_SIZE<block_size) {
            idx=0;
            for (; idx <= status.st_size; idx+=block_size) {
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
                    if (bytes_write < 0) {
                        perror("write failed");
                        close(fd_read);
                        close(fd_write);
                        exit(FAILURE);
                    }
                    total_bytes_written+=bytes_write;
                    double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                    cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
                } else if (bytes_read < 0) {
                    perror("read failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            }
        }
        else if(CHUNK_SIZE>=block_size) {
            idx=0;
            off_t mod=CHUNK_SIZE%block_size;
            while(idx<status.st_size) {
                off_t bytes_to_read = CHUNK_SIZE-mod;
                if(status.st_size-idx<block_size) {
                    bytes_to_read=status.st_size-idx;
                }
                
                if (lseek(fd_read, idx, SEEK_SET) == -1) {
                    perror("lseek failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
                bytes_read = read(fd_read, buf, bytes_to_read);
                if (bytes_read>0) {
                    for(off_t i=0;i<bytes_to_read;i+=block_size) {
                        off_t end=i+block_size;
                        if(bytes_read<block_size) end=i+bytes_read; 
                        for(off_t j=i;j<(i+end)/2;j++) {
                            char temp=buf[j];
                            buf[j]=buf[i+end-j-1];
                            buf[i+end-j-1]=temp;
                        }
                    }
                    bytes_write = write(fd_write, buf, bytes_read);
                    if (bytes_write < 0) {
                        perror("write failed");
                        close(fd_read);
                        close(fd_write);
                        exit(FAILURE);
                    }
                    total_bytes_written+=bytes_write;
                    double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                    cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
                    idx=total_bytes_written;
                } else if (bytes_read < 0) {
                    perror("read failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
            }
        }

        // idx=0;
        // for (; idx <= status.st_size; idx+=CHUNK_SIZE) {
        //     if (lseek(fd_read, idx, SEEK_SET) == -1) {
        //         perror("lseek failed");
        //         close(fd_read);
        //         close(fd_write);
        //         exit(FAILURE);
        //     }
        //     bytes_read = read(fd_read, buf, CHUNK_SIZE);
        //     if (bytes_read>0) {
        //         for(int i=0;i<bytes_read/2;i++) {
        //             char temp=buf[i];
        //             buf[i]=buf[bytes_read-i-1];
        //             buf[bytes_read-i-1]=temp;
        //         }
        //         bytes_write = write(fd_write, buf, bytes_read);
        //         total_bytes_written+=bytes_write;
        //         double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
        //         cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
        //         if (bytes_write < 0) {
        //             perror("write failed");
        //             close(fd_read);
        //             close(fd_write);
        //             exit(FAILURE);
        //         }
        //     } else if (bytes_read < 0) {
        //         perror("read failed");
        //         close(fd_read);
        //         close(fd_write);
        //         exit(FAILURE);
        //     }
        // }

    }
    else if(flag==1) {
        off_t mod=status.st_size%CHUNK_SIZE;
        idx=status.st_size-mod;
        for(; idx>=0;idx-=CHUNK_SIZE) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            bytes_read = read(fd_read, buf, CHUNK_SIZE);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                total_bytes_written+=bytes_write;
                double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
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
        cout << endl;
    }
    else if(flag==2) {
        off_t start_idx=atoi(argv[4]);
        off_t end_idx=atoi(argv[5]);
        cout << start_idx << endl << end_idx << endl;
        off_t mod=start_idx%CHUNK_SIZE;
        idx=start_idx-mod;
        for(; idx>=0;idx-=CHUNK_SIZE) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            if(start_idx-idx<CHUNK_SIZE) bytes_read = read(fd_read, buf, start_idx-idx);
            else bytes_read = read(fd_read, buf, CHUNK_SIZE);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                total_bytes_written+=bytes_write;
                double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
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
        for(;idx<=end_idx;idx+=CHUNK_SIZE) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            if(end_idx-idx<CHUNK_SIZE) bytes_read = read(fd_read, buf, end_idx-idx);
            else bytes_read = read(fd_read, buf, CHUNK_SIZE);
            if(bytes_read>0) {
                bytes_write=write(fd_write, buf, bytes_read);
                total_bytes_written+=bytes_write;
                double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
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
        mod=(status.st_size-end_idx)%CHUNK_SIZE;
        idx=status.st_size-mod;
        for(;idx>=end_idx;idx-=CHUNK_SIZE) {
            if(lseek(fd_read, idx, SEEK_SET)==-1) {
                perror("lseek failed");
                close(fd_read);
                close(fd_write);
                exit(FAILURE);
            }
            bytes_read = read(fd_read, buf, CHUNK_SIZE);
            if(bytes_read>0) {
                for(int i=0;i<bytes_read/2;i++) {
                    char temp=buf[i];
                    buf[i]=buf[bytes_read-i-1];
                    buf[bytes_read-i-1]=temp;
                }
                bytes_write=write(fd_write, buf, bytes_read);
                total_bytes_written+=bytes_write;
                double progress = (static_cast<double>(total_bytes_written) / status.st_size) * 100.0;
                cout << "\rProgress: " << fixed << setprecision(1) << progress << "%" << flush;
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
    cout << endl;
    close(fd_read);
    close(fd_write);
    return 0;
}