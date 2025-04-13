extern int PCclose(int fd);
extern int PCinit(void);
extern int PClseek(int fd, int offset, int mode);
extern int PCopen(char* name, int flags, int perms);
extern int PCread(int fd, char* buff, int len);

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2