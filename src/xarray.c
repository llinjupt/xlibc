/* 
 * stolen from xen/libxc/freearray.c and modify something  lli_njupt@163.com 
 * 2013/02/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <alloca.h>
#include <assert.h>

#include "xarray.h"

/****************************************************************************
 *desc :  创建可扩展数组对象
 *input： size 数组的大小：可以包含元素的个数
          append_enable 数组是否可以自动增长：0，不可以；!0 可以
 
 *output: None
 *return: NULL 失败; !NULL 成功
****************************************************************************/
xarray_t *xarray_create(unsigned int size, int append_enable,
                        void (*free_f)(void *data),
                        int (*cmp_f)(void *data1, void *data2),
                        void (*dump_f)(void *data))
{
  assert(size > 0);

  xarray_t *array = malloc(sizeof(struct xarray));
  if(array == NULL)
  {
    perror("malloc array");
    return NULL;
  }

  array->size = size;
  array->append_enable = append_enable;
  array->count = 0;
  array->data = calloc(size, sizeof(void *));
  
  array->free_f = free_f;
  array->cmp_f = cmp_f;
  array->dump_f = dump_f;

  return array;
}

void xarray_dump(xarray_t *array)
{
  int i = 0;
  
  if(array == NULL)
  {
    printf("array is NULL!\n");
    return;
  }
  
  if(array->dump_f == NULL)
  {
    printf("array's dump func is NULL!\n");
    return;
  }
    
  for(; i < array->count; i++)
  {
    printf("slot[%d]:\n", i);
    array->dump_f(array->data[i]);
    printf("\n");
  }
}

void xarray_traverse(xarray_t *array, void (*func)(void *))
{
  int i = 0;
  
  if(array == NULL)
  {
    printf("array is NULL!\n");
    return;
  }

  for(; i < array->count; i++)
    func(array->data[i]);
}

/* traverse func with another parameter */
void xarray_traverse2(xarray_t *array, void *para, void (*func)(void *node, void *para))
{
  int i = 0;
  
  if(array == NULL)
  {
    printf("array is NULL!\n");
    return;
  }

  for(; i < array->count; i++)
    func(array->data[i], para);
}

/****************************************************************************
 *desc :  释放可扩展数组对象
 *input： array 要释放数组的指针
 
 *output: None
 *return: None
****************************************************************************/
void xarray_destroy(xarray_t *array)
{
  int i = 0;
  
  if(array == NULL)
    return;
  
  if(array->free_f)
  {
    for(; i < array->count; i++)
      array->free_f(array->data[i]);
  }
  
  free(array->data);
  free(array);
}

/****************************************************************************
 *desc :  扩展/压缩可扩展数组对象的大小
 *input： array 要扩展数组的指针
          extends 要扩展的大小，扩展后数组大小将增加 extends，可以为负数
 
 *output: None
 *return: -1，扩展失败，数组无改变；>=0 成功返回扩展后的数组大小
****************************************************************************/
int xarray_append(xarray_t *array, int extents)
{
  void **data;
  int newsize;
  
  newsize = array->size + extents;
  if(newsize <= 0)
  {
    array->size = 0;
    array->count = 0;
    free(array->data);
    array->data = NULL;
    
    return 0;
  }
  
  data = realloc(array->data, sizeof(void *) * newsize);
  if(data == NULL)
  {
    perror("realloc array");
    return -1;
  }
  
  array->data = data;
  if(extents > 0)
    memset(&array->data[array->size], 0, sizeof(void *) * extents);

  array->size = newsize;
  
  return newsize;
}

/****************************************************************************
 *desc :  设置 [index] 的元素数据为ptr
 *input： array 要设置元素的数组指针
          index 数组下标
          ptr   要设置的元素
          force 如果该下标元素不为空，是否用新的ptr强制替代

 *output: None
 *return: -1，失败，数组无改变；0 成功，设置时该 [index] 数据应该为空
****************************************************************************/
static int __xarray_set_index(xarray_t *array, unsigned int index,
                              void *ptr, int force)
{
  if(index < array->size)
  {
    if(array->data[index] != NULL
      && force == 0)
    {
      printf("Error: slot(%d) is not empty!\n", index);
      return -1;
    }
  }
  else
  {
    int newsize = 0;
    if(array->append_enable == 0)
    {
      printf("Error: index %d is too big but array not support append_enable!\n", index);
      return -1;
    }
    
    newsize = (array->size * 2 < index) ? index + 1 : array->size * 2;
    if(xarray_append(array, newsize - array->size) < 0)
      return -1;
  }
  
  if(index + 1 > array->count)
    array->count = index + 1;

  array->data[index] = ptr;

  return 0;
}

/****************************************************************************
 *desc :  无论[index]是否为空，都强制设置 [index] 的元素数据为 ptr，
 *input： array 要设置元素的数组指针
          index 数组下标
          ptr   要设置的元素
          
 *output: None
 *return: -1，失败，数组无改变；0 成功
****************************************************************************/
int xarray_fset_index(xarray_t *array, unsigned int index, void *ptr)
{
  return __xarray_set_index(array, index, ptr, 1);
}

