// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_img {
  sm_env env;
  // what about sms_all_heaps?
  sm_heap   sms_heap;
  sm_symbol sms_true;
  sm_symbol sms_false;
  sm_heap   sms_symbol_heap;
} sm_img;
