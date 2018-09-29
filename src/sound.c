/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      sound.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Sound functionality
 *
 * $Id: sound.c 11335 2006-09-27 17:47:00Z ska $
 *
 ********************************************************************/
#include <boulder.h>

void sound_play(game_t *p_game, sound_type_t which)
{
  if (!p_game->conf.sound)
    return;
  switch(which)
    {
    case EXPLOSION:
    	// Something here
      break;
    case DIAMOND_TAKEN:
      break;
    default:
      break;
    }
}