/* 与xarray_fset_byindex类似，但如果 [index] 已经有元素，则返回-1 */
int xarray_set_index(xarray_t *array, unsigned int index, void *ptr)
{
  return __xarray_set_index(array, index, ptr, 0);
}

/****************************************************************************
 *desc :  向数组中添加新元素
 *input： array 要添加元素的数组指针
          ptr 新元素
          
 *output: None
 *return: -1，失败，数组无改变；0 成功
****************************************************************************/
int xarray_set(xarray_t *array, void *ptr)
{
  return xarray_set_index(array, array->count, ptr);
}

/****************************************************************************
 *desc :  向数组中批量添加新元素
 *input： array 要添加元素的数组指针
          ... 新元素列表，最后一个元素必须为NULL 
              example: xarray_vset(new, strdup("world"), NULL);

 *output: None
 *return: 返回成功设置的元素个数
****************************************************************************/
int xarray_vset(xarray_t *array, ...)
{
  va_list va;
  void *ptr;
  int ret;

  va_start(va, array);

  for(ret = 0; (ptr = va_arg(va, void *)); ret++)
  {
    if(xarray_set(array, ptr))
      break;
  }
  
  va_end(va);
  return ret;
}

void *xarray_find(xarray_t *array, void *data)
{
  int i = 0;
  
  if(array == NULL || data == NULL)
    return NULL;
  
  if(array->cmp_f)
  {
    for(; i < array->count; i++)
    {
      if(array->cmp_f(array->data[i], data) == 0)
        return array->data[i];
    }
  }

  return NULL;
}

void *xarray_find_byfunc(xarray_t *array, void *args, 
                         int (*cmp_f)(void *data, void *args))
{ 
  int i = 0;
    
  if(array == NULL || cmp_f == NULL)
    return NULL;
  
  for(; i < array->count; i++)
  {
    if(cmp_f(array->data[i], args) == 0)
      return array->data[i];
  }

  return NULL;
}

int xarray_find_index(xarray_t *array, void *data)
{
  int i = 0;
  
  if(array == NULL || data == NULL)
    return -1;

  if(array->cmp_f)
  {
    for(; i < array->count; i++)
    {
      if(array->cmp_f(array->data[i], data) == 0)
        return i;
    }
  }
  
  return -1;
}

/****************************************************************************
 *desc :  从数组中删除元素
 *input： array 要删除元素的数组指针
          index 要删除元素的下标

 *output: None
 *return: 返回被删除元素的指针
****************************************************************************/
void *xarray_remove_index(xarray_t *array, int index)
{
  void *ret = NULL;
  
  if(index >= array->size)
    return NULL;
  
  ret = array->data[index];
  array->data[index] = NULL;
  
  /* removed is at last */
  if(index == array->count - 1)
  {
    array->data[index] = NULL;
    array->count--;
  }
  else /* use memmove to keep the order */
  {
    memmove(array->data + index, array->data + index + 1, 
            (array->count - index - 1) * sizeof(void *));
    array->count--;
    array->data[array->count] = NULL;
  }

  if(array->append_enable
     && array->count <= array->size / 2)
    xarray_append(array, -array->count);  
  
  return ret;
}

int xarray_remove_data(xarray_t *array, void *data)
{
  int i = 0, index = 0;
  void *ret = NULL;

  if(array == NULL || data == NULL)
    return -1;

  if(index >= array->size)
    return -1;
  
  if(array->cmp_f)
  {
    for(i = 0; i < array->count; i++)
    {
      if(array->cmp_f(array->data[i], data) != 0)
        continue;
      
      ret = array->data[i];
      index = i;
      break;
    }
  }
  
  if(ret == NULL)
    return -1;

  /* removed is at last */
  if(index == array->count - 1)
  {
    array->data[index] = NULL;
    array->count--;
  }
  else /* use memmove to keep the order */
  {
    memmove(array->data + index, array->data + index + 1, 
            (array->count - index - 1) * sizeof(void *));
    array->count--;
    array->data[array->count] = NULL;
  }

  if(array->append_enable
     && array->count <= array->size / 2)
    xarray_append(array, -array->count);
  
  if(array->free_f)
    array->free_f(ret);

  return 0;
}

/****************************************************************************
 *desc :  从数组中删除最后一个非空元素
 *input： array 要删除元素的数组指针

 *output: None
 *return: 返回被删除元素的指针
****************************************************************************/
void *xarray_remove(xarray_t *array)
{
  return xarray_remove_index(array, array->count - 1);
}

/****************************************************************************
 *desc :  从数组中获取 index 下标元素
 *input： array 要获取元素的数组指针
          index 要获取元素的下标
      
 *output: None
 *return: 返回获取元素的指针
****************************************************************************/
void *xarray_get(xarray_t *array, int index)
{ 
  if(index >= array->size)
    return NULL;

  return array->data[index];
}

