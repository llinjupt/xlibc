/* 
 * copied from xen/libxc/freearray.c and modify something  lli_njupt@163.com 
 * 2013/02/20
*/

/*
 * Copyright (C) 2009      Citrix Ltd.
 * Author Vincent Hanquez <vincent.hanquez@eu.citrix.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 only. with the special
 * exception on linking described in file LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

#ifndef XARRAY_H
#define XARRAY_H

typedef struct xarray
{
  /* 数组的大小 */
  unsigned int size;
  
  /* 是否支持弹性大小调整: 0 or !0 */
  int append_enable;
  
  /* 当前包含的元素个数 */
  unsigned int count;
  
  /* 释放内部元素的钩子函数 */
  void (*free_f)(void *data);
  
  /* 比较内部元素的钩子函数 */
  int (*cmp_f)(void *data1, void *data2);
  
  /* 打印内部元素的钩子函数 */
  void (*dump_f)(void *data);
  
  /* 动态分配的数组指针 */
  void **data; 
} xarray_t;

xarray_t *xarray_create(unsigned int size, int append_enable, 
                        void (*free_f)(void *data),
                        int (*cmp_f)(void *data1, void *data2),
                        void (*dump_f)(void *data));
void xarray_destroy(xarray_t *array);

int xarray_append(xarray_t *array, int extents);
int xarray_fset_index(xarray_t *array, unsigned int index, void *ptr);
int xarray_set_index(xarray_t *array, unsigned int index, void *ptr);

int xarray_set(xarray_t *array, void *ptr);
int xarray_vset(xarray_t *array, ...);

void *xarray_get(xarray_t *array, int index);
void *xarray_remove_index(xarray_t *array, int index);
void *xarray_remove(xarray_t *array);
int xarray_remove_data(xarray_t *array, void *data);

void *xarray_find(xarray_t *array, void *data);
int xarray_find_index(xarray_t *array, void *data);
void *xarray_find_byfunc(xarray_t *array, void *args, 
                         int (*cmp_f)(void *data, void *args));
void xarray_dump(xarray_t *array);

void xarray_traverse(xarray_t *array, void (*func)(void *));
void xarray_traverse2(xarray_t *array, void *para, 
                      void (*func)(void *node, void *para));

unsigned int xarray_getcount(xarray_t *array);
void **xarray_contents(xarray_t *array);

/**** below is a sample for str array ****/

/* create a new str array */
#define XSTRARRAY_SPLIT     '\3'
#define XSTRARRAY_SPLIT_STR "\3"

xarray_t *xstrarray();
int xstrarrayadd(xarray_t *array, char *str);
char* xarray2str(xarray_t *array, const char split);
xarray_t *xstr2array(char *instr, const char *split);

#endif /* XARRAY_H */
