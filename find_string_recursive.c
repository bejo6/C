#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR "\\" 
#else 
#define PATH_SEPARATOR "/" 
#endif 

#define MB 1024


// Function regex
int cr( char* a, const char* b){
  regex_t c;
  int e;
  int f = 0;
  e = regcomp(&c, b, 0);
  if (e) {
    return 0;
  }
  e = regexec(&c, a, 0, NULL, 0);
  if (!e) {
    f = 1;
  }
  regfree(&c);
  return f;
}

// Function find_string
int find_string(char *file, const char* patern){
  FILE *fp;
  char *buf;  
  size_t sz;
  int match = 0;
  
  if ((fp = fopen(file, "r")) == NULL){
      return 0;
  }
  
  fseek(fp, 0, SEEK_END);
  sz = ftell(fp);
  buf = malloc(sz + 1);
  rewind(fp);

  while (fgets(buf, sz, fp) != NULL)
  {
     match = cr(buf,patern);
     if( match ){
         break;
     }
     
  }
  
  fclose(fp);
  
  return match;
}


int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}


void listFilesRecursively(char *basePath, char *strVal, char *extVal)
{
    char path[1000];
    char *ext;
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, PATH_SEPARATOR);
            strcat(path, dp->d_name);

            if(is_regular_file(path)) {
                if( extVal ){
                    ext = strrchr(path, '.');
                    if (ext) {
                       if(!strcmp(ext+1, extVal)){
                            if(find_string(path, strVal)){
                                printf("[+] FILE: %s\n", path);
                            }
                       }
                    }
                }else{
                    if(find_string(path, strVal)){
                        printf("[+] FILE: %s\n", path);
                    }
                }
            }

            listFilesRecursively(path, strVal, extVal);
        }
    }

    closedir(dir);
}

int isprint(int c);

int main (int argc, char **argv)
{
  int index;
  int c;

  char *dir_value    = NULL;
  char *string_value = NULL;
  char *ext_value    = NULL;
  char base[MB];

  char cwd[MB];



  opterr = 0;

  while ((c = getopt (argc, argv, "d:s:e:")) != -1)
    switch (c)
      {
      case 'd':
        dir_value = optarg;
        break;
      case 's':
        string_value = optarg;
        break;
      case 'e':
        ext_value = optarg;
        break;
      case '?':
        if (optopt == 'd'){
          fprintf (stderr, "[*] Option -%c requires an argument.\n", optopt);
          fprintf (stderr, "[*] Ex: -%c C:\\myfolder\n", optopt);
        } else if (optopt == 's'){
          fprintf (stderr, "[*] Option -%c requires an argument.\n", optopt);
          fprintf (stderr, "[*] Ex: -%c \"some string\"", optopt);
        }else if (optopt == 'e'){
          fprintf (stderr, "[*] Option -%c requires an argument.\n", optopt);
          fprintf (stderr, "[*] Ex: -%c txt", optopt);
        }else if (isprint (optopt)){
          fprintf (stderr, "[-] Unknown option -%c.\n", optopt);
        }else{
          fprintf (stderr, "[-] Unknown option character `\\x%x'.\n", optopt);
        }

        return 1;
      default:
        abort ();
      }

  if(!dir_value){
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       sprintf(base, "%s", cwd);
    }
  }else{
      sprintf(base, "%s", dir_value);
  }
  
  if( base && string_value){
      printf ("[*] Directory: %s\n", base);
      printf ("[*] Find for string: %s\n", string_value);

      if(ext_value){
          printf ("[*] File extension: %s\n", ext_value);
      }
      
      listFilesRecursively(base, string_value, ext_value);
  }



  for (index = optind; index < argc; index++){
    printf ("[-] Non-option argument %s\n", argv[index]);
  }

  return 0;
}