/****************************************************************************
 *desc :  从可扩展数组对象中返回数组指针，并销毁原数组对象
 *input： array 要获取元素数组的数据对性指针

 *output: None
 *return: 返回获取元素数组的指针，注意释放其中的元素
****************************************************************************/
void **xarray_contents(xarray_t *array)
{
  void **data;

  if(array == NULL)
    return NULL;
  
  data = array->data;
  free(array);
  
  return data;
}

unsigned int xarray_getcount(xarray_t *array)
{
  if(array == NULL)
    return 0;

  return array->count;
}

/* below is a sample for str array */
void strarray_dump(void *data)
{
  printf("%s", data ? (char *)data : "NULL");
}

static int strarray_cmp(void *a, void *b)
{
  return strcmp((char *)a, (char *)b);
}

static void strarray_free(void *data)
{
  if(data)
    free(data);
}

/* don't forget to free the returned str */
char* xarray2str(xarray_t *array, const char split)
{
  int i = 0;
  int len = 0, offset = 0;
  int size = 256;
  
  char *str = NULL;
  
  if(array == NULL)
    return NULL;

  str = malloc(size);
  memset(str, 0, size);
  if(!str)
  {
    perror("strarray2str malloc");
    return NULL;
  }

  for(; i < array->count; i++)
  {
    char *tmp = (char *)array->data[i];

    if(tmp == NULL 
      || !(len = strlen(tmp)))
      continue;
          
    if(size - offset <= len - 1)
    {
      char *data = NULL;
      
      if(size <= len - 1)
        size = size + len + 1;
      else
        size = size * 2;

      data = realloc(str, size);
      if(data == NULL)
      {
        perror("realloc array");
        free(str);
        return NULL;
      }
      
      str = data;
    }
    
    sprintf(str + offset, "%s%c", tmp, split);
    offset = offset + len + 1;
  }
  
  if(offset)
    str[offset - 1] = '\0';
  
  return str;
}

xarray_t *xstrarray()
{
  return xarray_create(8, 1, strarray_free, strarray_cmp, strarray_dump);
}

int xstrarrayadd(xarray_t *array, char *str)
{
  char *new = NULL;

  if(!str || !array)
    return -1;

  new = strdup(str);
  if(!new)
    return -1;
  
  if(xarray_vset(array, new, NULL) == 1)
    return 0;

  free(new); 
  return -1;
}

xarray_t *xstr2array(char *instr, const char *split)
{
  int len = 0;
  int i = 0, j = 0;

  char *tmp = NULL;
  xarray_t *new = NULL;

  len = strlen(instr);
  if(len == 0)
    return NULL;

  tmp = strdup(instr); 
  if(tmp == NULL)
  {
    perror("xtr2array strdup");
    return NULL;
  }
  
  new = xarray_create(8, 1, strarray_free, strarray_cmp, strarray_dump);
  if(new == NULL)
  {
    printf("xarray_create error\n");
    goto err;
  }

  for(; i <= len; i++)
  {
    if(strchr(split, instr[i]) == NULL 
      && instr[i] != '\0')
    {
      tmp[j++] = instr[i]; 
    }
    else
    {
      char *newstr = NULL;
      
      tmp[j] = '\0';
      j = 0;
      
      newstr = strdup(tmp);
      if(newstr == NULL 
        || xarray_vset(new, newstr, NULL) != 1)
        goto err;
    }
  }
  free(tmp);
  return new;

err:
  free(tmp);
  xarray_destroy(new);
  
  return NULL;
}

#ifdef TEST
void test_xarray()
{
  int ret = 0;
  
  xarray_t *str_array = xstr2array("0-1-2-3-4-5-6", "-");
  printf("count :%d\n", xarray_getcount(str_array));
  xarray_dump(str_array);
  
  char *removed = xarray_remove_index(str_array, 2);
  printf("\nafter remove index [2]:\n");
  printf("count :%d\n", xarray_getcount(str_array));
  xarray_dump(str_array);
  strarray_free(removed);
  
  removed = xarray_remove(str_array);
  printf("\nafter remove the last index:\n");
  printf("count :%d\n", xarray_getcount(str_array));
  xarray_dump(str_array);
  strarray_free(removed);

  ret = xarray_remove_data(str_array, "0");
  if(ret == 0)
  {
    printf("\nafter remove string \"0\":\n");
    printf("count :%d\n", xarray_getcount(str_array));
    xarray_dump(str_array);
  }
  else
    printf("\nremove string \"4\" failed!\n");

  xarray_vset(str_array, strdup("hello"), NULL);
  printf("\nafter add string \"hello\":\n");
  printf("count :%d\n", xarray_getcount(str_array));
  xarray_dump(str_array);

  xarray_destroy(str_array);
}
#endif
