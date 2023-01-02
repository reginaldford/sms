#include "sms.h"

sm_string *sm_object_type_str(enum object_type t){
  char *response_string[]={"sm_double","sm_expression","sm_primitive","sm_string","sm_symbol","sm_array","sm_context","sm_gco"};
  int response_string_len[]={10,13,12,9,9,8,10,6};
  if( t >= 0 && t < sizeof(response_string) / sizeof(void*) )
    return sm_new_string(response_string_len[t],response_string[t]);
  else
    return sm_new_string(10,"NoSuchType");
}

sm_string *sm_object_to_string(sm_object *obj1){
	enum object_type t = obj1->my_type;

	if(t==sm_double_type)
	{
		sm_double *n1 = (sm_double*)obj1;
		char string_space[64];
		sprintf(string_space,"%f",n1->value);
    return sm_new_string(strlen(string_space),string_space);
	}else if(t==sm_string_type)
	{
		sm_string *pss = (sm_string*)obj1;
		//2 more characters for quotes, and 1 for the extra null char
		char *str_buf = malloc(sizeof(char)*(pss->size+3));
    sprintf(str_buf,"\"%s\"",&(pss->content));
    sm_string *answer = sm_new_string(pss->size+2,str_buf);
    free(str_buf);
    return answer;
	} else if(t==sm_expression_type){
	  return sm_expression_to_string((sm_expression*)obj1);
	} else if(t==sm_symbol_type){
    return ((sm_symbol*)obj1)->name;
	} else if(t==sm_array_type){
		return sm_array_to_string((sm_array*)obj1);
	} else if(t==sm_context_type){
    return sm_context_to_string((sm_context*)obj1);
	} else {
	  return sm_new_string(5,"other");
	}
}

int sm_sizeof(sm_object *obj1)
{
  enum object_type obj_type = obj1->my_type;
  switch ( obj_type )
  {
    case sm_double_type: return sizeof(sm_double);
      break;
    case sm_expression_type: return sizeof(sm_expression) + sizeof(sm_object*)*((sm_expression*)obj1)->num_args;
      break;
    case sm_primitive_type: return sizeof(sm_expression);
      break;
    case sm_string_type: return sm_round_size(sizeof(sm_string) + ((sm_string*)obj1) -> size + 1);
      break;
    case sm_array_type: return sizeof(sm_array) + sizeof(void*) * ((sm_array*)obj1)->capacity;
      break;
    case sm_symbol_type: return sizeof(sm_symbol);
      break;
    case sm_context_type: return sizeof(sm_context) + sizeof(sm_table_entry) * ((sm_context*)obj1)->capacity;
      break;
    case sm_gco_type: return sizeof(sm_gco);
      break;
    default:
    	printf("Cannot determine size of object of type %d\n",obj_type);
  		exit(0);
  }
}
