/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Utils
 *
 * $Id: utils.c 11242 2006-09-25 18:48:33Z ska $
 *
 ********************************************************************/

#include <utils.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



/* Get the screen size, from 2dbackground.c */
int get_screen_size(int32_t *p_w, int32_t *p_h)
{
	return -1;
}


/* Bresenhams algorithm, from http://www.fact-index.com/b/br/bresenham_s_line_algorithm_c_code.html */
int bresenham(int x0, int y0, int x1, int y1,
	      int (*callback)(int x, int y, void *p), void *p_callback_arg)
{
  int steep = 1;
  int sx, sy;  /* step positive or negative (1 or -1) */
  int dx, dy;  /* delta (difference in X and Y between points) */
  int e;
  int i;

  /*
   * inline swap. On some architectures, the XOR trick may be faster
   */
  int tmpswap;
#define SWAP(a,b) tmpswap = a; a = b; b = tmpswap;

  /*
   * optimize for vertical and horizontal lines here
   */
  dx = abs(x1 - x0);
  sx = ((x1 - x0) > 0) ? 1 : -1;
  dy = abs(y1 - y0);
  sy = ((y1 - y0) > 0) ? 1 : -1;
  if (dy > dx)
    {
      steep = 0;
      SWAP(x0, y0);
      SWAP(dx, dy);
      SWAP(sx, sy);
    }
  e = (dy << 1) - dx;
  for (i = 0; i < dx; i++)
    {
      if (steep)
	{
	  if (callback(x0,y0, p_callback_arg))
	    return 1;
	}
      else
	{
	  if (callback(y0,x0, p_callback_arg))
	    return 1;
	}
      while (e >= 0)
	{
	  y0 += sy;
	  e -= (dx << 1);
	}
      x0 += sx;
      e += (dy << 1);
    }
#undef SWAP

  return 0;
}

void *get_resource(const char *filename, size_t *out_sz)
{
  uint8_t *p_out;
  FILE *fd;
  int n_read;

  size_t file_sz = 0;
  struct stat st;

  if (lstat(filename, &st) < 0)
  {
	  error_msg("Can't lstat %s\n", filename);
	  return NULL;
  }
  file_sz = st.st_size;
  *out_sz = file_sz;

  if ( !(p_out = (uint8_t*)malloc(file_sz)) )
    {
      debug_msg("Alloc space failed\n");
      return NULL;
    }

  if ( (fd = fopen((const char*)filename, "r" )) < 0)
    {
      debug_msg("fopen failed!\n");
      free(p_out);
      return NULL;
    }

  if ((n_read = fread(p_out, 1, file_sz, fd)) != file_sz)
    {
      debug_msg("fread(): Could not read enough: %d vs %d!\n", n_read, file_sz);
      free(p_out);
      return NULL;
    }
  fclose(fd);

  return p_out;
}

#if 0
#include <vmgpsonyericsson.h>
#endif

void store_midifile(const char *filename, uint8_t *p_data, uint32_t size)
{
#if 0
  int fd;
  int n_read;

  fd = fopen(filename, STREAM_FILE | STREAM_READ);
  if (fd < 0)
    {
      fd = fopen(filename, STREAM_FILE | STREAM_WRITE | STREAM_BINARY | STREAM_CREATE);
      if (fd < 0)
	return; /* Error in opening! */
      vStreamWrite(fd, p_data, size);
      fclose(fd);
    }
  else
    fclose(fd);
#endif
}

void play_midifile(const char *filename)
{
#if 0
  static SYSCTL_SONYERICSSON_SOUND_INFO_t midisound;

  midisound.SoundFormat = SYSCTL_SONYERICSSON_SOUND_MIDI;
  midisound.StartOffset = 0;
  midisound.RepeatSound = 0;
  midisound.RepeatDelay = 0;

  /* Play the tune! */
  vStrCpy(midisound.Filename, filename);
  if (vSysCtl(SYSCTL_SONYERICSSON_SOUND, (uint32_t)&midisound) != SYSCTL_SONYERICSSON_SOUND_OK)
    {
      error_msg("Could not play sound!\n");
    }
#endif
}
