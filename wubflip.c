/*

Wubflip v1.0

(C) Copyright 2009 Alex Sisson (alexsisson@gmail.com)

This program is free software; you can redistribute it and/or modify
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>
#include "ladspa.h"

LADSPA_Descriptor *LD = NULL;

struct wubflip_t {
  LADSPA_Data *in;
  LADSPA_Data *out;
  LADSPA_Data *upper;
  LADSPA_Data *lower;
  LADSPA_Data *multiplier;
};

LADSPA_Handle wubflip_instantiate(const LADSPA_Descriptor *D, unsigned long sr) {
  return malloc(sizeof(struct wubflip_t));
}

void wubflip_activate(LADSPA_Handle handle) {
}

void wubflip_connectport(LADSPA_Handle handle, unsigned long port, LADSPA_Data *data) {
  struct wubflip_t *w = (struct wubflip_t*)handle;
  switch (port) {
    case 0: w->in         = data; break;
    case 1: w->out        = data; break;
    case 2: w->upper      = data; break;
    case 3: w->lower      = data; break;
    case 4: w->multiplier = data; break;
  }
}

void wubflip_run(LADSPA_Handle handle, unsigned long n) {
  unsigned long i;
  struct wubflip_t *w = (struct wubflip_t*)handle;
  LADSPA_Data u = *(w->upper);
  LADSPA_Data l = *(w->lower);
  LADSPA_Data m = *(w->multiplier);
  LADSPA_Data d;

  if(u<=l) {
    u = l;
    *(w->upper) = u;
    *(w->lower) = l;
  }

  for(i=0;i<n;i++) {
    w->out[i] = w->in[i];
    if(w->out[i] > u) {
      d = w->out[i] - u;
      w->out[i] -= m * d;
    }
    else if(w->out[i] < l) {
      d = l - w->out[i];
      w->out[i] += m * d;
    }
  }

  for(i=0;i<n;i++) {
    if(w->out[i] > u)
      w->out[i] = u;
    if(w->out[i] < l)
      w->out[i] = l;
  }
}

void wubflip_cleanup(LADSPA_Handle handle) {
  free(handle);
}

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index) {
  char **pn;
  LADSPA_PortDescriptor *pd;
  LADSPA_PortRangeHint *prh;

  if(index!=0)
    return NULL;
  if(LD)
    return LD;

  LD = malloc(sizeof(LADSPA_Descriptor));
  if(!LD)
    return NULL;

  LD->UniqueID   = 4104;
  LD->Label      = strdup("WubFlip");
  LD->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
  LD->Name       = strdup("WubFlip");
  LD->Maker      = strdup("Alex Sisson (alexsisson@gmail.com)");
  LD->Copyright  = strdup("(C) Alex Sisson 2009");
  LD->PortCount  = 5;

  pd = malloc(sizeof(LADSPA_PortDescriptor)*LD->PortCount);
  pd[0] = LADSPA_PORT_AUDIO   | LADSPA_PORT_INPUT;
  pd[1] = LADSPA_PORT_AUDIO   | LADSPA_PORT_OUTPUT;
  pd[2] = LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT;
  pd[3] = LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT;
  pd[4] = LADSPA_PORT_CONTROL | LADSPA_PORT_INPUT;
  LD->PortDescriptors = pd;

  pn = malloc(sizeof(char*)*LD->PortCount);
  pn[0] = strdup("Input");
  pn[1] = strdup("Output");
  pn[2] = strdup("Upper Threshold");
  pn[3] = strdup("Lower Threshold");
  pn[4] = strdup("Multiplier");
  LD->PortNames = (const char **)pn;

  prh = malloc(sizeof(LADSPA_PortRangeHint)*LD->PortCount);
  prh[0].HintDescriptor  = 0;
  prh[1].HintDescriptor  = 0;
  prh[2].HintDescriptor  = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MAXIMUM;
  prh[2].LowerBound      = -1;
  prh[2].UpperBound      = 1;
  prh[3].HintDescriptor  = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM;
  prh[3].LowerBound      = -1;
  prh[3].UpperBound      = 1;
  prh[4].HintDescriptor  = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
  prh[4].LowerBound      = 0;
  prh[4].UpperBound      = 10;
  LD->PortRangeHints     = prh;

  LD->instantiate         = wubflip_instantiate;
  LD->activate            = wubflip_activate;
  LD->connect_port        = wubflip_connectport;
  LD->run                 = wubflip_run;
  LD->cleanup             = wubflip_cleanup;
  LD->run_adding          = NULL;
  LD->set_run_adding_gain = NULL;
  LD->deactivate          = NULL;
  return LD;
}

static void __attribute__((constructor)) init() {
}

static void __attribute__((destructor)) fini() {
  if(LD) {
    free(LD);
    LD = NULL;
  }
}
