#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define MB 1024

static time_t getFileModifiedTime(const char *path)
{
    struct stat attr;
    if (stat(path, &attr) == 0)
    {
        //printf("%s: last modified time: %s", path, ctime(&attr.st_mtime));
        return attr.st_mtime;
    }
    return 0;
}

void remove_old(char *file)
{
    time_t now;
    struct tm * timeinfo;

    time (&now);
    timeinfo = localtime (&now);
    timeinfo->tm_mday = timeinfo->tm_mday - 7;
    //printf ("1 week ago time and date: %s\n", asctime(timeinfo));

    time_t t1 = mktime ( timeinfo );
    time_t t2 = getFileModifiedTime(file);

    if (t1 > t2){
        remove(file);
    }

}

// scan dir recursive
void sdr(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char cwd[MB];
    char fullpath[MB];

    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
            sdr(entry->d_name,depth+4);
        }else{
            if (getcwd(cwd, sizeof(cwd)) != NULL){
                sprintf(fullpath, "%s/%s", cwd,entry->d_name);
                remove_old(fullpath);
            }
        }
    }
    chdir("..");
    closedir(dp);
}

int main()
{
    sdr("/var/www/TMP",0);
    return 0;
}
