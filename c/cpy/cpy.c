
/**
 * TODO: 1. Добавить копирование директорий (методом создания новой в цеоевой с прежним названием)
 *       2. 
 */



// #define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 1024 * 64 // Размер буфера копирования

char *bar;
struct winsize ws;
size_t effective_term_width;

typedef enum file_cp_type_s {
  SRC_FILE,
  SRC_DIR,
  DST_FILE,
  DST_DIR,
} file_cp_type_t;

struct path_cp_info {
  file_cp_type_t f_cp_type_src;
  file_cp_type_t f_cp_type_dst;
  int files_count;
  char *path_src;
  char *path_dst;
  // struct dirent*** entries;
  char **path_src_list;
  char **path_dst_list;
};

int filter(const struct dirent *entry) {
  if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
    return 0;
  }
  return 1;
}

int get_file_list(char *dir_name, struct dirent **namelist) {
  // struct dirent** namelist = NULL;
  int n = scandir(dir_name, &namelist, filter, alphasort);
  if (n == -1) {
    fprintf(stderr, "opendir: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return n;
}

void get_file_cp_info(char *path_src, char *path_dst,
                      struct path_cp_info *p_info) {

  struct stat sb;

  // Путь SRC файла или директории
  if (lstat(path_src, &sb) == -1) {
    fprintf(stderr, "stat read: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // SRC это директория, значик копируем все файлы
  if (S_ISDIR(sb.st_mode)) {

    p_info->f_cp_type_src = SRC_DIR;

    // struct dirent *ents;
    // get_file_list(path_src, &ents);
    struct dirent **ents;
    int n = scandir(path_src, &ents, filter, alphasort);
    if (n == -1) {
      fprintf(stderr, "opendir: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    p_info->files_count = n;
    p_info->files_count = n;
    p_info->path_dst_list = calloc(n, sizeof(char *));
    p_info->path_src_list = calloc(n, sizeof(char *));

    // Идем по циклю и копируем все файлы в директорию
    for (int i = 0; i < n; ++i) {
      size_t len_path = strlen(path_src);

      size_t len_file_name = strlen(ents[i]->d_name);
      char *src = calloc(len_path + len_file_name + 1, sizeof(char));
      sprintf(src, "%s%s", path_src, ents[i]->d_name);

      char *dst = NULL;
      size_t len_dst = strlen(path_dst);
      int res = strcmp(&(path_dst[len_dst - 1]), "/");
      dst = calloc(len_dst + len_file_name + 3, sizeof(char));
      if (res != 0) {
        sprintf(dst, "%s/%s", path_dst, ents[i]->d_name);
      } else {
        sprintf(dst, "%s%s", path_dst, ents[i]->d_name);
      }

      p_info->path_src_list[i] = calloc(strlen(src) + 1, sizeof(char));
      strcpy(p_info->path_src_list[i], src);
      p_info->path_dst_list[i] = calloc(strlen(dst) + 1, sizeof(char));
      strcpy(p_info->path_dst_list[i], dst);

      free(dst);
      free(src);
    }

    for (int i = 0; i < n; ++i) {
      free(ents[i]);
    }
    free(ents);
  }

  // SRC это файл
  else if (S_ISREG(sb.st_mode)) {

    p_info->f_cp_type_src = SRC_FILE;
    p_info->path_src = path_src;

    // DST
    // Проверяем, есть ли файл с именем, в которое хотим скопировать,
    // если нет, то создаем пустой файл
    if (access(path_dst, F_OK) == -1) {
      open(path_dst, O_RDWR | O_CREAT | O_TRUNC,
           S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
    }
    if (lstat(path_dst, &sb) == -1) {
      fprintf(stderr, "stat read: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    char *dst = NULL;

    if (S_ISREG(sb.st_mode)) {
      dst = path_dst;
    }

    if (S_ISDIR(sb.st_mode)) {
      char *f_name = strrchr(path_src, '/');

      size_t len_f_name = strlen(f_name + 1);
      size_t len_dst = strlen(path_dst);
      int res = strcmp(&(path_dst[len_dst - 1]), "/");
      dst = calloc(len_dst + len_f_name + 2, sizeof(char));
      if (res != 0) {
        path_dst[len_dst - 2] = '\0';
        sprintf(dst, "%s/%s", path_dst, f_name);
      } else {
        sprintf(dst, "%s%s", path_dst, f_name);
      }
    }

    p_info->f_cp_type_dst = DST_FILE;
    p_info->path_dst = dst;
  }
}

// Вывод статуса операции копирования в процентах
void print_copy_status_in_proc(size_t read_file_size, ssize_t bytes_readed,
                               char *read_file_path, char *write_file_path) {

  // Расситываем кол-во байт на процент
  size_t bytes_per_proc = read_file_size / 100;
  // Выводим индикатор копирования в процентаах
  printf("\e[?25l"); // Скрыть курсор
  fflush(stdout);
  // Если размер файла меньше или равен размеру буфера, то
  // файл скопируеися за один цикл, сразу устанавливаем значение в 100%,
  // иначе рассчитываем
  printf("Copy %s to %s: [%zu %%]\r", read_file_path, write_file_path,
         read_file_size <= BUF_SIZE ? 100 : bytes_readed / bytes_per_proc);
  printf("\e[?25h"); // Отобразить курсор
  fflush(stdout);
}

// Вывод статуса операции копирования в виде бара
void print_copy_status_bar(size_t read_file_size, ssize_t bytes_readed,
                           /*int effective_term_width,*/
                           [[maybe_unused]] char *read_file_path,
                           [[maybe_unused]] char *write_file_path) {

  // Расситываем кол-во байт на процент
  size_t bytes_per_proc = read_file_size / 100;
  // Расситываем кол-во байт на 10 процентов
  size_t bytes_per_10_proc = bytes_per_proc * 10;
  // Рассчитываем кол-во заполненных символов (кол-во символов на каждые 10
  // процентов)
  size_t active_bar_symbols_per_10_proc = effective_term_width / 10;
  // Если кол-во байт в файле меньше, чем байт на процент, то заполняем сразу
  // все ширину полосы бара
  size_t active_bar_symbols =
      (bytes_per_10_proc != 0)
          ? (bytes_readed / bytes_per_10_proc * active_bar_symbols_per_10_proc)
          : (effective_term_width);

  for (size_t i = 0; i < active_bar_symbols; ++i) {
    bar[i] = '|';
    bar[effective_term_width - 1] = '\0';
  }
  for (size_t j = active_bar_symbols; j < sizeof(bar) - 1; ++j) {
    bar[j] = ' ';
  }

  printf("\e[?24l"); // Скрыть курсор
  fflush(stdout);

  // Если размер файла меньше или равен размеру буфера, то
  // файл скопируеися за один цикл, сразу устанавливаем значение в 100%,
  // иначе рассчитываем
  printf("copy: [\033\[32m%s\033\[0m] %zu%%\r", bar,
         read_file_size <= BUF_SIZE ? 100 : bytes_readed / bytes_per_proc);

  printf("\e[?25h"); // Отобразить курсор
  fflush(stdout);
}

void handler([[maybe_unused]] int sig, [[maybe_unused]] siginfo_t *info,
             [[maybe_unused]] void *uc) {
  free(bar);
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  int term_width = ws.ws_col;
  // Вычитаем кол-во символов "copy: [] 100%"
  effective_term_width = term_width - 13;
  // Выделяем память для бара
  bar = malloc(effective_term_width);
  memset(bar, ' ', effective_term_width);
  bar[effective_term_width - 1] = '\0';
}

int copy_file_(char *read_file_path, char *write_file_path,
               void (*print_status)(size_t, ssize_t, char *, char *)) {
  int rfd = open(read_file_path, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
  if (rfd == -1) {
    fprintf(stderr, "open read: %s\n", strerror(errno));
    return 1; // EXIT_FAILURE;
  }
  int wfd = open(write_file_path, O_RDWR | O_CREAT | O_TRUNC,
                 S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
  if (wfd == -1) {
    fprintf(stderr, "open write: %s\n", strerror(errno));
    return 2; // EXIT_FAILURE;
  }
  // Получаем размер файла
  struct stat sb;
  if (fstat(rfd, &sb) == -1) {
    fprintf(stderr, "stat read: %s\n", strerror(errno));
    return 3; // EXIT_FAILURE;
  }
  ssize_t read_file_size = sb.st_size;

  // Цикл чтения записи
  char buf[BUF_SIZE];
  for (ssize_t bytes_readed = 0; bytes_readed < read_file_size;) {
    ssize_t readed = read(rfd, buf, sizeof(buf));
    if (readed == -1) {
      fprintf(stderr, "read file: %s\n", strerror(errno));
      return 4; // EXIT_FAILURE;
    }
    bytes_readed += readed;

    ssize_t written = write(wfd, buf, readed);
    if (written == -1) {
      fprintf(stderr, "write file: %s\n", strerror(errno));
      return 5; // EXIT_FAILURE;
    }

    print_status(read_file_size, bytes_readed, NULL, NULL);
  }
  printf("\n");
  close(rfd);
  close(wfd);
  return 0;
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Необходимо ввести 2 аргумента");
    return EXIT_FAILURE;
  }

  struct sigaction sa;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;

  if (sigaction(SIGWINCH, &sa, NULL) == -1) {
    fprintf(stderr, "sigaction: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  //  Определяем ширину терминала
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  int term_width = ws.ws_col;
  // Вычитаем кол-во символов "copy: [] 100%"
  effective_term_width = term_width - 13;
  // Выделяем память для бара
  bar = malloc(effective_term_width);
  memset(bar, ' ', effective_term_width);
  bar[effective_term_width - 1] = '\0';

  struct path_cp_info p_info = {0};
  get_file_cp_info(argv[1], argv[2], &p_info);

  if (p_info.f_cp_type_src == SRC_FILE) {
    if (p_info.f_cp_type_dst == DST_FILE) {
      copy_file_(p_info.path_src, p_info.path_dst, print_copy_status_bar);
    } else if (p_info.f_cp_type_dst == DST_DIR) {
      copy_file_(p_info.path_src, p_info.path_dst, print_copy_status_bar);
    }
  } else if (p_info.f_cp_type_src == SRC_DIR) {
    for (int i = 0; i < p_info.files_count; ++i) {
      copy_file_(p_info.path_src_list[i], p_info.path_dst_list[i],
                 print_copy_status_bar);
    }
  }

  // Освобождаем память
  for (int i = 0; i < p_info.files_count; ++i) {
      free(p_info.path_src_list[i]);
      free(p_info.path_dst_list[i]);
  }
  free(p_info.path_src_list);
  free(p_info.path_dst_list);

  if (bar != NULL)
    free(bar);

  return EXIT_SUCCESS;
}
