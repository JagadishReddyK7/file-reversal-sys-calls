#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <iomanip>
using namespace std;

#define FAILURE 1
#define CHUNK_SIZE 524288


int main(int argc, char* argv[]) {
    const char* input_file = argv[1];
    cout << input_file << endl; 
    int flag = atoi(argv[2]);
    cout << flag << endl;
    cout << CHUNK_SIZE << endl;
    off_t buf_size=CHUNK_SIZE;
    off_t block_size=CHUNK_SIZE;
    if(flag==0) {
        block_size = atoi(argv[3]);
        cout << block_size << endl;
        if(CHUNK_SIZE<block_size) buf_size=block_size;
    }
    char buf[buf_size];
    int fd_read, fd_write;
    off_t idx;
    struct stat status;
    ssize_t bytes_read, bytes_write;
    ssize_t total_bytes_written=0;
    const char* dir_name = "Assignment1";

    if(block_size<=0) {
        perror("Invalid block size");
        exit(FAILURE);
        return 1;
    }

    if(mkdir(dir_name,0700)==-1) {
        if(errno!=EEXIST) {
            perror("Error creating directory");
            exit(FAILURE);
        }
    }

    const char* input_filename;
    const char* last_slash=strrchr(input_file,'/');
    input_filename=last_slash+1;
    cout << input_filename << endl;

    char output_path[256];
    int out_length = snprintf(output_path, sizeof(output_path), "%s/%d_%s", dir_name, flag, input_filename);

    fd_read = open(input_file, O_RDONLY, 0644);
    if (fd_read < 0) {
        perror("open read_file failed");
        exit(FAILURE);
    }

    fd_write = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
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
        return 1;
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
            cout << endl << "Block-wise reversal of the given file is successful and was written to " << output_path << " with block size " << block_size << endl;
        }
        else if(CHUNK_SIZE>=block_size) {
            idx=0;
            off_t mod=CHUNK_SIZE%block_size;
            while(idx<status.st_size) {
                off_t bytes_to_read = CHUNK_SIZE-mod;
                if(CHUNK_SIZE>=status.st_size) bytes_to_read=status.st_size;
                if(status.st_size-idx<block_size) {
                    bytes_to_read=status.st_size-idx;
                    cout << bytes_to_read << endl;
                }
                cout << bytes_to_read << endl;

                if (lseek(fd_read, idx, SEEK_SET) == -1) {
                    perror("lseek failed");
                    close(fd_read);
                    close(fd_write);
                    exit(FAILURE);
                }
                bytes_read = read(fd_read, buf, bytes_to_read);
                cout<< buf[status.st_size-1] << endl;
                if (bytes_read>0) {
                    for(off_t i=0;i<bytes_to_read;i+=block_size) {
                        off_t end=i+block_size;
                        if(bytes_read<block_size) end=i+bytes_read;
                        if(end>=bytes_read) end=bytes_read;
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
            cout << endl << "Block-wise reversal of the given file is successful and was written to " << output_path << " with block size " << block_size << endl;
        }

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
        cout << endl << "Full file reversal of the given input file is successful and is written to " << output_path << endl;
    }
    else if(flag==2) {
        off_t start_idx=atoi(argv[3]);
        off_t end_idx=atoi(argv[4]);
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
        cout << endl << "Partial range reversal of the given input file is successful and is written to " << output_path << " with range between " << start_idx << " and " << end_idx << endl;
    }
    close(fd_read);
    close(fd_write);
    return 0;
}