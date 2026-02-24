// Gingoduino — Music Theory Library for Embedded Systems
// Umbrella header — includes all available modules based on tier.
//
// SPDX-License-Identifier: MIT

#ifndef GINGODUINO_H
#define GINGODUINO_H

#include "gingoduino_config.h"
#include "gingoduino_types.h"

// Tier 1: Note, Interval, Chord
#if GINGODUINO_HAS_NOTE
  #include "GingoNote.h"
#endif

#if GINGODUINO_HAS_INTERVAL
  #include "GingoInterval.h"
#endif

#if GINGODUINO_HAS_CHORD
  #include "GingoChord.h"
#endif

// Tier 2: Scale, Field, Duration, Tempo, TimeSignature
#if GINGODUINO_HAS_SCALE
  #include "GingoScale.h"
#endif

#if GINGODUINO_HAS_FIELD
  #include "GingoField.h"
#endif

#if GINGODUINO_HAS_DURATION
  #include "GingoDuration.h"
#endif

#if GINGODUINO_HAS_TEMPO
  #include "GingoTempo.h"
#endif

#if GINGODUINO_HAS_TIMESIG
  #include "GingoTimeSig.h"
#endif

// Tier 2+: Fretboard
#if GINGODUINO_HAS_FRETBOARD
  #include "GingoFretboard.h"
#endif

// Tier 3: Event, Sequence, Tree, Progression
#if GINGODUINO_HAS_EVENT
  #include "GingoEvent.h"
#endif
#if GINGODUINO_HAS_SEQUENCE
  #include "GingoSequence.h"
#endif
#if GINGODUINO_HAS_TREE
  #include "GingoTree.h"
#endif
#if GINGODUINO_HAS_PROGRESSION
  #include "GingoProgression.h"
#endif

#endif // GINGODUINO_H
