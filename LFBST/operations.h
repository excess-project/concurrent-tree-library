#ifndef operations_h
#define operations_h


bool search(thread_data_t * data, size_t key);
	
bool insert(thread_data_t * data, size_t key);

bool delete_node(thread_data_t * data, size_t key);

void mark_Node(volatile AO_t * word);

size_t in_order_visit(node_t * rootNode);

#endif
