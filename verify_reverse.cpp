#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
using namespace std;

#define FAILURE 1
#define CHUNK_SIZE 4096

int main(int argc, char* argv[]) {
    const char* old_file = argv[1];
    const char* new_file = argv[2];
    const char* directory = argv[3];
    int flag=atoi(argv[4]);
    off_t buf_size = CHUNK_SIZE;
    off_t block_size, start_idx, end_idx;
    if(argc==6) {
        block_size = atoi(argv[5]);
        buf_size = block_size;
    }
    else if(argc==7) {
        start_idx=atoi(argv[5]);
        end_idx=atoi(argv[6]);
    }
    char buf1[buf_size];
    char buf2[buf_size];
    struct stat st1, st2;
    ssize_t bytes_read_1, bytes_read_2;
    string content="No";
    string same_size="Yes";


    int old_fd = open(old_file, O_RDONLY);
    if (old_fd < 0) {
        perror("open old_file failed");
        exit(FAILURE);
    }
    int new_fd = open(new_file, O_RDONLY);
    if (new_fd < 0) {
        perror("open new_file failed");
        close(old_fd);
        exit(FAILURE);
    }
    if (fstat(old_fd, &st1) != 0) {
        perror("fstat failed");
        close(old_fd);
        close(new_fd);
        exit(FAILURE);
    }
    if (st1.st_size == 0) {
        close(old_fd);
        close(new_fd);
        return 0;
    }
    if (fstat(new_fd, &st2) != 0) {
        perror("fstat failed");
        close(old_fd);
        close(new_fd);
        exit(FAILURE);
    }
    if (st2.st_size == 0) {
        close(old_fd);
        close(new_fd);
        return 0;
    }
    if(st1.st_size!=st2.st_size) {
        content="No";
        same_size="No";
    }
    else if(flag==1) {
        off_t mod=st1.st_size%buf_size;
        cout << mod << endl;
        off_t idx1=0, idx2=st2.st_size-mod;
        for(;idx1<st1.st_size;idx1+=buf_size) {
            if (lseek(old_fd, idx1, SEEK_SET) == -1) {
                perror("lseek1 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if (lseek(new_fd, idx2, SEEK_SET) == -1) {
                perror("lseek2 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if(idx1!=0) mod=buf_size;
            bytes_read_1 = read(old_fd, buf1, mod);
            bytes_read_2 = read(new_fd, buf2, mod);
            cout << buf1[mod-1] << buf2[mod-1] << endl;
            if (bytes_read_1>0) {
                int i;
                for(i=0;i<bytes_read_1;i++) {
                    if(buf1[i]!=buf2[bytes_read_1-i-1]) {
                        break;
                    }
                }
                if(i!=bytes_read_1) break;
            } else if (bytes_read_1 < 0) {
                perror("read failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            idx2=st2.st_size-mod-idx1;
        }
        if(idx1<st1.st_size) content="No";
        else content = "YES";
    }
    else if(flag==0) {
        off_t idx1=0, idx2=0;
        for(; idx1<st1.st_size;idx1+=block_size) {
            idx2=idx1;
            if (lseek(old_fd, idx1, SEEK_SET) == -1) {
                perror("lseek1 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if (lseek(new_fd, idx2, SEEK_SET) == -1) {
                perror("lseek2 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            bytes_read_1 = read(old_fd, buf1, block_size);
            bytes_read_2 = read(new_fd, buf2, block_size);
            if (bytes_read_1>0) {
                int i;
                for(i=0;i<bytes_read_1;i++) {
                    if(buf1[i]!=buf2[bytes_read_1-i-1]) {
                        break;
                    }
                }
                if(i!=bytes_read_1) break;
            } else if (bytes_read_1 < 0) {
                perror("read failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
        }
        if(idx1<st1.st_size) content="No";
        else content = "YES";
    }
    else if(flag==2) {
        off_t mod=start_idx%buf_size;
        cout << mod << endl;
        off_t idx1=0, idx2=start_idx-mod;
        for(;idx1<start_idx;idx1+=buf_size) {
            if (lseek(old_fd, idx1, SEEK_SET) == -1) {
                perror("lseek1 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if (lseek(new_fd, idx2, SEEK_SET) == -1) {
                perror("lseek2 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if(idx1!=0) mod=buf_size;
            bytes_read_1 = read(old_fd, buf1, mod);
            bytes_read_2 = read(new_fd, buf2, mod);
            cout << buf1[mod-1] << "11" << buf2[mod-1] << endl;
            if (bytes_read_1>0) {
                int i;
                for(i=0;i<bytes_read_1;i++) {
                    if(buf1[i]!=buf2[bytes_read_1-i-1]) {
                        break;
                    }
                }
                if(i!=bytes_read_1) break;
            } else if (bytes_read_1 < 0) {
                perror("read failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            idx2=st2.st_size-mod-idx1;
        }
        mod=(st2.st_size-end_idx)%buf_size;
        idx1=end_idx;
        idx2=st2.st_size-mod;
        for(;idx1<st1.st_size;idx1+=buf_size) {
            if (lseek(old_fd, idx1, SEEK_SET) == -1) {
                perror("lseek1 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if (lseek(new_fd, idx2, SEEK_SET) == -1) {
                perror("lseek2 failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            if(idx1!=end_idx) mod=buf_size;
            bytes_read_1 = read(old_fd, buf1, mod);
            bytes_read_2 = read(new_fd, buf2, mod);
            cout << buf1[mod-1] << "22" << buf2[mod-1] << endl;
            if (bytes_read_1>0) {
                int i;
                for(i=0;i<bytes_read_1;i++) {
                    if(buf1[i]!=buf2[bytes_read_1-i-1]) {
                        break;
                    }
                }
                if(i!=bytes_read_1) break;
            } else if (bytes_read_1 < 0) {
                perror("read failed");
                close(old_fd);
                close(new_fd);
                exit(FAILURE);
            }
            idx2=st2.st_size-mod-idx1;
        }
        if(idx1<st1.st_size) content="No";
        else content = "Yes";
    }

    cout << "Directory is created: Yes" << endl;
    cout << "Whether file contents are correctly processed: " << content << endl;
    cout << "Both Files sizes are same: " << same_size << endl;

    cout << "User has read permission on oldfile: " << ((st1.st_mode & S_IRUSR) ? "Yes" : "No") << endl;
    cout << "User has write permission on oldfile: " << ((st1.st_mode & S_IWUSR) ? "Yes" : "No") << endl;
    cout << "User has execute permission on oldfile: " << ((st1.st_mode & S_IXUSR) ? "Yes" : "No") << endl;
    cout << "Group has read permission on oldfile: " << ((st1.st_mode & S_IRGRP) ? "Yes" : "No") << endl;
    cout << "Group has write permission on oldfile: " << ((st1.st_mode & S_IWGRP) ? "Yes" : "No") << endl;
    cout << "Group has execute permission on oldfile: " << ((st1.st_mode & S_IXGRP) ? "Yes" : "No") << endl;
    cout << "Others has read permission on oldfile: " << ((st1.st_mode & S_IROTH) ? "Yes" : "No") << endl;
    cout << "Others has write permission on oldfile: " << ((st1.st_mode & S_IWOTH) ? "Yes" : "No") << endl;
    cout << "Others has execute permission on oldfile: " << ((st1.st_mode & S_IXOTH) ? "Yes" : "No") << endl;

    cout << "User has read permission on newfile: " << ((st2.st_mode & S_IRUSR) ? "Yes" : "No") << endl;
    cout << "User has write permission on newfile: " << ((st2.st_mode & S_IWUSR) ? "Yes" : "No") << endl;
    cout << "User has execute permission on newfile: " << ((st2.st_mode & S_IXUSR) ? "Yes" : "No") << endl;
    cout << "Group has read permission on newfile: " << ((st2.st_mode & S_IRGRP) ? "Yes" : "No") << endl;
    cout << "Group has write permission on newfile: " << ((st2.st_mode & S_IWGRP) ? "Yes" : "No") << endl;
    cout << "Group has execute permission on newfile: " << ((st2.st_mode & S_IXGRP) ? "Yes" : "No") << endl;
    cout << "Others has read permission on newfile: " << ((st2.st_mode & S_IROTH) ? "Yes" : "No") << endl;
    cout << "Others has write permission on newfile: " << ((st2.st_mode & S_IWOTH) ? "Yes" : "No") << endl;
    cout << "Others has execute permission on newfile: " << ((st2.st_mode & S_IXOTH) ? "Yes" : "No") << endl;

    struct stat dir_st;
    
    if(stat(directory, &dir_st)==-1) {
        perror("directory stat failed");
        exit(FAILURE);
    }

    cout << "User has read permission on directory: " << ((dir_st.st_mode & S_IRUSR) ? "Yes" : "No") << endl;
    cout << "User has write permission on directory: " << ((dir_st.st_mode & S_IWUSR) ? "Yes" : "No") << endl;
    cout << "User has execute permission on directory: " << ((dir_st.st_mode & S_IXUSR) ? "Yes" : "No") << endl;
    cout << "Group has read permission on directory: " << ((dir_st.st_mode & S_IRGRP) ? "Yes" : "No") << endl;
    cout << "Group has write permission on directory: " << ((dir_st.st_mode & S_IWGRP) ? "Yes" : "No") << endl;
    cout << "Group has execute permission on directory: " << ((dir_st.st_mode & S_IXGRP) ? "Yes" : "No") << endl;
    cout << "Others has read permission on directory: " << ((dir_st.st_mode & S_IROTH) ? "Yes" : "No") << endl;
    cout << "Others has write permission on directory: " << ((dir_st.st_mode & S_IWOTH) ? "Yes" : "No") << endl;
    cout << "Others has execute permission on directory: " << ((dir_st.st_mode & S_IXOTH) ? "Yes" : "No") << endl;
    
    close(old_fd);
    close(new_fd);
    return 0;
}