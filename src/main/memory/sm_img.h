// Read https://raw.githubusercontent.com/reginaldford/sms/main/LICENSE.txt for license information

typedef struct sm_img {
  sm_env    env;
  sm_heap   sms_heap;
  sm_symbol sms_true;
  sm_symbol sms_false;
  sm_heap   sms_symbol_heap;
  sm_heap   sms_symbol_name_heap;
  uint32_t  num_symbols;
} sm_img;

sm_object *sm_image_save(sm_string *fname_str, sm_expr *sme);
