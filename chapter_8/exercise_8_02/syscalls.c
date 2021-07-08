#include <fcntl.h>
#include "syscalls.h"

#define PERMISSIONS 0666 // RW for owners, group, others

FILE _io_buffer[MAX_NR_OF_OPEN_FILES] = {
    {0, (char *)0, (char *)0, _READ, 0},          // stdin
    {0, (char *)0, (char *)0, _WRITE, 1},         // stdout
    {0, (char *)0, (char *)0, _WRITE | _UNBUF, 2} // stderr
};

void *malloc(long unsigned int size);
long int lseek(int file_descriptor, long int offset, int whence);
long int read(int file_descriptor, void *buffer, long unsigned int nr_of_bytes);
long int write(int file_descriptor, void *buffer, long unsigned int nr_of_bytes);

int _fill_buffer(FILE *file_p)
{
  int buffer_size;

  if ((file_p->flag & (_READ | _EOF | _ERR)) != _READ)
  {
    return EOF;
  }

  buffer_size = (file_p->flag & _UNBUF) ? 1 : BUFFER_SIZE;

  if (file_p->base == NULL)
  {
    if ((file_p->base = (char *)malloc(buffer_size)) == NULL)
    {
      return EOF;
    }
  }

  file_p->next_char_pos_p = file_p->base;
  file_p->counter = read(file_p->file_descriptor, file_p->next_char_pos_p, buffer_size);

  if (--file_p->counter < 0)
  {
    if (file_p->counter == -1)
    {
      file_p->flag |= _EOF;
    }
    else
    {
      file_p->flag |= _ERR;
    }

    file_p->counter = 0;
    return EOF;
  }

  return (unsigned char)*file_p->next_char_pos_p++;
}

FILE *file_open(char *name, char *mode)
{
  int file_descriptor;
  FILE *file_p;

  if (*mode != 'r' && *mode != 'w' && *mode != 'a')
  {
    return NULL;
  }

  for (file_p = _io_buffer; file_p < _io_buffer + MAX_NR_OF_OPEN_FILES; ++file_p)
  {
    if ((file_p->flag & (_READ | _WRITE)) == 0)
    {
      break; // found free slot
    }
  }

  if (file_p >= _io_buffer + MAX_NR_OF_OPEN_FILES)
  {
    return NULL; // no free slots
  }

  if (*mode == 'w')
  {
    file_descriptor = creat(name, PERMISSIONS);
  }
  else if (*mode == 'a')
  {
    if ((file_descriptor = open(name, O_WRONLY, 0)) == -1)
    {
      file_descriptor = creat(name, PERMISSIONS);
    }
    lseek(file_descriptor, 0L, 2);
  }
  else
  {
    file_descriptor = open(name, O_RDONLY, 0);
  }

  if (file_descriptor == -1)
  {
    return NULL; // couldn't access name
  }

  file_p->file_descriptor = file_descriptor;
  file_p->counter = 0;
  file_p->base = NULL;
  file_p->flag = (*mode == 'r') ? _READ : _WRITE;

  return file_p;
}

int main(void)
{
  FILE *file_p;

  if ((file_p = file_open("syscalls.c", "r")) == NULL)
  {
    return 1;
  }
  else
  {
    char c;
    while ((c = getc(file_p)) != EOF)
    {
      write(1, &c, 1);
    }
  }

  return 0;
}
