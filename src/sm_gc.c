#include "sms.h"

sm_object* sm_move_to_new_heap(sm_object *obj)
{
  	sm_object* new_obj = sm_realloc(obj,sm_sizeof(obj));
  	// sm_object* new_obj = sm_realloc_obj(obj);

  	sm_gco* new_gco = (sm_gco*)obj;
  	new_gco->my_type = sm_gco_type;
  	new_gco->new_address = new_obj;
  	return new_obj;
}

sm_object *sm_meet_object(sm_object * obj)
{
	enum object_type the_type = obj->my_type;
	if(the_type==sm_gco_type)
		return ((sm_gco*)obj)->new_address;
	else
  	return  sm_move_to_new_heap(obj);
}

void sm_inflate_heap()
{
	//inflate new space. 'meet' every ptr,
	//meeting will copy to new heap if necessary
	char* scan_cursor = (char*)sm_global_current_heap(NULL)->storage;
	while(scan_cursor < ((char*)sm_global_current_heap(NULL)->storage) + sm_global_current_heap(NULL)->used)
	{
		sm_object * current_obj = (sm_object*)scan_cursor;
		switch(current_obj->my_type)
		{
			case sm_context_type:
			{
				sm_table_entry * table = get_table_entries((sm_context*)current_obj);
				for(int i = 0; i < ((sm_context*)current_obj) -> size; i ++)
				{
					table[i].name  = (sm_string*) sm_meet_object( (sm_object*) table[i].name);
					table[i].value = sm_meet_object(table[i].value);
				}
				break;
			}
			case sm_expression_type:
      {
        sm_expression *expr = (sm_expression*)current_obj;
      	for(int i = 0;i < expr -> num_args; i++)
      	{
      	  sm_expression * new_expr = (sm_expression*)sm_meet_object(sm_get_expression_arg(expr,i));
      	  sm_set_expression_arg(expr,i,(sm_object*)new_expr);
      	}
        break;
      }
      case sm_symbol_type:
      {
        sm_symbol *sym = (sm_symbol*)current_obj;
      	sym->name = (sm_string*) sm_meet_object((sm_object*)sym->name);
        break;
      }
      case sm_array_type:
      {
      	sm_array *arr = (sm_array*)current_obj;
      	for(int i = 0;i < arr -> size;i ++)
      	{
      	  sm_object * new_obj  = sm_meet_object(sm_array_get(arr,i));
      		sm_array_set(arr,i,new_obj);
      	}
        break;
      }

			default:
			  break;
		}
		scan_cursor += sm_sizeof(current_obj);
	}
}

//copying gc
void sm_garbage_collect()
{
	if(sm_global_current_heap(NULL) -> used != 0)
	{
		//build new space if necessary, same size as current
    if(sm_global_other_heap(NULL) == NULL)
      sm_global_other_heap(sm_new_memory_heap(sm_global_current_heap(NULL)->capacity));

    //swap heaps now
		sm_global_other_heap(sm_global_current_heap(sm_global_other_heap(NULL)));

    //consider this heap empty now
    sm_global_current_heap(NULL) -> used = 0;

    //copy roots
    sm_move_to_new_heap((sm_object*)sm_global_context(NULL));

    //update global variables
    sm_global_context((sm_context*)((sm_gco*)sm_global_context(NULL)) -> new_address);
    
    //inflate
    sm_inflate_heap();

    //for tracking purposes
    sm_gc_count(1);
	}

	printf("%i bytes used after gc.\n",sm_global_current_heap(NULL)->used);

}
