/* without extern, gcc will complain "undefined reference to `inc'" */
extern inline int inc (int *a)
{
  return (*a)++;
}

