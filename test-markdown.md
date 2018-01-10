# 这是一个测试文件
![](https://github.com/twytzhonghua/test2016/blob/master/test1.png)
```C

```
# 如何在行内修饰文字:删除、加粗、斜体、颜色
[专业主义](https://book.douban.com/subject/1790456/)
- 描述：这本书着重阐释了真正的专家必须具备的四种能力：**先见能力**、**构思能力**、**讨论的能力**、**适应矛盾**的能力，以丰富的案例和深刻的洞见警示人们*重新思考专业*的内涵与效用，培养并吸纳专业人才。
- 状态：~~已读完。~~
- 备注： <span style="color:red">分析、设计、实现、改进</span>.

```C
/*
** Create or reuse a zero-terminated string, first checking in the
** cache (using the string address as a key). The cache can contain
** only zero-terminated strings, so it is safe to use 'strcmp' to
** check hits.
*/
TString *luaS_new (lua_State *L, const char *str) {
  unsigned int i = point2uint(str) % STRCACHE_N;  /* hash */
  int j;
  TString **p = G(L)->strcache[i];
  for (j = 0; j < STRCACHE_M; j++) {
    if (strcmp(str, getstr(p[j])) == 0)  /* hit? */
      return p[j];  /* that is it */
  }
  /* normal route */
  for (j = STRCACHE_M - 1; j > 0; j--)
    p[j] = p[j - 1];  /* move out last element */
  /* new element is first in the list */
  p[0] = luaS_newlstr(L, str, strlen(str));
  return p[0];
}
```
