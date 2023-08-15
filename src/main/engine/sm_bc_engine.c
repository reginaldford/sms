// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

#include "../sms.h"

extern sm_symbol *sms_true;
extern sm_symbol *sms_false;

sm_object *sm_bc_eval(sm_bc_block *block) {
  // The bytecode block is assumed to be safely designed
  int pos = 0;
  for (;;) {
    switch ((&block->content)[pos++]) {}
  }
  return (sm_object *)sms_true;
}
